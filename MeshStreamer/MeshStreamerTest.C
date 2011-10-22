#include "MeshStreamerTest.decl.h"
#include "MeshStreamer.h"

CProxy_Senders senderGroup;
CProxy_MeshStreamer aggregator; 


#define PAYLOAD_SIZE 4
#define BUCKET_SIZE 2
#define NUM_ROWS 2
#define NUM_COLUMNS 2
#define NUM_PLANES 2
#define NUM_PES_PER_NODE 2
#define FLUSH_PERIOD_IN_MS 10


class Main: public CBase_Main {
public:
  Main(CkArgMsg *m) {

    senderGroup = CProxy_Senders::ckNew();
    int totalBufferCapacity = BUCKET_SIZE * (NUM_ROWS + NUM_COLUMNS + NUM_PLANES - 2); 
    aggregator = 
      CProxy_MeshStreamer::ckNew(PAYLOAD_SIZE, totalBufferCapacity,
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
    CkPrintf("[%d] receiving message for %d \n", CkMyPe(), ((int *) msg->data)[0]);
    delete msg; 
  }

  void runTest() {
    // send to neighbor
    
    MeshStreamerMessage *msg;
    int destinationPe;

    for (int i = 0; i < BUCKET_SIZE; i++) {
      destinationPe = 1; 
      msg = new (1, PAYLOAD_SIZE) MeshStreamerMessage(PAYLOAD_SIZE);
      msg->addData((void *) &destinationPe, destinationPe);
      aggregator[CkMyNode()].insertData(msg);       
      msg = NULL;

      destinationPe = 3; 
      msg = new (1, PAYLOAD_SIZE) MeshStreamerMessage(PAYLOAD_SIZE);
      msg->addData((void *) &destinationPe, destinationPe);
      aggregator[CkMyNode()].insertData(msg);  
      msg = NULL;

      destinationPe = 5; 
      msg = new (1, PAYLOAD_SIZE) MeshStreamerMessage(PAYLOAD_SIZE);
      msg->addData((void *) &destinationPe, destinationPe);
      aggregator[CkMyNode()].insertData(msg);       
      msg = NULL;

      destinationPe = 7; 
      msg = new (1, PAYLOAD_SIZE) MeshStreamerMessage(PAYLOAD_SIZE);
      msg->addData((void *) &destinationPe, destinationPe);
      aggregator[CkMyNode()].insertData(msg);  
      msg = NULL;

      destinationPe = 9; 
      msg = new (1, PAYLOAD_SIZE) MeshStreamerMessage(PAYLOAD_SIZE);
      msg->addData((void *) &destinationPe, destinationPe);
      aggregator[CkMyNode()].insertData(msg);       
      msg = NULL;

      destinationPe = 11; 
      msg = new (1, PAYLOAD_SIZE) MeshStreamerMessage(PAYLOAD_SIZE);
      msg->addData((void *) &destinationPe, destinationPe);
      aggregator[CkMyNode()].insertData(msg);  
      msg = NULL;

      destinationPe = 13; 
      msg = new (1, PAYLOAD_SIZE) MeshStreamerMessage(PAYLOAD_SIZE);
      msg->addData((void *) &destinationPe, destinationPe);
      aggregator[CkMyNode()].insertData(msg);       
      msg = NULL;

      destinationPe = 15; 
      msg = new (1, PAYLOAD_SIZE) MeshStreamerMessage(PAYLOAD_SIZE);
      msg->addData((void *) &destinationPe, destinationPe);
      aggregator[CkMyNode()].insertData(msg);  
      msg = NULL;


    }
  }

};


#include "MeshStreamerTest.def.h"
