#include "converse.h"

#ifdef LONG_IS_64BITS
#define ZERO64B 0L
#define POLY 0x0000000000000007UL
#define PERIOD 1317624576693539401L
#else
#define ZERO64B 0LL
#define POLY 0x0000000000000007ULL
#define PERIOD 1317624576693539401LL
#endif

#define  UPDATE_QUIESCENCE  0
#define  VERIFY_QUIESCENCE 1

int logLocalTableSize;
CmiInt8 localTableSize;
CmiInt8 tableSize;
int numOfUpdaters;

CmiInt8 HPCC_starts(CmiInt8 n);

typedef struct mesg
{
  char head[CmiMsgHeaderSizeBytes];
  CmiInt8 val;
}       
IntMsg;

int initialize_handler;
int start_handler;
int generateUpdates_handler;
int updateLocalTable_handler;
int checkErrors_handler;
int verifyDone_handler;
int shutdown_handler;

double starttime;
int phase;

void allUpdatesDone(void *msg, double t);

void start(void *msg)
{
  CmiPrintf("\nstart RandomAccess\n");
  starttime = CmiWallTimer();
  CmiSetHandler(msg, generateUpdates_handler);
  CmiSyncBroadcastAllAndFree(CmiMsgHeaderSizeBytes, msg);
  phase = UPDATE_QUIESCENCE;      //randomAccess phase 
  CmiStartQD(allUpdatesDone, NULL);
}

void allUpdatesDone(void *msg, double t)
{
        double singlegups;
        double gups;
        double update_walltime;
        
        update_walltime = CmiWallTimer() - starttime;
        if(phase == UPDATE_QUIESCENCE)
        {
            gups = 1e-9 * tableSize * 4.0/update_walltime;
            singlegups =  gups/numOfUpdaters;
            CmiPrintf( "CPU time used = %.6f seconds\n", update_walltime );
            CmiPrintf( "%.9f Billion(10^9) Updates    per second [GUP/s]\n",  gups);
            CmiPrintf( "%.9f Billion(10^9) Updates/PE per second [GUP/s]\n", singlegups );
            CmiPrintf("\n\nStart verifying...\n");
            starttime = CmiWallTimer();
            phase = VERIFY_QUIESCENCE;      //verify
            void *msg = CmiAlloc(CmiMsgHeaderSizeBytes);
            CmiSetHandler(msg, generateUpdates_handler);
            CmiSyncBroadcastAllAndFree(CmiMsgHeaderSizeBytes, msg);
            CmiStartQD(allUpdatesDone, NULL);
        }else if(phase == VERIFY_QUIESCENCE)
        {
            //verify done
            void *msg = CmiAlloc(CmiMsgHeaderSizeBytes);
            CmiSetHandler(msg, checkErrors_handler);
            CmiSyncBroadcastAllAndFree(CmiMsgHeaderSizeBytes, msg);
        }
}

void shutdown(void *msg)
{
  ConverseExit();
}

void verifyDone(IntMsg *msg) 
{
  CmiInt8 GlbnumErrors = msg->val;
  CmiPrintf(  "Found %lld errors in %lld locations (%s).\n", GlbnumErrors, 
            tableSize, (GlbnumErrors <= 0.01*tableSize) ? "passed" : "failed");
  CmiSetHandler(msg, shutdown_handler);
  CmiSyncBroadcastAllAndFree(CmiMsgHeaderSizeBytes, msg);
}

void * nop(int *size, void *data, void **remote, int count) {
  return data;
}


CmiInt8 *HPCC_Table;
CmiInt8 globalStartmyProc;

void initialize(void *msg)
{
    HPCC_Table = (CmiInt8*)malloc(sizeof(CmiInt8) * localTableSize);
    for(CmiInt8 i=0; i<localTableSize; i++)
      HPCC_Table[i] = i + globalStartmyProc;
    CmiSetHandler((char *)msg, start_handler);
    CmiReduce(msg, CmiMsgHeaderSizeBytes, nop);
}

