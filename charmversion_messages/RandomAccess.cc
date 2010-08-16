#include "hpcc.h"
#include "RandomAccess.decl.h"
#include "buckets.h"
#include "RandomAccess.h"


/* Readonly variables */
CProxy_Main mainProxy;
CProxy_Updater updater_array;

int logLocalTableSize;
u64Int LocalTableSize;
u64Int TableSize;
int numofchares;
#define  UPDATE_QUIESCENCE  0
#define  VERIFY_QUIESCENCE 1
#define  INTERREUPT  3
/* Verification phase: local buckets to sort into */
#define BUCKET_SIZE 1024
#define SLOT_CNT 0
#define FIRST_SLOT 1

/* end readonly variables */

u64Int nth_random(int64_t n);

Main::Main(CkArgMsg* args) 
{
    int i;
   
    CkPrintf("Usage: RandomAccess loglocaltablesize\n");
    CkAssert(args->argc == 2);
    
    logLocalTableSize = atoi(args->argv[1]);
    LocalTableSize = 1 << logLocalTableSize;
    numofchares = CkNumPes();
    TableSize = LocalTableSize * numofchares ;

    mainProxy = thishandle;
    mainhandle = thishandle;    
    starttime = CmiWallTimer();

    if(numofchares == 1)
    {
        SingleUpdate();
        CkExit();
    }

    updater_array = CProxy_Updater::ckNew(numofchares);
    for(i=0; i<numofchares; i++)
        updater_array[i].generateUpdates();
    CkStartQD(CkIndex_Main::Quiescence1((DUMMYMSG *)0), &mainhandle);
}

void Main::SingleUpdate()
{
    u64Int ran;
    u64Int randseed;
    int i;
    u64Int Updatesnum;
    u64Int LocalOffset;
    u64Int *HPCC_Table;
   
    ran= nth_random(randseed);
    HPCC_Table = (u64Int*)malloc(sizeof(u64Int) * LocalTableSize);
    Updatesnum = 4 * LocalTableSize;
    for(i=0; i<LocalTableSize; i++)
        HPCC_Table[i] = i;

    for(i=0; i< Updatesnum; i++)
    {
        ran = (ran << 1) ^ ((s64Int) ran < ZERO64B ? POLY : ZERO64B);
        LocalOffset = (ran & (TableSize - 1));
        HPCC_Table[LocalOffset] ^= ran;
    }
    double singlegups;
    double gups;
    double update_walltime = CmiWallTimer() - starttime;
    double update_cputime = CmiCpuTimer()-starttime;
    gups = 1e-9 * TableSize * 4.0/update_walltime;
    singlegups =  gups/CkNumPes();
    CkPrintf("\n\nRandom Access update done\n");
    CkPrintf("Total processor number is :%d\n", CkNumPes());
    CkPrintf( "CPU time used = %.6f seconds\n", update_cputime );
    CkPrintf( "Real time used = %.6f seconds\n", update_walltime);
    CkPrintf( "%.9f Billion(10^9) Updates    per second [GUP/s]\n",
        gups);
    CkPrintf( "%.9f Billion(10^9) Updates/PE per second [GUP/s]\n",
        singlegups );
}
void Main::collectVerification(int errors) 
{
    verifydonenum++;
    
    GlbNumErrors += errors;
    if(verifydonenum == numofchares)
    {
        CkPrintf(  "Verification:  CPU time used = %.6f seconds\n", CmiCpuTimer() - startverifytime);
        CkPrintf(  "Verification:  Real time used = %.6f seconds\n", CmiWallTimer() - startverifytime);
        CkPrintf(  "Found "FSTR64"  errors in "FSTR64"  locations (%s).\n",
            GlbNumErrors, TableSize, (GlbNumErrors <= 0.01*TableSize) ?
            "passed" : "failed");

        CkExit();
    }
}

void Main::Quiescence1(DUMMYMSG *msg)
{
    double singlegups;
    double gups;

    double update_walltime = CmiWallTimer() - starttime;
    double update_cputime = CmiCpuTimer()-starttime;
    gups = 1e-9 * TableSize * 4.0/update_walltime;
    singlegups =  gups/numofchares;
    CkPrintf("\n\nRandom Access update done\n");
    CkPrintf("Total processor number is :%d\n", CkNumPes());
    CkPrintf( "CPU time used = %.6f seconds\n", update_cputime );
    CkPrintf( "Real time used = %.6f seconds\n", update_walltime);
    CkPrintf( "%.9f Billion(10^9) Updates    per second [GUP/s]\n",
        gups);
    CkPrintf( "%.9f Billion(10^9) Updates/PE per second [GUP/s]\n",
        singlegups );

    CkPrintf("\n\n Start verifying...\n");

    startverifytime = CmiWallTimer();
    verifydonenum = 0;
    GlbNumErrors = 0;
    updater_array.verify();
}

