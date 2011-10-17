#include "util.h"
#include "randomAccess.decl.h"
#include "randomAccess.h"

#define  UPDATE_QUIESCENCE  0
#define  VERIFY_QUIESCENCE 1

/* Readonly variables */
CProxy_Main mainProxy;
CProxy_Updater updater_array;

int logLocalTableSize;
u64Int localTableSize;
u64Int tableSize;
int numOfUpdaters;

Main::Main(CkArgMsg* args) 
{
    CkPrintf("Usage: RandomAccess logLocaltablesize\n");

    logLocalTableSize = atoi(args->argv[1]);
    delete args;

    numOfUpdaters = CkNumPes();

    localTableSize = 1l << logLocalTableSize;
    tableSize = localTableSize * numOfUpdaters ;
    
    CkPrintf("\nRunning randomAccess on %d processors\n Memory size per core:"FSTR64"Mbytes\n Total Memory:"FSTR64"MBytes\n", CkNumPes(), localTableSize/1024/1024, tableSize/1020/1024);


    mainProxy = thishandle;
    mainhandle = thishandle;    

    updater_array   = CProxy_Updater::ckNew(numOfUpdaters);
    updater_array.initialize();
}
void Main::start(CkReductionMsg *msg)
{
    CkPrintf("\nstart RandomAccess\n");
    delete msg;
    updater_array.generateUpdates();
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
        singlegups =  gups/numOfUpdaters;
        CkPrintf("Random Access update done\n");
        CkPrintf( "CPU time used = %.6f seconds\n", update_cputime );
        CkPrintf( "Real time used = %.6f seconds\n", update_walltime);
        CkPrintf( "%.9f Billion(10^9) Updates    per second [GUP/s]\n",  gups);
        CkPrintf( "%.9f Billion(10^9) Updates/PE per second [GUP/s]\n", singlegups );

        CkPrintf("\n\nStart verifying...\n");

        starttime = CmiWallTimer();
        phase = VERIFY_QUIESCENCE;      //verify
        updater_array.generateUpdates();
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

Updater::Updater(){}

void Updater::initialize(){

    int numErrors = 0;
    int GlobalStartmyProc = thisIndex * localTableSize  ;
    HPCC_Table = (u64Int*)malloc(sizeof(u64Int) * localTableSize);
    /* Initialize Table */
    for(int i=0; i<localTableSize; i++)
        HPCC_Table[i] = i + GlobalStartmyProc;
    contribute(CkCallback(CkIndex_Main::start(NULL), mainProxy)); 
}

void Updater::generateUpdates()
{
    u64Int updatesNum;
    u64Int ran, globalOffset, localOffset;
    int peUpdates;
    int tableIndex;
    int updatesnum;

    int globalStartmyProc = thisIndex * localTableSize  ;
    u64Int randseed = 4 * globalStartmyProc; 
    ran= nth_random(randseed);
    
    updatesNum = 4 * localTableSize;
    for(int i=0; i<updatesNum;i++)
    {
        ran = (ran << 1) ^ ((s64Int) ran < ZERO64B ? POLY : ZERO64B);
        //globalOffset = ran & (tableSize-1);
        //tableIndex = globalOffset/localTableSize; if localsize is not power 2
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

/* For better performance, message will be better than method parameters */
void Updater::updateLocalTable( u64Int inmsg)
{
    u64Int localOffset;
    localOffset = inmsg & (localTableSize - 1);
    HPCC_Table[localOffset] ^= inmsg;
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
