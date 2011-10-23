#include "randomAccess.decl.h"
#include "MeshStreamer.h"
#include "TopoManager.h"

#ifdef LONG_IS_64BITS
#define ZERO64B 0L
#define POLY 0x0000000000000007UL
#define PERIOD 1317624576693539401L
#else
#define ZERO64B 0LL
#define POLY 0x0000000000000007ULL
#define PERIOD 1317624576693539401LL
#endif

#define  FLUSH_QUIESCENCE   0
#define  UPDATE_QUIESCENCE  1
#define  VERIFY_QUIESCENCE 2
#define  FLUSH_VERIFY_QUIESCENCE   3

#define PAYLOAD_SIZE 8
#define BUCKET_SIZE 1024
#define FLUSH_PERIOD_IN_MS 10


/* Readonly variables */
CProxy_Main mainProxy;
CProxy_Updater updater_array;
CProxy_MeshStreamer aggregator; 

int logLocalTableSize;
CmiInt8 localTableSize;
CmiInt8 tableSize;
int numOfUpdaters;

CmiUInt8 HPCC_starts(CmiInt8 n);
class DUMMYMSG : public CMessage_DUMMYMSG {
};

class Main : public CBase_Main {
private:
    CkChareID mainhandle;
    double starttime;
    int phase;    

public:
    Main(CkArgMsg* args) 
    {
        //default 256 cores
        int NUM_ROWS = 2;
        int NUM_COLUMNS= 2;
        int NUM_PLANES= 1;
        int NUM_PES_PER_NODE;
	TopoManager tmgr;

        NUM_PES_PER_NODE = CkMyNodeSize();
        //CkPrintf("Usage: RandomAccess logLocaltablesize %d   %d\n", sizeof(CmiInt8), sizeof(CmiInt8));
        logLocalTableSize = atoi(args->argv[1]);
        if(args->argc>2)
        {
            //use this if you do not want to differentiate based on core ID's
	    NUM_ROWS = tmgr.getDimNX()*tmgr.getDimNT();
            //use this if you want have specific task for each core
	    //NUM_ROWS = tmgr.getDimNX();
            NUM_COLUMNS = tmgr.getDimNY();
            NUM_PLANES = tmgr.getDimNZ();
            /*NUM_ROWS = atoi(args->argv[2]);
            NUM_COLUMNS = atoi(args->argv[3]);
            NUM_PLANES = atoi(args->argv[4]);*/
        }
        delete args;
        numOfUpdaters = CkNumPes();
        localTableSize = 1l << logLocalTableSize;
        tableSize = localTableSize * numOfUpdaters ;
        CkPrintf("Main table size   = 2^%d * %d = %lld words\n", logLocalTableSize, CkNumPes(), tableSize);
        CkPrintf("Number of processes = %d\n", CkNumPes());
        CkPrintf("Number of updates = %lld\n", (4*tableSize));
        mainProxy = thishandle;
        mainhandle = thishandle;  
        updater_array   = CProxy_Updater::ckNew();
        int totalBufferCapacity = BUCKET_SIZE * (NUM_ROWS + NUM_COLUMNS + NUM_PLANES - 2); 
        aggregator = 
            CProxy_MeshStreamer::ckNew(PAYLOAD_SIZE, totalBufferCapacity,
                NUM_ROWS, NUM_COLUMNS, NUM_PLANES, 
                NUM_PES_PER_NODE, 
                updater_array,  
                FLUSH_PERIOD_IN_MS);

        updater_array.initialize();
    }

    // start to update 
    void start(CkReductionMsg *msg)
    {
        CkPrintf("\nstart RandomAccess\n");
        delete msg;
        starttime = CkWallTimer();
        updater_array.generateUpdates();
        //phase = UPDATE_QUIESCENCE;
        phase = FLUSH_QUIESCENCE;
        CkStartQD(CkIndex_Main::allUpdatesDone((DUMMYMSG *)0), &mainhandle);
    }
    void allUpdatesDone(DUMMYMSG *msg)
    {
        delete msg;
        double singlegups;
        double gups;
        double update_walltime;
        
        update_walltime = CkWallTimer() - starttime;
        if(phase == FLUSH_QUIESCENCE)
        {
            CkPrintf( "Before flushing CPU time used = %.6f seconds\n", update_walltime);
            aggregator.flushDirect();
            phase = UPDATE_QUIESCENCE;      //randomAccess phase 
            CkStartQD(CkIndex_Main::allUpdatesDone((DUMMYMSG *)0), &mainhandle);
        }
        else if(phase == UPDATE_QUIESCENCE)
        {
            gups = 1e-9 * tableSize * 4.0/update_walltime;
            singlegups =  gups/numOfUpdaters;
            CkPrintf( "CPU time used = %.6f seconds\n", update_walltime );
            CkPrintf( "%.9f Billion(10^9) Updates    per second [GUP/s]\n",  gups);
            CkPrintf( "%.9f Billion(10^9) Updates/PE per second [GUP/s]\n", singlegups );
            CkPrintf("\n\nStart verifying...\n");
            starttime = CkWallTimer();
            phase = VERIFY_QUIESCENCE;      //verify
            updater_array.generateUpdates();
            CkStartQD(CkIndex_Main::allUpdatesDone((DUMMYMSG *)0), &mainhandle);
        }else if(phase == VERIFY_QUIESCENCE)
        {
            //verify done, flush msgs
            aggregator.flushDirect();
            phase = FLUSH_VERIFY_QUIESCENCE;      //randomAccess phase 
            CkStartQD(CkIndex_Main::allUpdatesDone((DUMMYMSG *)0), &mainhandle);
        }else if(phase == FLUSH_VERIFY_QUIESCENCE)
        {
            updater_array.checkErrors(); 
        }
    }

