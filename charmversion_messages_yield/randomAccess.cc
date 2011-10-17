#include "hpcc.h"
#include "randomAccess.decl.h"
#include "buckets.h"
#include "randomAccess.h"
#include "util.h"

#define  UPDATE_QUIESCENCE  0
#define  VERIFY_QUIESCENCE 1

#define  WORK_ON_ONE_PE    1
/* Readonly variables */
CProxy_Main mainProxy;
CProxy_Generator generator_array;
CProxy_Updater updater_array;

int logLocalTableSize;
u64Int localTableSize;
u64Int tableSize;
int numOfUpdators;

/* How to map objects to processors 
 * By just changing this mapping, we can control how to implement the updater*/
class PMEMap : public CkArrayMap
{
    int offset;
public:
    PMEMap(int off) {
        offset = off;
    }
    //PMEMap(CkMigrateMessage *m){}
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
/*  Two modes
 *  (1)  WORK_ON_ONE_PE is defined: each processor is responsible both for generating numbers and updating table
 *  (2) WORK_ON_ONE_PE is not defined: each processor is dedicated to either generating numbers or updating table
 *      In this mode, the ratio of generators and updators can also be adjusted by using runtime parameter
 */ 

Main::Main(CkArgMsg* args) 
{
    CkPrintf("Usage: RandomAccess logLocaltablesize\n");
    
    logLocalTableSize = atoi(args->argv[1]);
    delete args;

#ifdef WORK_ON_ONE_PE 
    numOfUpdators = CkNumPes();
#else
    numOfUpdators = CkNumPes()/2;
#endif

    localTableSize = 1l << logLocalTableSize;
    tableSize = localTableSize * numOfUpdators ;
    
    CkPrintf("\nRunning randomAccess on %d processors\n Memory size per core:"FSTR64"Mbytes\n Total Memory:"FSTR64"MBytes\n", CkNumPes(), localTableSize/1024/1024, tableSize/1020/1024);


    mainProxy = thishandle;
    mainhandle = thishandle;    

#ifdef WORK_ON_ONE_PE
    generator_array = CProxy_Generator::ckNew(numOfUpdators);
    updater_array   = CProxy_Updater::ckNew(numOfUpdators);
#else
    CProxy_PMEMap generatorMap=CProxy_PMEMap::ckNew(0);
    CkArrayOptions opts_generator(numOfUpdators);
    opts_generator.setMap(generatorMap);
    generator_array = CProxy_Generator::ckNew(opts_generator);
   
    CProxy_PMEMap updaterMap=CProxy_PMEMap::ckNew(1);
    CkArrayOptions opts_updater(numOfUpdators);
    opts_updater.setMap(updaterMap);
    updater_array   = CProxy_Updater::ckNew(opts_updater);
#endif
    updater_array.initialize();
}
void Main::start(CkReductionMsg *msg)
{
    CkPrintf("\nstart RandomAccess\n");
    delete msg;
    generator_array.generateUpdates();
    starttime = CmiWallTimer();
    phase = UPDATE_QUIESCENCE;      //randomAccess phase 
    CkStartQD(CkIndex_Main::allUpdatesDone((DUMMYMSG *)0), &mainhandle);
}
void Main::allUpdatesDone(DUMMYMSG *msg)
{
    double singlegups;
    double gups;

    double update_walltime = CmiWallTimer() - starttime;
    double update_cputime = CmiCpuTimer()-starttime;
    
    delete msg;
    if(phase == UPDATE_QUIESCENCE)
    {
        gups = 1e-9 * tableSize * 4.0/update_walltime;
        singlegups =  gups/numOfUpdators;
        CkPrintf("Random Access update done\n");
        CkPrintf( "CPU time used = %.6f seconds\n", update_cputime );
        CkPrintf( "Real time used = %.6f seconds\n", update_walltime);
        CkPrintf( "%.9f Billion(10^9) Updates    per second [GUP/s]\n",  gups);
        CkPrintf( "%.9f Billion(10^9) Updates/PE per second [GUP/s]\n", singlegups );

        CkPrintf("\n\nStart verifying...\n");

        starttime = CmiWallTimer();
        phase = VERIFY_QUIESCENCE;      //verify
        generator_array.generateUpdates();
        CkStartQD(CkIndex_Main::allUpdatesDone((DUMMYMSG *)0), &mainhandle);
    }else if(phase == VERIFY_QUIESCENCE)
    {
        //verify done
       CkPrintf("verify done\n");
       updater_array.checkErrors(); 
    }
}

void Main::verifyDone(CkReductionMsg *msg) 
{
    long int GlbnumErrors = *(int*)msg->getData();
    CkPrintf(  "Verification:  CPU time used = %.6f seconds\n", CmiCpuTimer() - starttime);
    CkPrintf(  "Verification:  Real time used = %.6f seconds\n", CmiWallTimer() - starttime);
    CkPrintf(  "Found "FSTR64"  errors in "FSTR64"  locations (%s).\n",
        GlbnumErrors, tableSize, (GlbnumErrors <= 0.01*tableSize) ?
        "passed" : "failed");
    delete msg;
    CkExit();
}

Generator::Generator() 
{ 
}

void Generator::generateUpdates()
{
    u64Int updatesNum;
    u64Int ran, globalOffset;
    int peUpdates;
    int tableIndex;
    Bucket_Ptr buckets;
    int pendingUpdates;
    int Updatesnum;
    PassData *remoteData;

    int GlobalStartmyProc = thisIndex * localTableSize  ;
    u64Int randseed = 4 * GlobalStartmyProc; 
    ran= nth_random(randseed);
    
    pendingUpdates = 0;
    updatesNum = 4 * localTableSize;
    buckets = HPCC_InitBuckets(numOfUpdators, MAX_TOTAL_PENDING_UPDATES);
    for(int i=0; i<updatesNum;)
    {
        if (pendingUpdates < MAX_TOTAL_PENDING_UPDATES)
        {
            ran = (ran << 1) ^ ((s64Int) ran < ZERO64B ? POLY : ZERO64B);
            globalOffset = ran & (tableSize-1);
            //tableIndex = globalOffset/localTableSize; if localsize is not power 2
            tableIndex = globalOffset >>  logLocalTableSize;
            HPCC_InsertUpdate(ran, tableIndex, buckets);
            pendingUpdates++;
            i++;
        }else
        {
            tableIndex = HPCC_GetMaxUpdates(buckets, &peUpdates);
            remoteData = new (peUpdates, 0) PassData(peUpdates, thisIndex);
            HPCC_GetUpdates(buckets, remoteData->getBuffer(), tableIndex, peUpdates);
            pendingUpdates -= peUpdates;
            updater_array[tableIndex].updateLocalTable(remoteData);
#ifdef WORK_ON_ONE_PE
            CthYield();   
#endif
        }
    }

    while(pendingUpdates > 0)
    {
        tableIndex = HPCC_GetMaxUpdates(buckets, &peUpdates);
        PassData *remoteData = new (peUpdates, 0) PassData(peUpdates, thisIndex);
        HPCC_GetUpdates(buckets, remoteData->getBuffer(), tableIndex, peUpdates);
        
        pendingUpdates -= peUpdates;
        updater_array[tableIndex].updateLocalTable(remoteData);
    }
}

Updater::Updater(){}

void Updater::initialize(){

    int numErrors = 0;
    int GlobalStartmyProc = thisIndex * localTableSize  ;
    HPCC_Table = (u64Int*)malloc(sizeof(u64Int) * localTableSize);
    /* Initialize Table */
    for(int i=0; i<localTableSize; i++)
        HPCC_Table[i] = i + GlobalStartmyProc;
    contribute(CkCallback(CkIndex_Main::start(NULL), mainProxy)); 
    //contribute(sizeof(int), &numErrors, CkReduction::sum_int, CkCallback(CkIndex_Main::start(NULL), mainProxy)); 
}
/* For better performance, message will be better than method parameters */
void Updater::updateLocalTable(PassData* remotedata)
{
    u64Int localOffset;
    u64Int inmsg;
    int size = remotedata->size;
    u64Int *data = remotedata->data;
    for(int j=0; j<size; j++)
    {
        inmsg = *((u64Int*)data+j);
        localOffset = inmsg & (localTableSize - 1);
        HPCC_Table[localOffset] ^= inmsg;
    }
}

void Updater::checkErrors()
{
    long int numErrors = 0;
    int GlobalStartmyProc = thisIndex * localTableSize  ;
    for (int j=0; j<localTableSize; j++)
        if (HPCC_Table[j] != j + GlobalStartmyProc)
            numErrors++;
    contribute(sizeof(long), &numErrors, CkReduction::sum_long, CkCallback(CkIndex_Main::verifyDone(NULL), mainProxy)); 
}

#include "randomAccess.def.h"
