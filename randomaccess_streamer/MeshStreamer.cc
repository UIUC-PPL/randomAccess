#include "MeshStreamer.h"

MeshStreamerClient::MeshStreamerClient() {

}


void MeshStreamerClient::receiveCombinedData(LocalMessage *msg) {

  delete msg;
}


MeshStreamer::MeshStreamer(int payloadSize, int totalBufferCapacity, int numRows, 
                           int numColumns, int numPlanes, int numPesPerNode,
                           const CProxy_MeshStreamerClient &clientProxy, 
                           int flushPeriodInMs) {
   
  payloadSize_ = payloadSize; 
  // limit total number of messages in system to totalBufferCapacity
  // the buffers for your own column and plane are never used
  bucketSize_ = totalBufferCapacity / (numRows + numColumns + numPlanes - 2); 
  numRows_ = numRows; 
  numColumns_ = numColumns;
  numPlanes_ = numPlanes; 
  numNodes_ = CkNumNodes(); 
  numPesPerNode_ = numPesPerNode;
  clientProxy_ = clientProxy; 
  flushPeriod_ = flushPeriodInMs;

  personalizedBuffers_ = new MeshStreamerMessage*[numRows];
  for (int i = 0; i < numRows; i++) {
    personalizedBuffers_[i] = NULL; 
  }

  columnBuffers_ = new MeshStreamerMessage*[numColumns];
  for (int i = 0; i < numColumns; i++) {
    columnBuffers_[i] = NULL; 
  }

  planeBuffers_ = new MeshStreamerMessage*[numPlanes]; 
  for (int i = 0; i < numPlanes; i++) {
    planeBuffers_[i] = NULL; 
  }

  // determine plane, column, and row location of this node
  myNodeIndex_ = CkMyNode();
  planeSize_ = numRows_ * numColumns_; 
  myPlaneIndex_ = myNodeIndex_ / planeSize_; 
  int indexWithinPlane = myNodeIndex_ - myPlaneIndex_ * planeSize_;
  myRowIndex_ = indexWithinPlane / numColumns_;
  myColumnIndex_ = indexWithinPlane - myRowIndex_ * numColumns_; 

}

MeshStreamer::~MeshStreamer() {

  for (int i = 0; i < numRows_; i++) {
    if (personalizedBuffers_[i] != NULL) {
      delete personalizedBuffers_[i]; 
    }
  }

  for (int i = 0; i < numColumns_; i++) {
    if (columnBuffers_[i] != NULL) {
      delete columnBuffers_[i]; 
    }
  }

  for (int i = 0; i < numPlanes_; i++) {
    if (planeBuffers_[i] != NULL) {
      delete planeBuffers_[i]; 
    } 
  }

  delete[] personalizedBuffers_;
  delete[] columnBuffers_;
  delete[] planeBuffers_; 

}

void MeshStreamer::determineLocation(const int destinationPe, int &rowIndex, 
                                     int &columnIndex, int &planeIndex, 
                                     MeshStreamerMessageType &msgType) {
  
  int nodeIndex, indexWithinPlane; 

  nodeIndex = destinationPe / numPesPerNode_;
  planeIndex = nodeIndex / planeSize_; 
  if (planeIndex != myPlaneIndex_) {
    msgType = PlaneMessage;     
  }
  else {
    indexWithinPlane = nodeIndex - planeIndex * planeSize_;
    rowIndex = indexWithinPlane / numColumns_;
    columnIndex = indexWithinPlane - rowIndex * numColumns_; 
    if (columnIndex != myColumnIndex_) {
      msgType = ColumnMessage; 
    }
    else {
      msgType = PersonalizedMessage;
    }
  }

}

void MeshStreamer::storeMessage(MeshStreamerMessage **messageBuffers, 
                                const int bucketIndex, const int destinationPe, 
                                const int rowIndex, const int columnIndex, 
                                const int planeIndex, 
                                const MeshStreamerMessageType msgType, 
                                void *data) {

  // allocate new message if necessary
  if (messageBuffers[bucketIndex] == NULL) {
    int dataSize = bucketSize_ * payloadSize_;  
    messageBuffers[bucketIndex] = 
      new (bucketSize_, dataSize) MeshStreamerMessage(payloadSize_);
  }
  
  MeshStreamerMessage *destinationBucket = messageBuffers[bucketIndex];

  // copy data into message and send if buffer is full
  if (destinationBucket->addData(data, destinationPe) == bucketSize_) {

    int destinationIndex;
    switch (msgType) {

    case PlaneMessage:
      destinationIndex = 
        myNodeIndex_ + (planeIndex - myPlaneIndex_) * planeSize_;      
      thisProxy[destinationIndex].receiveAggregateData(destinationBucket);
      break;
    case ColumnMessage:
      destinationIndex = myNodeIndex_ + (columnIndex - myColumnIndex_);
      thisProxy[destinationIndex].receiveAggregateData(destinationBucket);
      break;
    case PersonalizedMessage:
      destinationIndex = myNodeIndex_ + (rowIndex - myRowIndex_) * numColumns_;
      thisProxy[destinationIndex].receivePersonalizedData(destinationBucket);
      break;
    default: 
      CkError("Incorrect MeshStreamer message type\n");
      break;
    }
    messageBuffers[bucketIndex] = NULL;
  }

}

