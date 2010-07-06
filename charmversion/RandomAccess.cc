#include "hpcc.h"
#include "RandomAccess.decl.h"
#include "buckets.h"
#include "RandomAccess.h"


/* Readonly variables */
CProxy_Main main;
CProxy_DataTable table_array;
CProxy_Updater updater_array;

int logLocalTableSize;
int LocalTableSize;
int TableSize;

int chunk_size;
int num_table_entries;
int num_table_chunks;
int num_updaters;

int INTERREUPT = 1;
int numOfChares;

/* end readonly variables */

Main::Main(CkArgMsg* args) :
    total_wrong_entries(0), verification_checkins(0)
{
    CkAssert(args->argc == 3);
    logLocalTableSize = atoi(args->argv[1]);
    LocalTableSize = 1 << logLocalTableSize;
    TableSize = LocalTableSize * CkNumPes();

    int iterations = atoi(args->argv[2]);
    chunk_size = 1024;
    main = thishandle;
    
    num_table_chunks = CkNumPes();
    num_updaters = CkNumPes();
    numOfChares = CkNumPes();


    table_array = CProxy_DataTable::ckNew(num_table_entries / num_table_chunks, num_table_chunks);
    updater_array = CProxy_Updater::ckNew(iterations, num_updaters);

    updater_array.generateUpdates(iterations);
    table_array.verify();
}

void Main::done() {}

void Main::collectVerification(int wrong_entries)
{
    total_wrong_entries += wrong_entries;
    if (++verification_checkins == num_table_chunks) {
        CkPrintf("%d incorrect entries out of %d\n", total_wrong_entries, num_table_entries);
        CkExit();
    }
}

DataTable::DataTable(int entries) :
    base_index(chunk_size*thisIndex),
    num_entries(entries),
    table(new u64Int[entries])
{
    for (int i=0; i<num_entries; ++i) table[i] = base_index + i;
}

void DataTable::verify()
{
    int wrong_entries = 0;
    for (int i=0; i<num_entries; ++i) if (table[i] != base_index + i) wrong_entries++;
    main.collectVerification(wrong_entries);
}

void DataTable::doUpdates(u64Int* updates, int num_updates)
{
    for (int i=0; i<num_updates; ++i) {
        u64Int datum = updates[i];
        int index = datum & (num_entries - 1);
        table[index] ^= datum;
    }
}

//#define ZERO64B 0L
//#define POLY 0x0000000000000007ULL
//#define PERIOD 1317624576693539401LL
Updater::Updater(int base_index) 
{  
    ran= nth_random(base_index);

    HPCC_Table = (u64Int*)malloc(sizeof(u64Int) * LocalTableSize);
    GlobalStartMyProc = thisIndex;
}
void Updater::generateUpdates(int num_updates)
{
    int Whichchare;
    pendingUpdates = 0;
    maxPendingUpdates = MAX_TOTAL_PENDING_UPDATES;
    localBufferSize = LOCAL_BUFFER_SIZE;


    Buckets = HPCC_InitBuckets(CkNumPes(), maxPendingUpdates);
    //Ran = HPCC_starts (4 * GlobalStartMyProc);

    Updatesnum = 4 * LocalTableSize;
    int i=0; 
    for(i=0; i<Updatesnum;)
    {
        if(i%INTERREUPT == 0)
        {
            CsdSchedulePoll();
        }

        if (pendingUpdates < maxPendingUpdates)
        {
            ran = (ran << 1) ^ ((s64Int) ran < ZERO64B ? POLY : ZERO64B);
            Whichchare = (ran >> logLocalTableSize) & (numOfChares - 1);
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
            pe = HPCC_GetUpdates(Buckets, LocalSendBuffer, localBufferSize, &peUpdates);
            pendingUpdates -= peUpdates;
            thisProxy[pe].updatefromremote(peUpdates, LocalSendBuffer);//(remotedata);
        }
    }

    while(pendingUpdates > 0)
    {
        pe = HPCC_GetUpdates(Buckets, LocalSendBuffer, localBufferSize, &peUpdates);
        pendingUpdates -= peUpdates;
        thisProxy[pe].updatefromremote(peUpdates, LocalSendBuffer);//(remotedata);
    }
    /* When to exit the whole program */
    /* broadcast the message that I am done to indicate that I will never send message. Once this processor receives all other done messages from others, it can send */

}
/* For better performance, message will be better than method parameters */
void Updater::updatefromremote(int size, u64Int data[])
{
    int j;
    u64Int LocalOffset;
    u64Int inmsg;
    for(j=0; j<size; j++)
    {
        inmsg = *((u64Int*)data+j);
        LocalOffset = (inmsg & (TableSize - 1)) - GlobalStartMyProc;
        HPCC_Table[LocalOffset] ^= inmsg;

    }
}
/*
 * Start the random number generation at the nth step.
 * Taken from hpcs reference implementation.
 */
u64Int Updater::nth_random(int64_t n)
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
