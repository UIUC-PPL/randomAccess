#include "hpcc.h"
#include "RandomAccess.decl.h"
#include "buckets.h"
#include "randomAccess.h"
#include "util.h"

#define  UPDATE_QUIESCENCE  0
#define  VERIFY_QUIESCENCE 1

/* Readonly variables */
CProxy_Main mainProxy;
CProxy_Generator generator_array;
CProxy_Updater updater_array;

int loglocalTableSize;
u64Int localTableSize;
u64Int tableSize;
int numOfNodes;

/* node-level share variable */
u64Int *HPCC_Table;

class PMEMap : public CkArrayMap
{
    int offset;
public:
    PMEMap(int off) {
        offset = off;
    }
    PMEMap(CkMigrateMessage *m){}
    int registerArray(CkArrayIndex& numElements,CkArrayID aid) {
        return 0;
    }
    int procNum(int /*arrayHdl*/,const CkArrayIndex &idx) {
        int penum;
        int *index =  (int *)idx.data();
        penum = 2*(*index)+offset;
        return penum;
    }
};


Main::Main(CkArgMsg* args) 
{
    CkPrintf("Usage: RandomAccess loglocaltablesize\n Must be running on smp with (ppn 2)\n");
    CkAssert(args->argc == 2);
    
    loglocalTableSize = atoi(args->argv[1]);
    localTableSize = 1 << loglocalTableSize;
    numOfNodes = CkNumNodes();
    tableSize = localTableSize * numOfNodes ;

    mainProxy = thishandle;
    mainhandle = thishandle;    
    starttime = CmiWallTimer();

    CProxy_PMEMap generatorMap=CProxy_PMEMap::ckNew(0);
    CkArrayOptions opts_generator(numofNodes);
    opts_generator.setMap(generatorMap);

    generator_array = CProxy_Generator::ckNew(numOfNodes, generatorMap);
    
    
    CProxy_PMEMap updaterMap=CProxy_PMEMap::ckNew(1);
    CkArrayOptions opts_updater(numofNodes);
    opts_updater.setMap(updaterMap);
    updater_array   = CProxy_Updater::ckNew(numOfNodes, updaterMap);
    
    generator_array.generateUpdates();
    phase = UPDATE_QUIESCENCE;      //randomAccess phase 
    CkStartQD(CkIndex_Main::allUpdatesDone((DUMMYMSG *)0), &mainhandle);
}

void Main::allUpdatesDone(DUMMYMSG *msg)
{
    double singlegups;
    double gups;

    double update_walltime = CmiWallTimer() - starttime;
    double update_cputime = CmiCpuTimer()-starttime;

    if(phase == UPDATE_QUIESCENCE)
    {
        gups = 1e-9 * tableSize * 4.0/update_walltime;
        singlegups =  gups/numOfNodes;
        CkPrintf("\n\nRandom Access update done\n");
        CkPrintf("Total node number is :%d\n", CkNumNodes());
        CkPrintf( "CPU time used = %.6f seconds\n", update_cputime );
        CkPrintf( "Real time used = %.6f seconds\n", update_walltime);
        CkPrintf( "%.9f Billion(10^9) Updates    per second [GUP/s]\n",  gups);
        CkPrintf( "%.9f Billion(10^9) Updates/PE per second [GUP/s]\n", singlegups );

        CkPrintf("\n\n Start verifying...\n");

        starttime = CmiWallTimer();
        phase = VERIFY_QUIESCENCE;      //verify
        generator_array.generateUpdates();
    }else if(phase == VERIFY_QUIESCENCE)
    {
        //verify done
       updater_array.checkErrors(); 
    }
}

void Main::verifyDone(CkReductionMsg *msg) 
{
    int GlbnumErrors = *(int*)msg->getData();
    CkPrintf(  "Verification:  CPU time used = %.6f seconds\n", CmiCpuTimer() - starttime);
    CkPrintf(  "Verification:  Real time used = %.6f seconds\n", CmiWallTimer() - starttime);
    CkPrintf(  "Found "FSTR64"  errors in "FSTR64"  locations (%s).\n",
        GlbnumErrors, tableSize, (GlbnumErrors <= 0.01*tableSize) ?
        "passed" : "failed");
    CkExit();
}

Generator::Generator() 
{ 
    int GlobalStartmyProc = thisIndex * localTableSize  ;
    u64Int randseed = 4 * GlobalStartmyProc; 
    ran= nth_random(randseed);
    HPCC_Table = (u64Int*)malloc(sizeof(u64Int) * localTableSize);
    /* Initialize Table */
    for(int i=0; i<localTableSize; i++)
        HPCC_Table[i] = i + GlobalStartmyProc;
}

void Generator::generateUpdates()
{
    u64Int updatesNum;
    u64Int ran;
    int peUpdates;
    int tableIndex;
    Bucket_Ptr buckets;
    int pendingUpdates;
    int Updatesnum;
    PassData *remoteData;
    pendingUpdates = 0;
    updatesNum = 4 * localTableSize;

    buckets = HPCC_InitBuckets(numOfNodes, MAX_TOTAL_PENDING_UPDATES);

    for(int i=0; i<updatesNum;)
    {
        if (pendingUpdates < MAX_TOTAL_PENDING_UPDATES)
        {
            ran = (ran << 1) ^ ((s64Int) ran < ZERO64B ? POLY : ZERO64B);
            tableIndex = (ran >> loglocalTableSize) & (numOfNodes - 1);
            HPCC_InsertUpdate(ran, tableIndex, buckets);
            pendingUpdates++;
            i++;
        }else
        {
            tableIndex = HPCC_GetMaxUpdates(buckets, &peUpdates);
            remoteData = new (peUpdates, 0) PassData(peUpdates, thisIndex);
            HPCC_GetUpdates(buckets, remotedata->getBuffer(), tableIndex, peUpdates);
            pendingUpdates -= peUpdates;
            updater_array[tableIndex].updateLocalTable(remoteData);
        }
    }

    while(pendingUpdates > 0)
    {
        tableIndex = HPCC_GetMaxUpdates(buckets, &peUpdates);
        PassData *remoteData = new (peUpdates, 0) PassData(peUpdates, thisIndex);
        HPCC_GetUpdates(buckets, remotedata->getBuffer(), tableIndex, peUpdates);
        
        pendingUpdates -= peUpdates;
        updater_array[tableIndex].updateLocalTable(remoteData);
    }
}

Updater::Updater(){}
/* For better performance, message will be better than method parameters */
void Updater::updateLocalTable(PassData* remotedata)
{
    u64Int LocalOffset;
    u64Int inmsg;
    int size = remotedata->size;
    u64Int *data = remotedata->data;
    for(int j=0; j<size; j++)
    {
        inmsg = *((u64Int*)data+j);
        LocalOffset = inmsg & (localTableSize - 1);
        HPCC_Table[LocalOffset] ^= inmsg;
    }
}

void Updater::checkErrors()
{
    int numErrors = 0;
    int GlobalStartmyProc = thisIndex * localTableSize  ;
    for (int j=0; j<localTableSize; j++)
        if (HPCC_Table[j] != j + GlobalStartmyProc)
            numErrors++;
    contribute(sizeof(int), &numErrors, CkReduction::sum_int, CkCallback(CkIndex_Main::verifyDone(NULL), mainProxy)); 
}

#include "RandomAccess.def.h"
