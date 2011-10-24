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

#define DATA_ITEM_SIZE 8
#define NUM_MESSAGES_BUFFERED 1024
#define FLUSH_PERIOD_IN_MS 10

CProxy_Main     mainProxy;
int             N;                  //log local table size    
CmiInt8         localTableSize;
CmiInt8         tableSize;
CProxy_MeshStreamer aggregator; 

CmiUInt8 HPCC_starts(CmiInt8 n);

class Main : public CBase_Main {
private:
    CProxy_Updater  updater_array;
    double starttime;
public:
    Main(CkArgMsg* args) {
        int NUM_ROWS = 2;
        int NUM_COLUMNS= 2;
        int NUM_PLANES= 1;
        int NUM_PES_PER_NODE;
        TopoManager tmgr;
        N = atoi(args->argv[1]);
        NUM_PES_PER_NODE = CkMyNodeSize();
        //use this if you do not want to differentiate based on core ID's
        NUM_ROWS = tmgr.getDimNX()*tmgr.getDimNT();
        NUM_COLUMNS = tmgr.getDimNY();
        NUM_PLANES = tmgr.getDimNZ();
        CkPrintf("Running on NX %d NY %d NZ %d cores_per_node %d\n",NUM_ROWS,NUM_COLUMNS,NUM_PLANES,NUM_PES_PER_NODE);

        delete args;
        localTableSize = 1l << N;
        tableSize = localTableSize * CkNumPes();
        CkPrintf("Main table size   = 2^%d * %d = %lld words\n", N, CkNumPes(), tableSize);
        CkPrintf("Number of processors = %d\n", CkNumPes());
        CkPrintf("Number of updates = %lld\n", (4*tableSize));
        mainProxy = thishandle;
        //initialize the global table 
        updater_array   = CProxy_Updater::ckNew();
        aggregator = CProxy_MeshStreamer::ckNew(DATA_ITEM_SIZE, NUM_MESSAGES_BUFFERED, NUM_ROWS, NUM_COLUMNS, NUM_PLANES, updater_array);
    }
    // start RandomAccess
    void start(CkReductionMsg *msg)
    {
        delete msg;
        starttime = CkWallTimer();
        updater_array.generateUpdates();
        CkStartQD(CkCallback(CkIndex_Main::startFlush(), mainProxy));
    }
    //flush the messages that sit in the mesh streamer buffer
    void startFlush()
    {
        aggregator.flushDirect();
        CkStartQD(CkCallback(CkIndex_Main::allUpdatesDone(), mainProxy));
    }
    void allUpdatesDone()
    {
        double update_walltime = CkWallTimer() - starttime;
        double gups = 1e-9 * tableSize * 4.0/update_walltime;
        double singlegups =  gups/CkNumPes();
        CkPrintf( "CPU time used = %.6f seconds\n", update_walltime );
        CkPrintf( "%.9f Billion(10^9) Updates    per second [GUP/s]\n",  gups);
        CkPrintf( "%.9f Billion(10^9) Updates/PE per second [GUP/s]\n", singlegups );
        // repeat the update to verify 
        updater_array.generateUpdates();
        CkStartQD(CkCallback(CkIndex_Main::startFlushVerify(), mainProxy));
    }
    
    void startFlushVerify()
    {
        aggregator.flushDirect();
        CkStartQD(CkCallback(CkIndex_Updater::checkErrors(), updater_array));
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
public:
    Updater() {
        globalStartmyProc = CkMyPe()* localTableSize  ;
        HPCC_Table = (CmiUInt8*)malloc(sizeof(CmiUInt8) * localTableSize);
        for(CmiInt8 i=0; i<localTableSize; i++)
            HPCC_Table[i] = i + globalStartmyProc;
        contribute(CkCallback(CkIndex_Main::start(NULL), mainProxy)); 
    }

    Updater(CkMigrateMessage* m) {}
    void generateUpdates()
    {
        CmiUInt8 ran= HPCC_starts(4* globalStartmyProc);
        CmiUInt8 updatesNum = 4 * localTableSize;
        for(CmiInt8 i=0; i<updatesNum;i++)
        {
            ran = (ran << 1) ^ ((CmiInt8) ran < ZERO64B ? POLY : ZERO64B);
            int tableIndex = (ran >>  N)&(CkNumPes()-1);
            if(tableIndex ==  CkMyPe())
            {
                CmiUInt8 localOffset = (ran&(tableSize-1))-globalStartmyProc;
                HPCC_Table[localOffset] ^= ran;
            }
            else {
                //sending messages out and receive message to apply the update table
                ((MeshStreamer *)CkLocalBranch(aggregator))->insertData(&ran, tableIndex);
                if(i%1024 == 0) CthYield();   
            }
        }
    }
    //receive remote updates and update the table
    void receiveCombinedData(MeshStreamerMessage *msg) 
    {
        for (int i = 0; i < msg->numDataItems; i++) {
            CmiUInt8 ran = ((CmiUInt8*)(msg->data))[i];
            CmiInt8  localOffset = ran & (localTableSize - 1);
            HPCC_Table[localOffset] ^= ran;
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
