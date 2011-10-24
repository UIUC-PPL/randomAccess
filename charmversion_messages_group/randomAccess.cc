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

/* Readonly global variables */
CProxy_Main     mainProxy;
int             N;                  //log local table size    
CmiInt8         localTableSize;
CmiInt8         tableSize;

CmiUInt8 HPCC_starts(CmiInt8 n);

class Main : public CBase_Main {
private:
    CkChareID   mainhandle;
    CProxy_Updater  updater_array;
    double      starttime;
public:
    Main(CkArgMsg* args) 
    {
        N = atoi(args->argv[1]);
        delete args;
        localTableSize = 1l << N;
        tableSize = localTableSize * CkNumPes();
        CkPrintf("Main table size   = 2^%d * %d = %lld words\n", N, CkNumPes(), tableSize);
        CkPrintf("Number of processors = %d\n", CkNumPes());
        CkPrintf("Number of updates = %lld\n", (4*tableSize));
        mainProxy = thishandle;
        mainhandle = thishandle;  
        //initialize the global table 
        updater_array   = CProxy_Updater::ckNew();
    }
    void start(CkReductionMsg *msg)
    {
        // start updating
        delete msg;
        starttime = CkWallTimer();
        updater_array.generateUpdates();
        //when the updating is done, allUpdatesDone function is called
        CkStartQD(CkIndex_Main::allUpdatesDone(), &mainhandle);
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
        //After verification is done, check errors 
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

class Updater : public CBase_Updater {
private:
    CmiUInt8 *HPCC_Table;
    CmiUInt8 globalStartmyProc;
public:
    Updater() {
        globalStartmyProc = CkMyPe()* localTableSize;
        HPCC_Table = (CmiUInt8*)malloc(sizeof(CmiUInt8) * localTableSize);
        for(CmiInt8 i=0; i<localTableSize; i++)
            HPCC_Table[i] = i + globalStartmyProc;
        contribute(CkCallback(CkIndex_Main::start(NULL), mainProxy)); 
    }
    void generateUpdates() {
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
                thisProxy[tableIndex].updateLocalTable(ran);
                if(i%1024 == 0) CthYield();               //processing incoming messages
            }
        }
    }

    void updateLocalTable( CmiUInt8 ran) {
        CmiInt8 localOffset = ran & (localTableSize - 1);
        HPCC_Table[localOffset] ^= ran;
    }

    void checkErrors() {
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
