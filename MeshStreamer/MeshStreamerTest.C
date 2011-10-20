#include "MeshStreamerTest.decl.h"
#include "MeshStreamer.h"

CProxy_Senders senderGroup;
CProxy_MeshStreamer aggregator; 


#define PAYLOAD_SIZE 8
#define BUCKET_SIZE 1
#define NUM_ROWS 1
#define NUM_COLUMNS 1
#define NUM_PLANES 1
#define NUM_PES_PER_NODE 2
#define FLUSH_PERIOD_IN_MS 10


class Main: public CBase_Main {
public:
  Main(CkArgMsg *m) {

    senderGroup = CProxy_Senders::ckNew();
    aggregator = 
      CProxy_MeshStreamer::ckNew(PAYLOAD_SIZE, BUCKET_SIZE, 
                                 NUM_ROWS, NUM_COLUMNS, NUM_PLANES, 
                                 NUM_PES_PER_NODE, 
                                 static_cast<CProxy_MeshStreamerClient>(senderGroup),  
                                 FLUSH_PERIOD_IN_MS);

    senderGroup[0].runTest();
  }
};

class Senders: public MeshStreamerClient {
public:
  Senders() {

  }
  
  void receiveCombinedData(LocalMessage *msg) {
    CkPrintf("Pe %d receiving message \n", CkMyPe());
    delete msg; 
  }

  void runTest() {
    // send to neighbor
    double data = 1; 

    MeshStreamerMessage *msg = new (1, PAYLOAD_SIZE) MeshStreamerMessage(PAYLOAD_SIZE);
    msg->destinationPes[0] = 1; 
    aggregator[CkMyPe()].insertData(msg);       
  }

};


#include "MeshStreamerTest.def.h"
