#include "NDMeshStreamer.h"
#include "randomAccess.decl.h"
#include "TopoManager.h"
#include "completion.h"

#ifdef LONG_IS_64BITS
#define ZERO64B 0L
#define POLY 0x0000000000000007UL
#define PERIOD 1317624576693539401L
#else
#define ZERO64B 0LL
#define POLY 0x0000000000000007ULL
#define PERIOD 1317624576693539401LL
#endif

#define NUM_MESSAGES_BUFFERED 1024

CProxy_Main     mainProxy;
int             N;                  //log local table size    
CmiInt8         localTableSize;
CmiInt8         tableSize;
CProxy_GroupMeshStreamer<CmiUInt8> aggregator; 
CProxy_CompletionDetector detector;

CmiUInt8 HPCC_starts(CmiInt8 n);

class Main : public CBase_Main {
private:
    CProxy_Updater  updater_group;
    double starttime;
public:
    Main(CkArgMsg* args) {
        int NUM_ROWS, NUM_COLUMNS, NUM_PLANES;
        TopoManager tmgr;
        N = atoi(args->argv[1]);
        //use this if you do not want to differentiate based on core ID's
        NUM_ROWS = tmgr.getDimNX()*tmgr.getDimNT();
        NUM_COLUMNS = tmgr.getDimNY();
        NUM_PLANES = tmgr.getDimNZ();
        delete args;

        int dims[3] = {NUM_ROWS, NUM_COLUMNS, NUM_PLANES}; 
        localTableSize = 1l << N;
        tableSize = localTableSize * CkNumPes();
        CkPrintf("Main table size   = 2^%d * %d = %lld words\n", N, CkNumPes(), tableSize);
        CkPrintf("Number of processors = %d\n", CkNumPes());
        CkPrintf("Number of updates = %lld\n", (4*tableSize));
        mainProxy = thishandle;
        // Create the chares storing and updating the global table
        updater_group   = CProxy_Updater::ckNew();
        //Create Mesh Streamer instance
        aggregator = CProxy_GroupMeshStreamer<CmiUInt8>::ckNew(NUM_MESSAGES_BUFFERED, 3, dims, updater_group, 1, 10);
        detector = CProxy_CompletionDetector::ckNew();
    }

    void start() {
        starttime = CkWallTimer();
        // Give the updater chares the 'go' signal
        CkCallback startCb(CkIndex_Updater::generateUpdates(), updater_group);
        CkCallback endCb(CkIndex_Main::allUpdatesDone(), thisProxy);          
        aggregator.associateCallback(CkNumPes(), startCb, endCb, detector, 
                                     INT_MIN);
    }

    void allUpdatesDone()
    {
        double update_walltime = CkWallTimer() - starttime;
        double gups = 1e-9 * tableSize * 4.0/update_walltime;
        double singlegups =  gups/CkNumPes();
        CkPrintf( "CPU time used = %.6f seconds\n", update_walltime );
        CkPrintf( "%.9f Billion(10^9) Updates    per second [GUP/s]\n",  gups);
        CkPrintf( "%.9f Billion(10^9) Updates/PE per second [GUP/s]\n", singlegups );
        // Repeat the update process to verify
        CkCallback startCb(CkIndex_Updater::generateUpdates(), updater_group);  
        CkCallback endCb(CkIndex_Updater::checkErrors(), updater_group);
        aggregator.associateCallback(CkNumPes(), startCb, endCb, detector, 
                                     INT_MIN);
    }
    
    void verifyDone(CmiInt8 globalNumErrors) {
        CkPrintf(  "Found %lld errors in %lld locations (%s).\n", globalNumErrors, 
            tableSize, (globalNumErrors <= 0.01*tableSize) ? "passed" : "failed");
        CkExit();
    }
};

class Updater : public MeshStreamerGroupClient<CmiUInt8> {
private:
    CmiUInt8 *HPCC_Table;
    CmiUInt8 globalStartmyProc;
public:
    Updater() {
        globalStartmyProc = CkMyPe()* localTableSize  ;
        HPCC_Table = (CmiUInt8*)malloc(sizeof(CmiUInt8) * localTableSize);
        for(CmiInt8 i=0; i<localTableSize; i++)
            HPCC_Table[i] = i + globalStartmyProc;
        contribute(CkCallback(CkReductionTarget(Main, start), mainProxy));
    }

    inline virtual void process(CmiUInt8  &ran) {
        CmiInt8  localOffset = ran & (localTableSize - 1);
        HPCC_Table[localOffset] ^= ran;
    }

    void generateUpdates()
    {
        CmiUInt8 ran= HPCC_starts(4* globalStartmyProc);
        GroupMeshStreamer<CmiUInt8> * streamer = ((GroupMeshStreamer<CmiUInt8> *)CkLocalBranch(aggregator));
        for(CmiInt8 i=0; i< 4 * localTableSize; i++)
        {
            ran = (ran << 1) ^ ((CmiInt8) ran < ZERO64B ? POLY : ZERO64B);
            int tableIndex = (ran >>  N)&(CkNumPes()-1);
            //    CkPrintf("[%d] sending %lld to %d\n", CkMyPe(), ran, tableIndex);
            streamer->insertData(ran, tableIndex);
        }
        streamer->done();
    }

    void checkErrors()
    {
        CmiInt8 numErrors = 0;
        for (CmiInt8 j=0; j<localTableSize; j++)
            if (HPCC_Table[j] != j + globalStartmyProc)
                numErrors++;
        // Sum the errors observed across the entire system
        contribute(sizeof(CmiInt8), &numErrors, CkReduction::sum_long, CkCallback(CkReductionTarget(Main,verifyDone), mainProxy));
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
