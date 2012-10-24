#include "NDMeshStreamer.h"

typedef CmiUInt8 dtype;
#include "randomAccess.decl.h"
#include "TopoManager.h"
#include "limits.h"

#define POLY 0x0000000000000007ULL
#define PERIOD 1317624576693539401LL
#define NUM_MESSAGES_BUFFERED 1024

CProxy_Main     mainProxy;
int             N;                  //log local table size    
CmiInt8         localTableSize;
CmiInt8         tableSize;
CProxy_GroupMeshStreamer<dtype> aggregator; 

CmiUInt8 HPCC_starts(CmiInt8 n);

class Main : public CBase_Main {
private:
    CProxy_Updater  updater_group;
    double starttime;
public:
    Main(CkArgMsg* args) {
        TopoManager tmgr;
        N = atoi(args->argv[1]);
        delete args;
        int dims[3] = {tmgr.getDimNX() * tmgr.getDimNT(), tmgr.getDimNY(), tmgr.getDimNZ()}; 
        localTableSize = 1l << N;
        tableSize = localTableSize * CkNumPes();
        CkPrintf("Main table size   = 2^%d * %d = %lld words\n", N, CkNumPes(), tableSize);
        CkPrintf("Number of processors = %d\nNumber of updates = %lld\n", CkNumPes(), 4*tableSize);
        mainProxy = thishandle;
        // Create the chares storing and updating the global table
        updater_group   = CProxy_Updater::ckNew();
        //Create Mesh Streamer instance
        aggregator = CProxy_GroupMeshStreamer<dtype>::ckNew(NUM_MESSAGES_BUFFERED, 3, dims, updater_group, 1, 10);
    }

    void start() {
        starttime = CkWallTimer();
        // Give the updater chares the 'go' signal
        CkCallback startCb(CkIndex_Updater::generateUpdates(), updater_group);
        CkCallback endCb(CkIndex_Main::allUpdatesDone(), thisProxy);          
        aggregator.init(1, startCb, endCb, INT_MIN, false);
    }

    void allUpdatesDone() {
        double update_walltime = CkWallTimer() - starttime;
        double gups = 1e-9 * tableSize * 4.0/update_walltime;
        double singlegups =  gups/CkNumPes();
        CkPrintf( "CPU time used = %.6f seconds\n", update_walltime );
        CkPrintf( "%.9f Billion(10^9) Updates    per second [GUP/s]\n",  gups);
        CkPrintf( "%.9f Billion(10^9) Updates/PE per second [GUP/s]\n", singlegups );
        // Repeat the update process to verify
        CkCallback startCb(CkIndex_Updater::generateUpdates(), updater_group);  
        CkCallback endCb(CkIndex_Updater::checkErrors(), updater_group);
        aggregator.init(1, startCb, endCb, INT_MIN, false);
    }
    
    void verifyDone(CmiInt8 globalNumErrors) {
        CkPrintf(  "Found %lld errors in %lld locations (%s).\n", globalNumErrors, 
            tableSize, (globalNumErrors <= 0.01*tableSize) ? "passed" : "failed");
        CkExit();
    }
};

class Updater : public MeshStreamerGroupClient<dtype> {
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

    inline virtual void process(const dtype  &ran) {
        CmiInt8  localOffset = ran & (localTableSize - 1);
        HPCC_Table[localOffset] ^= ran;
    }

    void generateUpdates() {
        CmiUInt8 ran= HPCC_starts(4* globalStartmyProc);
        GroupMeshStreamer<dtype> * streamer = ((GroupMeshStreamer<dtype> *)CkLocalBranch(aggregator));
        for(CmiInt8 i=0; i< 4 * localTableSize; i++)
        {
            ran = (ran << 1) ^ ((CmiInt8) ran < 0 ? POLY : 0);
            int tableIndex = (ran >>  N)&(CkNumPes()-1);
            //    CkPrintf("[%d] sending %lld to %d\n", CkMyPe(), ran, tableIndex);
            streamer->insertData(ran, tableIndex);
        }
        streamer->done();
    }

    void checkErrors() {
        CmiInt8 numErrors = 0;
        for (CmiInt8 j=0; j<localTableSize; j++)
            if (HPCC_Table[j] != j + globalStartmyProc)
                numErrors++;
        // Sum the errors observed across the entire system
        contribute(sizeof(CmiInt8), &numErrors, CkReduction::sum_long, CkCallback(CkReductionTarget(Main,verifyDone), mainProxy));
    }
};

/** random generator */
CmiUInt8 HPCC_starts(CmiInt8 n) {
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
