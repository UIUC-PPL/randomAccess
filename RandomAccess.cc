#include "RandomAccess.h"

/* Readonly variables */
CProxy_Main main;
CProxy_DataTable table_array;
CProxy_Updater updater_array;

int chunk_size;
int num_table_entries;
int num_table_chunks;
int num_updaters;
/* end readonly variables */

Main::Main(CkArgMsg* args) :
    total_wrong_entries(0), verification_checkins(0)
{
    CkAssert(args->argc == 3);
    num_table_entries = atoi(args->argv[1]);
    int iterations = atoi(args->argv[2]);
    chunk_size = 1024;
    main = thishandle;
    
    num_table_chunks = CkNumPes();
    num_updaters = CkNumPes();

    table_array = CProxy_DataTable::ckNew(num_table_entries / num_table_chunks, num_table_chunks);
    updater_array = CProxy_Updater::ckNew(iterations, num_updaters);

    updater_array.generateUpdates(iterations);
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
    table(new uint64_t[entries])
{
    for (int i=0; i<num_entries; ++i) table[i] = base_index + i;
}

void DataTable::verify()
{
    int wrong_entries = 0;
    for (int i=0; i<num_entries; ++i) if (table[i] != base_index + i) wrong_entries++;
    main.collectVerification(wrong_entries);
}

void DataTable::doUpdates(uint64_t* updates, int num_updates)
{
    for (int i=0; i<num_updates; ++i) {
        uint64_t datum = updates[i];
        int index = datum & (num_entries - 1);
        table[index] ^= datum;
    }
}


#define POLY 0x0000000000000007ULL
#define PERIOD 1317624576693539401LL
Updater::Updater(int base_index) :
    ran(nth_random(base_index)),
    buffer(new uint64_t[chunk_size])
{ } 

void Updater::generateUpdates(int num_updates)
{
    for (int i=0; i<num_updates; i+=chunk_size) {
        for (int j=0; j<chunk_size; ++j) {
            ran = (ran << 1) ^ ((int64_t)ran < 0LL ? POLY : 0LL);
            buffer[i] = ran;
        }
        table_array[thisIndex].doUpdates(buffer, chunk_size);
    }
}

/*
 * Start the random number generation at the nth step.
 * Taken from hpcs reference implementation.
 */
uint64_t Updater::nth_random(int64_t n)
{
    int i, j;
    uint64_t m2[64];
    uint64_t temp, ran;

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
