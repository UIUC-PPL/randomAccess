#include "randomAccess.decl.h"

#ifdef LONG_IS_64BITS
#define ZERO64B 0L
#define POLY 0x0000000000000007UL
#define PERIOD 1317624576693539401L
#else
#define ZERO64B 0LL
#define POLY 0x0000000000000007ULL
#define PERIOD 1317624576693539401LL
#endif

#define  UPDATE_QUIESCENCE  0
#define  VERIFY_QUIESCENCE 1

/* Readonly variables */
CProxy_Main mainProxy;
CProxy_Updater updater_array;

int logLocalTableSize;
CmiInt8 localTableSize;
CmiInt8 tableSize;
int numOfUpdaters;

CmiInt8 HPCC_starts(CmiInt8 n);
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
        //CkPrintf("Usage: RandomAccess logLocaltablesize %d   %d\n", sizeof(CmiInt8), sizeof(CmiInt8));
        logLocalTableSize = atoi(args->argv[1]);
        delete args;
        numOfUpdaters = CkNumPes();
        localTableSize = 1l << logLocalTableSize;
        tableSize = localTableSize * numOfUpdaters ;
        CkPrintf("Main table size   = 2^%d * %d = %lld words\n", logLocalTableSize, CkNumPes(), tableSize);
        CkPrintf("Number of processes = %d\n", CkNumPes());
        CkPrintf("Number of updates = %lld\n", (4*tableSize));
        mainProxy = thishandle;
        mainhandle = thishandle;  
        updater_array   = CProxy_Updater::ckNew(numOfUpdaters);
        updater_array.initialize();
    }

    // start to update 
    void start(CkReductionMsg *msg)
    {
        CkPrintf("\nstart RandomAccess\n");
        delete msg;
        starttime = CkWallTimer();
        updater_array.generateUpdates();
        phase = UPDATE_QUIESCENCE;      //randomAccess phase 
        CkStartQD(CkIndex_Main::allUpdatesDone((DUMMYMSG *)0), &mainhandle);
    }
    void allUpdatesDone(DUMMYMSG *msg)
    {
        delete msg;
        double singlegups;
        double gups;
        double update_walltime;
        
        update_walltime = CkWallTimer() - starttime;
        if(phase == UPDATE_QUIESCENCE)
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
            //verify done
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

class Updater : public CBase_Updater {
private:
    CmiInt8 *HPCC_Table;
    CmiInt8 globalStartmyProc;
public:
    Updater() {}
    Updater(CkMigrateMessage* m) {}
    void initialize(){
        globalStartmyProc = thisIndex * localTableSize  ;
        HPCC_Table = (CmiInt8*)malloc(sizeof(CmiInt8) * localTableSize);
        for(CmiInt8 i=0; i<localTableSize; i++)
            HPCC_Table[i] = i + globalStartmyProc;
        contribute(CkCallback(CkIndex_Main::start(NULL), mainProxy)); 
    }

    void generateUpdates()
    {
        CmiInt8 updatesNum;
        CmiInt8 ran, localOffset;
        int tableIndex;
        ran= HPCC_starts(4* globalStartmyProc);
        updatesNum = 4 * localTableSize;
        for(CmiInt8 i=0; i<updatesNum;i++)
        {
            ran = (ran << 1) ^ ((CmiInt8) ran < ZERO64B ? POLY : ZERO64B);
            tableIndex = (ran >>  logLocalTableSize)&(numOfUpdaters-1);
            if(tableIndex == thisIndex)
            {
                localOffset = (ran&(tableSize-1))-globalStartmyProc;
                HPCC_Table[localOffset] ^= ran;
            }
            else {
                //sending messages out and receive message to apply the update table
                updater_array[tableIndex].updateLocalTable(ran);
                if(i%1024 == 0)
                    CthYield();   
            }
            if(i%(1024*1024*64) == 0)
                CkPrintf("[%d] iter=%d\n", CkMyPe(), i/(1024*1024));
        }
    }

    void updateLocalTable( CmiInt8 ran)
    {
        CmiInt8 localOffset;
        localOffset = ran & (localTableSize - 1);
        HPCC_Table[localOffset] ^= ran;
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
CmiInt8 HPCC_starts(CmiInt8 n)
{
    int i, j;
    CmiInt8 m2[64];
    CmiInt8 temp, ran;
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
