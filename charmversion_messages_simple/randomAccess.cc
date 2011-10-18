#include "randomAccess.decl.h"

#define ZERO64B 0L
#define POLY 0x0000000000000007UL
#define PERIOD 1317624576693539401L
#define  UPDATE_QUIESCENCE  0
#define  VERIFY_QUIESCENCE 1

/* Readonly variables */
CProxy_Main mainProxy;
CProxy_Updater updater_array;

int logLocalTableSize;
long localTableSize;
long tableSize;
int numOfUpdaters;

long HPCC_starts(long n);
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
        //CkPrintf("Usage: RandomAccess logLocaltablesize\n");

        logLocalTableSize = atoi(args->argv[1]);
        delete args;

        numOfUpdaters = CkNumPes();
        localTableSize = 1l << logLocalTableSize;
        tableSize = localTableSize * numOfUpdaters ;
        CkPrintf("Main table size   = 2^%d * %d = %ld bytes\n", logLocalTableSize, CkNumPes(), tableSize);
        CkPrintf("Number of processes = %d\n", CkNumPes());
        CkPrintf("Number of updates = %ld\n", (4*tableSize));
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
        long GlbnumErrors = *(long*)msg->getData();
        CkPrintf(  "Found %ld errors in %ld locations (%s).\n", GlbnumErrors, 
            tableSize, (GlbnumErrors <= 0.01*tableSize) ? "passed" : "failed");
        delete msg;
        CkExit();
    }
};

class Updater : public CBase_Updater {
private:
    long *HPCC_Table;
    long globalStartmyProc;
public:
    Updater() {}
    Updater(CkMigrateMessage* m) {}
    void initialize(){
        globalStartmyProc = thisIndex * localTableSize  ;
        HPCC_Table = (long*)malloc(sizeof(long) * localTableSize);
        for(long i=0; i<localTableSize; i++)
            HPCC_Table[i] = i + globalStartmyProc;
        contribute(CkCallback(CkIndex_Main::start(NULL), mainProxy)); 
    }

    void generateUpdates()
    {
        long updatesNum;
        long ran, localOffset;
        int tableIndex;
        ran= HPCC_starts(4* globalStartmyProc);
        updatesNum = 4 * localTableSize;
        for(long i=0; i<updatesNum;i++)
        {
            ran = (ran << 1) ^ ((long) ran < ZERO64B ? POLY : ZERO64B);
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
        }
    }

    void updateLocalTable( long ran)
    {
        long localOffset;
        localOffset = ran & (localTableSize - 1);
        HPCC_Table[localOffset] ^= ran;
    }

    void checkErrors()
    {
        long numErrors = 0;
        for (long j=0; j<localTableSize; j++)
            if (HPCC_Table[j] != j + globalStartmyProc)
                numErrors++;
        contribute(sizeof(long), &numErrors, CkReduction::sum_long, CkCallback(CkIndex_Main::verifyDone(NULL), mainProxy)); 
    }
};

/** random generator */
long HPCC_starts(long n)
{
    int i, j;
    long m2[64];
    long temp, ran;
    while (n < 0) n += PERIOD;
    while (n > PERIOD) n -= PERIOD;
    if (n == 0) return 0x1;
    temp = 0x1;
    for (i=0; i<64; i++) {
        m2[i] = temp;
        temp = (temp << 1) ^ ((long) temp < 0 ? POLY : 0);
        temp = (temp << 1) ^ ((long) temp < 0 ? POLY : 0);
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
            ran = (ran << 1) ^ ((long) ran < 0 ? POLY : 0);
    }
    return ran;

}

#include "randomAccess.def.h"
