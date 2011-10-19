#include "MeshStreamerTest.decl.h"
#include "MeshStreamer.h"

CProxy_Senders senderGroup;
CProxy_MeshStreamer aggregator; 


#define PAYLOAD_SIZE 16 
#define BUCKET_SIZE 4
#define NUM_ROWS 2
#define NUM_COLUMNS 2
#define NUM_PLANES 2
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

    CkExit();
  }
};

class Senders: public MeshStreamerClient {
public:
  Senders() {

  }
  
  void receiveCombinedData(LocalMessage *msg) {

    delete msg; 
  }

};


#include "MeshStreamerTest.def.h"