Updater::Updater() 
{ 
    int i;
    GlobalStartMyProc = thisIndex * LocalTableSize  ;
    u64Int randseed = 4 * GlobalStartMyProc; 
    ran= nth_random(randseed);
    HPCC_Table = (u64Int*)malloc(sizeof(u64Int) * LocalTableSize);
    
    numofdone = 0;
    NumErrors = 0;

    /* Initialize Table */
    for(i=0; i<LocalTableSize; i++)
        HPCC_Table[i] = i + GlobalStartMyProc;
   
    shouldReceive = (u64Int*)malloc(sizeof(u64Int) * numofchares);
    alreadyReceive = (u64Int*)malloc(sizeof(u64Int) * numofchares);
    for(i=0; i<numofchares; i++)
    {
        shouldReceive[i] = 0;
        alreadyReceive[i] = 0;
    }
    verifydone = 0;

  }
void Updater::generateUpdates()
{
    int Whichchare;
    u64Int Updatesnum;
    int i;

    pendingUpdates = 0;
    maxPendingUpdates = MAX_TOTAL_PENDING_UPDATES;
    localBufferSize = LOCAL_BUFFER_SIZE;
    Updatesnum = 4 * LocalTableSize;

    Buckets = HPCC_InitBuckets(numofchares, maxPendingUpdates);

    for(i=0; i<Updatesnum;)
    {
        if(i%INTERREUPT == 0)
        {
            CsdSchedulePoll();
        }

        if (pendingUpdates < maxPendingUpdates)
        {
            ran = (ran << 1) ^ ((s64Int) ran < ZERO64B ? POLY : ZERO64B);
            Whichchare = (ran >> logLocalTableSize) & (numofchares - 1);
            if(Whichchare == thisIndex)
            {
                LocalOffset = (ran & (TableSize - 1)) - GlobalStartMyProc;
                HPCC_Table[LocalOffset] ^= ran;
            }else
            {
                HPCC_InsertUpdate(ran, Whichchare, Buckets);
                pendingUpdates++;
            }
            i++;
        }else
        {
            pe = HPCC_GetMaxUpdates(Buckets, &peUpdates);
            PassData *remotedata = new (peUpdates, 0) PassData(peUpdates, thisIndex);
            HPCC_GetUpdates(Buckets, remotedata->getBuffer(), pe, peUpdates);
            pendingUpdates -= peUpdates;
            thisProxy[pe].updatefromremote(remotedata);
        }
    }

    while(pendingUpdates > 0)
    {
        pe = HPCC_GetMaxUpdates(Buckets, &peUpdates);
        PassData *remotedata = new (peUpdates, 0) PassData(peUpdates, thisIndex);
        HPCC_GetUpdates(Buckets, remotedata->getBuffer(), pe, peUpdates);
        
        pendingUpdates -= peUpdates;
        thisProxy[pe].updatefromremote(remotedata);
    }
}
/* For better performance, message will be better than method parameters */
void Updater::updatefromremote(PassData* remotedata)
{
    int j;
    u64Int LocalOffset;
    u64Int inmsg;
    int size = remotedata->size;
    u64Int *data = remotedata->data;
    for(j=0; j<size; j++)
    {
        inmsg = *((u64Int*)data+j);
        LocalOffset = inmsg & (LocalTableSize - 1);
        HPCC_Table[LocalOffset] ^= inmsg;
    }
}

void Updater::verifyfromremote(PassData* remotedata)
{
    int j;
    u64Int LocalOffset;
    u64Int inmsg;
    int size = remotedata->size;
    u64Int *data = remotedata->data;
    int src = remotedata->src;
    for(j=0; j<size; j++)
    {
        inmsg = *((u64Int*)data+j);
        LocalOffset = (inmsg & (LocalTableSize - 1));
        HPCC_Table[LocalOffset] ^= inmsg;
    }
    alreadyReceive[src]  += size;
    /* all are done */
    if(shouldReceive[src] == alreadyReceive[src])
    {
        verifydone++;
        if(verifydone == numofchares)
        {
            NumErrors = 0;
            for (j=0; j<LocalTableSize; j++)
                if (HPCC_Table[j] != j + GlobalStartMyProc)
                    NumErrors++;
            free(shouldReceive);
            free(alreadyReceive);
            mainProxy.collectVerification(NumErrors);
        }
    }
}


