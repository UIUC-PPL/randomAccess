#include "NDMeshStreamer.h"
#include "randomAccess.decl.h"
#include "TopoManager.h"
#include "completion.h"
#include "limits.h"
#include "randomAccessDatatype.h"

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
int             numElementsPerPe; 
CmiInt8         localTableSize;
CmiInt8         tableSize;
CProxy_ArrayMeshStreamer<raDtype, CkArrayIndex> aggregator; 
CProxy_CompletionDetector detector; 

CmiUInt8 HPCC_starts(CmiInt8 n);

class Main : public CBase_Main {
private:
    CProxy_Updater  updater_array;
    double starttime;
public:
    Main(CkArgMsg* args) {
        int NUM_ROWS, NUM_COLUMNS, NUM_PLANES;
        TopoManager tmgr;
        N = atoi(args->argv[1]);
        numElementsPerPe = atoi(args->argv[2]); 
        NUM_ROWS = tmgr.getDimNX()*tmgr.getDimNT();
        NUM_COLUMNS = tmgr.getDimNY();
        NUM_PLANES = tmgr.getDimNZ();
        delete args;

        int dims[3] = {NUM_ROWS, NUM_COLUMNS, NUM_PLANES}; 
        localTableSize = (1l << N) / numElementsPerPe;
        CkPrintf("LocalTableSize: %lld\n", localTableSize);
        tableSize = localTableSize * CkNumPes() * numElementsPerPe;
        CkPrintf("Main table size   = 2^%d * %d = %lld words\n", N, CkNumPes(), tableSize);
        CkPrintf("Number of processors = %d\n", CkNumPes());
        CkPrintf("Number of updates = %lld\n", (4*tableSize));
        mainProxy = thishandle;
        // Create the chares storing and updating the global table
        updater_array   = CProxy_Updater::ckNew(CkNumPes() * numElementsPerPe, 1);
        //Create Mesh Streamer instance
        aggregator = 
          CProxy_ArrayMeshStreamer <raDtype, CkArrayIndex>::ckNew(
           NUM_MESSAGES_BUFFERED,3, dims, 
           (CProxy_MeshStreamerArrayClient<raDtype> )updater_array, true, 10.0);
        detector = CProxy_CompletionDetector::ckNew();
    }

    void start() {
        starttime = CkWallTimer();
        // Give the updater chares the 'go' signal after aggregator setup is complete
        CkCallback startCb(CkIndex_Updater::generateUpdates(), updater_array);
        CkCallback endCb(CkIndex_Main::allUpdatesDone(), thisProxy);          
        int numContributors = CkNumPes() * numElementsPerPe; 
        aggregator.associateCallback(numContributors, startCb, endCb, detector,
                                     INT_MIN, false);
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
        CkCallback startCb(CkIndex_Updater::generateUpdates(), updater_array);  
        CkCallback endCb(CkIndex_Updater::checkErrors(), updater_array);
        int numContributors = CkNumPes() * numElementsPerPe; 
        aggregator.associateCallback(numContributors, startCb, endCb, detector,
                                     INT_MIN, false);
    }
    
    void verifyDone(CmiInt8 globalNumErrors) {
        CkPrintf(  "Found %lld errors in %lld locations (%s).\n", globalNumErrors, 
            tableSize, (globalNumErrors <= 0.01*tableSize) ? "passed" : "failed");
        CkExit();
    }
};

class Updater : public MeshStreamerArrayClient<raDtype> {
private:
    CmiUInt8 *HPCC_Table;
    CmiUInt8 globalStartmyProc;
    CkIndex2D myIndex; 
public:
    Updater() {
      myIndex.x = thisIndex.data[0]; 
      myIndex.y = thisIndex.data[1]; 
      globalStartmyProc = myIndex.x * localTableSize  ;
        HPCC_Table = (CmiUInt8*)malloc(sizeof(CmiUInt8) * localTableSize);
        for(CmiInt8 i=0; i<localTableSize; i++)
            HPCC_Table[i] = i + globalStartmyProc;
        contribute(CkCallback(CkIndex_Main::start(), mainProxy));
    }

     Updater(CkMigrateMessage *msg) {}

    inline virtual void process(const raDtype  &ran) {
        CmiInt8  localOffset = ran.elems[0] & (localTableSize - 1);
        HPCC_Table[localOffset] ^= ran.elems[0];
    }

    void generateUpdates()
    {
        int arrayN = N - (int) log2((double) numElementsPerPe); 
        int numElements = CkNumPes() * numElementsPerPe;
        CmiUInt8 ran= HPCC_starts(4* globalStartmyProc);
        raDtype temp;
        ArrayMeshStreamer<raDtype, CkArrayIndex> * streamer = 
          ((ArrayMeshStreamer<raDtype, CkArrayIndex> *)
           CkLocalBranch(aggregator));
        for(CmiInt8 i=0; i< 4 * localTableSize; i++)
        {
            ran = (ran << 1) ^ ((CmiInt8) ran < ZERO64B ? POLY : ZERO64B);
            int tableIndex = (ran >>  arrayN)&(numElements-1);
            temp.elems[0] = ran;
            streamer->insertData(temp, CkArrayIndex2D(tableIndex, 0));
        }
        streamer->done();
    }

    void checkErrors()
    {
      CmiMemoryCheck();
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