void generateUpdates(void *msg)
{
        CmiInt8 updatesNum;
        CmiInt8 ran, localOffset;
        int tableIndex;
        ran= HPCC_starts(4* globalStartmyProc);
        updatesNum = 4 * localTableSize;
        int count = 0;
        for(CmiInt8 i=0; i<updatesNum;i++)
        {
            ran = (ran << 1) ^ ((CmiInt8) ran < ZERO64B ? POLY : ZERO64B);
            tableIndex = (ran >>  logLocalTableSize)&(numOfUpdaters-1);
            if(tableIndex ==  CmiMyPe())
            {
                localOffset = (ran&(tableSize-1))-globalStartmyProc;
                HPCC_Table[localOffset] ^= ran;
            }
            else {
                //sending messages out and receive message to apply the update table
                IntMsg *msg = (IntMsg*)CmiAlloc(sizeof(IntMsg));
                msg->val = ran;
                CmiSetHandler((char *)msg, updateLocalTable_handler);
                CmiSyncSendAndFree(tableIndex, sizeof(IntMsg), msg);
                if(++count%1024 == 0)
                    CthYield();   
            }
        }
}

void updateLocalTable(IntMsg *msg)
{
    CmiInt8 ran = msg->val;
    CmiInt8 localOffset;
    localOffset = ran & (localTableSize - 1);
    HPCC_Table[localOffset] ^= ran;
    CmiFree(msg);
}

void * sum_long(int *size, void *data, void **remote, int count) {
  int i;
  IntMsg *local = (IntMsg *)data;
  CmiInt8  sum = local->val;
  if (count == 0) return data;
  for (i=0; i<count; i++) sum += ((IntMsg *)remote[i])->val;
  local->val = sum;
  return data;
}

void checkErrors(void *msg)
{
    CmiFree(msg);
    CmiInt8 numErrors = 0;
    for (CmiInt8 j=0; j<localTableSize; j++)
            if (HPCC_Table[j] != j + globalStartmyProc)
                numErrors++;
    IntMsg *rmsg = (IntMsg*)CmiAlloc(sizeof(IntMsg));
    rmsg->val = numErrors;
    CmiSetHandler(rmsg, verifyDone_handler);
    CmiReduce(rmsg, sizeof(IntMsg), sum_long);
}

void main_init(int argc, char** argv) 
{
  initialize_handler = CmiRegisterHandler((CmiHandler)initialize);
  start_handler = CmiRegisterHandler((CmiHandler)start);
  generateUpdates_handler = CmiRegisterHandler((CmiHandler)generateUpdates);
  updateLocalTable_handler = CmiRegisterHandler((CmiHandler)updateLocalTable);
  checkErrors_handler = CmiRegisterHandler((CmiHandler)checkErrors);
  verifyDone_handler = CmiRegisterHandler((CmiHandler)verifyDone);
  shutdown_handler = CmiRegisterHandler((CmiHandler)shutdown);

  logLocalTableSize = atoi(argv[1]);
  numOfUpdaters = CmiNumPes();
  localTableSize = 1l << logLocalTableSize;
  tableSize = localTableSize * numOfUpdaters ;

  if (CmiMyPe() == 0) 
  {
        CmiPrintf("Main table size   = 2^%d * %d = %lld words\n", logLocalTableSize, CmiNumPes(), tableSize);
        CmiPrintf("Number of processes = %d\n", CmiNumPes());
        CmiPrintf("Number of updates = %lld\n", (4*tableSize));
        void *msg = (void *)CmiAlloc(CmiMsgHeaderSizeBytes);
        CmiSetHandler(msg, initialize_handler);
        CmiSyncBroadcastAllAndFree(CmiMsgHeaderSizeBytes, msg);
  }
}

int main(int argc, char **argv)
{
  ConverseInit(argc,argv,main_init,0,0);
}


/** random generator */
CmiInt8 HPCC_starts(CmiInt8 n)
{
    int i, j;
    CmiInt8 m2[64];
    CmiInt8 temp, ran;
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