void MeshStreamer::insertData(MeshStreamerMessage *msg) {

  int planeIndex, columnIndex, rowIndex; // location of destination
  int indexWithinPlane; 

  MeshStreamerMessageType msgType; 

  void *data = msg->data;
  int destinationPe = msg->destinationPes[0];

  determineLocation(destinationPe, rowIndex, columnIndex, planeIndex, msgType);

  // determine which array of buffers is appropriate for this message
  MeshStreamerMessage **messageBuffers;
  int bucketIndex; 

  switch (msgType) {
  case PlaneMessage:
    messageBuffers = planeBuffers_; 
    bucketIndex = planeIndex; 
    break;
  case ColumnMessage:
    messageBuffers = columnBuffers_; 
    bucketIndex = columnIndex; 
    break;
  case PersonalizedMessage:
    messageBuffers = personalizedBuffers_; 
    bucketIndex = rowIndex; 
    break;
  default: 
    CkError("Unrecognized MeshStreamer message type\n");
    break;
  }

  storeMessage(messageBuffers, bucketIndex, destinationPe, rowIndex, 
               columnIndex, planeIndex, msgType, data);


  delete msg; 
}

void MeshStreamer::receiveAggregateData(MeshStreamerMessage *msg) {

  int rowIndex, columnIndex, planeIndex, destinationPe; 
  MeshStreamerMessageType msgType;   

  for (int i = 0; i < msg->numElements; i++) {
    destinationPe = msg->destinationPes[i];
    void *dataFragment = msg->getFragment(i);
    determineLocation(destinationPe, rowIndex, columnIndex, 
                      planeIndex, msgType);
#ifdef DEBUG_STREAMER
    CkAssert(planeIndex == myPlaneIndex_);

    if (msgType == PersonalizedMessage) {
      CkAssert(columnIndex == myColumnIndex_);
    }
#endif    

    MeshStreamerMessage **messageBuffers;
    int bucketIndex; 

    switch (msgType) {
    case ColumnMessage:
      messageBuffers = columnBuffers_; 
      bucketIndex = columnIndex; 
      break;
    case PersonalizedMessage:
      messageBuffers = personalizedBuffers_; 
      bucketIndex = rowIndex; 
      break;
    default: 
      CkError("Incorrect MeshStreamer message type\n");
      break;
    }

    storeMessage(messageBuffers, bucketIndex, destinationPe, rowIndex, 
                 columnIndex, planeIndex, msgType, dataFragment);
    
  }

  delete msg;

}

void MeshStreamer::receivePersonalizedData(MeshStreamerMessage *msg) {
  
  // sort fragments into messages for each core on this node

  LocalMessage *localMsgs[numPesPerNode_];
  int dataSize = bucketSize_ * payloadSize_;

  for (int i = 0; i < numPesPerNode_; i++) {
    localMsgs[i] = new (bucketSize_, dataSize) LocalMessage(payloadSize_);
  }

  int destinationPe;
  for (int i = 0; i < msg->numElements; i++) {

    destinationPe = msg->destinationPes[i]; 
    void *dataFragment = msg->getFragment(i);   
    localMsgs[destinationPe % numPesPerNode_]->addData(dataFragment);

  }

  for (int i = 0; i < numPesPerNode_; i++) {
    if (localMsgs[i]->numElements > 0) {
      clientProxy_[myNodeIndex_ * numPesPerNode_ + i].receiveCombinedData(localMsgs[i]);
    }
    else {
      delete localMsgs[i];
    }
  }

  delete msg; 
}

void MeshStreamer::flush() {

}

void periodicFlushHandler(void *streamer, double time) {
  ((MeshStreamer *) streamer)->flush();
}

void MeshStreamer::registerPeriodicFlush() {
  CcdCallFnAfter(periodicFlushHandler, (void *) this, flushPeriod_);
}

#include "MeshStreamer.def.h"
