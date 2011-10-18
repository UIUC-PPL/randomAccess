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
 /*     each processor is dedicated to either generating numbers or updating table
 *      In this mode, the ratio of generators and updators can also be adjusted by using runtime parameter
 */ 

Main::Main(CkArgMsg* args) 
{
    CkPrintf("Usage: RandomAccess logLocaltablesize\n");
    
    logLocalTableSize = atoi(args->argv[1]);
    delete args;

    numOfUpdators = CkNumPes()/2;
    localTableSize = 1l << logLocalTableSize;
    tableSize = localTableSize * numOfUpdators ;
    
    CkPrintf("\nRunning randomAccess on %d processors\n Memory size per core:"FSTR64"Mbytes\n Total Memory:"FSTR64"MBytes\n", CkNumPes(), localTableSize/1024/1024, tableSize/1020/1024);


    mainProxy = thishandle;
    mainhandle = thishandle;    

    CProxy_PMEMap generatorMap=CProxy_PMEMap::ckNew(0);
    CkArrayOptions opts_generator(numOfUpdators);
    opts_generator.setMap(generatorMap);
    generator_array = CProxy_Generator::ckNew(opts_generator);
   
    CProxy_PMEMap updaterMap=CProxy_PMEMap::ckNew(1);
    CkArrayOptions opts_updater(numOfUpdators);
    opts_updater.setMap(updaterMap);
    updater_array   = CProxy_Updater::ckNew(opts_updater);
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
            //globalOffset = ran & (tableSize-1);
            //tableIndex = globalOffset/localTableSize; if localsize is not power 2
            tableIndex = (ran >>  logLocalTableSize)&(numOfUpdators-1);
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