void  Updater::verify()
{
    int NumProcs;
    int MyProc;
    s64Int *NumErrors;
    u64Int Ran;
    u64Int RanTmp;
    s64Int NextSlot;
    s64Int WhichChare;
    s64Int PeBucketBase;
    s64Int SendCnt;
    s64Int errors;
    int i;
    int j;

    u64Int *LocalBuckets;     /* buckets used in verification phase */
    u64Int *sentCounts;

    MyProc = thisIndex;
    sentCounts = (u64Int*) malloc ( sizeof(u64Int) * numofchares);
    LocalBuckets = (u64Int*) malloc ( sizeof(u64Int) * numofchares*(BUCKET_SIZE+FIRST_SLOT));
    SendCnt  = 4 * LocalTableSize; 
    Ran= nth_random( 4 * thisIndex * LocalTableSize);

    for(i=0; i<numofchares; i++)
    {
        sentCounts[i] = 0;
    }
    while (SendCnt > 0) {
        /* Initalize local buckets */
        for (i=0; i<numofchares; i++){
            PeBucketBase = i * (BUCKET_SIZE+FIRST_SLOT);
            LocalBuckets[PeBucketBase+SLOT_CNT] = FIRST_SLOT;
        }

        /* Fill local buckets until one is full or out of data */
        NextSlot = FIRST_SLOT;
        while(NextSlot != (BUCKET_SIZE+FIRST_SLOT) && SendCnt>0 ) {
            Ran = (Ran << 1) ^ ((s64Int) Ran < ZERO64B ? POLY : ZERO64B);
            WhichChare = (Ran >> (logLocalTableSize)) & (numofchares - 1);
            PeBucketBase = WhichChare * (BUCKET_SIZE+FIRST_SLOT);
            NextSlot = LocalBuckets[PeBucketBase+SLOT_CNT];
            LocalBuckets[PeBucketBase+NextSlot] = Ran;
            LocalBuckets[PeBucketBase+SLOT_CNT] = ++NextSlot;
            SendCnt--;
        }
        /* Now move all the buckets to the appropriate pe */
        for(i=0; i<numofchares; i++)
        {
            PeBucketBase = i * (BUCKET_SIZE+FIRST_SLOT);
            if(LocalBuckets[PeBucketBase+SLOT_CNT]  == FIRST_SLOT)
                continue;
            int updatenum = LocalBuckets[PeBucketBase+SLOT_CNT] - FIRST_SLOT;
            PassData *remotedata = new (updatenum) PassData(updatenum, thisIndex);
            remotedata->fillData(LocalBuckets+PeBucketBase+FIRST_SLOT);
            thisProxy[i].verifyfromremote(remotedata);
            sentCounts[i] += updatenum;
        }
    }

    for(i=0; i<numofchares; i++)
    {
        thisProxy[i].verifysentDone(thisIndex, sentCounts[i]);
    }
    free (sentCounts);
    free( LocalBuckets );
    return;
}

void Updater::verifysentDone(int src, u64Int num)
{
    int j;
    shouldReceive[src] = num;

    if(num == 0 || shouldReceive[src] == alreadyReceive[src])
    {
        verifydone++;
        if(verifydone == numofchares)
        {
            NumErrors = 0;
            for (j=0; j<LocalTableSize; j++)
                if (HPCC_Table[j] != j + GlobalStartMyProc)
                    NumErrors++;
            free(shouldReceive);
            free(alreadyReceive);
            mainProxy.collectVerification(NumErrors);
        }

    }
}
/*
 * Start the random number generation at the nth step.
 * Taken from hpcs reference implementation.
 */
u64Int nth_random(int64_t n)
{
    int i, j;
    u64Int m2[64];
    u64Int temp, ran;

    while (n < 0) n += PERIOD;
    while (n > PERIOD) n -= PERIOD;
    if (n == 0) return 0x1;

    temp = 0x1;
    for (i=0; i<64; i++) {
        m2[i] = temp;
        temp = (temp << 1) ^ ((int64_t) temp < 0 ? POLY : 0);
        temp = (temp << 1) ^ ((int64_t) temp < 0 ? POLY : 0);
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
            ran = (ran << 1) ^ ((int64_t) ran < 0 ? POLY : 0);
    }

    return ran;
}

#include "RandomAccess.def.h"