    void verifyDone(CkReductionMsg *msg) 
    {
        CmiInt8 GlbnumErrors = *(CmiInt8*)msg->getData();
        CkPrintf(  "Found %lld errors in %lld locations (%s).\n", GlbnumErrors, 
            tableSize, (GlbnumErrors <= 0.01*tableSize) ? "passed" : "failed");
        delete msg;
        CkExit();
    }
};

class Updater : public MeshStreamerClient {
private:
    CmiUInt8 *HPCC_Table;
    CmiUInt8 globalStartmyProc;
    int send_cnt;
    int recv_cnt;
public:
    Updater() {}
    Updater(CkMigrateMessage* m) {}
    void initialize(){
        globalStartmyProc = CkMyPe()* localTableSize  ;
        HPCC_Table = (CmiUInt8*)malloc(sizeof(CmiUInt8) * localTableSize);
        for(CmiInt8 i=0; i<localTableSize; i++)
            HPCC_Table[i] = i + globalStartmyProc;
        contribute(CkCallback(CkIndex_Main::start(NULL), mainProxy)); 
        send_cnt = recv_cnt = 0;
    }

    void generateUpdates()
    {
        CmiInt8 updatesNum;
        CmiUInt8 ran, localOffset;
        int tableIndex;
        MeshStreamerMessage *msg;
        ran= HPCC_starts(4* globalStartmyProc);
        updatesNum = 4 * localTableSize;
        for(CmiInt8 i=0; i<updatesNum;i++)
        {
            ran = (ran << 1) ^ ((CmiInt8) ran < ZERO64B ? POLY : ZERO64B);
            tableIndex = (ran >>  logLocalTableSize)&(numOfUpdaters-1);
            //CkPrintf("+++[%d==%d] ran=%lld\n", CkMyPe(), tableIndex, ran);
            if(tableIndex ==  CkMyPe())
            {
                localOffset = (ran&(tableSize-1))-globalStartmyProc;
                HPCC_Table[localOffset] ^= ran;
            }
            else {
                //CkPrintf("[%d][%d==%llu]\n", CkMyPe(), tableIndex, ran);
                //sending messages out and receive message to apply the update table
                msg = new (1, PAYLOAD_SIZE) MeshStreamerMessage(PAYLOAD_SIZE);
                msg->addData((void *) &ran, tableIndex);
                aggregator[CkMyNode()].insertData(msg);      
                send_cnt++;
                if(i%1024 == 0)
                    CthYield();   
            }
        }
    }

    //void updateLocalTable( CmiInt8 ran)
    void receiveCombinedData(LocalMessage *msg) 
    {
      // CkPrintf("[%d] receiving message for %d \n", CkMyPe()), ((int *) msg->data)[0];
      for (int i = 0; i < msg->numElements; i++) {
        CmiInt8 localOffset;
        CmiUInt8 ran = ((CmiUInt8*)(msg->data))[i];
        localOffset = ran & (localTableSize - 1);
        HPCC_Table[localOffset] ^= ran;
        //CkPrintf("[%d==%llu]++\n", CkMyPe(), ran);
        recv_cnt++;
      }

      delete msg;
    }

    void checkErrors()
    {

        CmiInt8 numErrors = 0;
        for (CmiInt8 j=0; j<localTableSize; j++)
            if (HPCC_Table[j] != j + globalStartmyProc)
                numErrors++;
        contribute(sizeof(CmiInt8), &numErrors, CkReduction::sum_long, CkCallback(CkIndex_Main::verifyDone(NULL), mainProxy)); 
    }
};

/** random generator */
CmiUInt8 HPCC_starts(CmiInt8 n)
{
    int i, j;
    CmiUInt8 m2[64];
    CmiUInt8 temp, ran;
    while (n < 0) n += PERIOD;
    while (n > PERIOD) n -= PERIOD;
    if (n == 0) return 0x1;
    temp = 0x1;
    for (i=0; i<64; i++) {
        m2[i] = temp;
        temp = (temp << 1) ^ ((CmiInt8) temp < 0 ? POLY : 0);
        temp = (temp << 1) ^ ((CmiInt8) temp < 0 ? POLY : 0);
    }
    for (i=62; i>=0; i--)
        if ((n >> i) & 1)
            break;

    ran = 0x2;
    while (i > 0) {
        temp = 0;
        for (j=0; j<64; j++)
            if ((ran >> j) & 1)
                temp ^= m2[j];
        ran = temp;
        i -= 1;
        if ((n >> i) & 1)
            ran = (ran << 1) ^ ((CmiInt8) ran < 0 ? POLY : 0);
    }
    return ran;

}

#include "randomAccess.def.h"
