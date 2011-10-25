#include "MeshStreamer.h"

// allocate more total buffer space then the maximum buffering limit but flush upon
// reaching totalBufferCapacity_
#define BUCKET_SIZE_FACTOR 4


//#define DEBUG_STREAMER 1

MeshStreamerClient::MeshStreamerClient() {}

void MeshStreamerClient::receiveCombinedData(MeshStreamerMessage *msg) {
  CkError("Default implementation of receiveCombinedData should never be called\n");
  delete msg;
}

MeshStreamer::MeshStreamer(int dataItemSize, int totalBufferCapacity, int numRows, 
                           int numColumns, int numPlanes, 
                           const CProxy_MeshStreamerClient &clientProxy) {
   
  dataItemSize_ = dataItemSize; 
  // limit total number of messages in system to totalBufferCapacity
  //   but allocate a factor BUCKET_SIZE_FACTOR more space to take
  //   advantage of nonuniform filling of buckets
  // the buffers for your own column and plane are never used
  bucketSize_ = BUCKET_SIZE_FACTOR * totalBufferCapacity / (numRows + numColumns + numPlanes - 2); 
  totalBufferCapacity_ = totalBufferCapacity;
  numDataItemsBuffered_ = 0; 
  numRows_ = numRows; 
  numColumns_ = numColumns;
  numPlanes_ = numPlanes; 
  numNodes_ = CkNumPes(); 
  clientProxy_ = clientProxy; 

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
  myNodeIndex_ = CkMyPe();
  planeSize_ = numRows_ * numColumns_; 
  myPlaneIndex_ = myNodeIndex_ / planeSize_; 
  int indexWithinPlane = myNodeIndex_ - myPlaneIndex_ * planeSize_;
  myRowIndex_ = indexWithinPlane / numColumns_;
  myColumnIndex_ = indexWithinPlane - myRowIndex_ * numColumns_; 

}

MeshStreamer::~MeshStreamer() {

  for (int i = 0; i < numRows_; i++)
      delete personalizedBuffers_[i]; 

  for (int i = 0; i < numColumns_; i++)
      delete columnBuffers_[i]; 

  for (int i = 0; i < numPlanes_; i++)
      delete planeBuffers_[i]; 

  delete[] personalizedBuffers_;
  delete[] columnBuffers_;
  delete[] planeBuffers_; 

}

void MeshStreamer::determineLocation(const int destinationPe, int &rowIndex, 
                                     int &columnIndex, int &planeIndex, 
                                     MeshStreamerMessageType &msgType) {
  
  int nodeIndex, indexWithinPlane; 

  nodeIndex = destinationPe;
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
    int dataSize = bucketSize_ * dataItemSize_;  
    if (msgType == PersonalizedMessage) {
      messageBuffers[bucketIndex] = 
        new (0, dataSize) MeshStreamerMessage(dataItemSize_);
    }
    else {
      messageBuffers[bucketIndex] = 
        new (bucketSize_, dataSize) MeshStreamerMessage(dataItemSize_);
    }
#ifdef DEBUG_STREAMER
    CkAssert(messageBuffers[bucketIndex] != NULL);
#endif
  }
  
  MeshStreamerMessage *destinationBucket = messageBuffers[bucketIndex];
  
  int numBuffered = destinationBucket->addDataItem(data); 
  if (msgType != PersonalizedMessage) {
    destinationBucket->markDestination(numBuffered-1, destinationPe);
  }
  numDataItemsBuffered_++;
  // copy data into message and send if buffer is full
  if (numBuffered == bucketSize_) {

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
      clientProxy_[destinationIndex].receiveCombinedData(destinationBucket);      
      //      thisProxy[destinationIndex].receivePersonalizedData(destinationBucket);
      break;
    default: 
      CkError("Incorrect MeshStreamer message type\n");
      break;
    }
    messageBuffers[bucketIndex] = NULL;
    numDataItemsBuffered_ -= numBuffered; 
  }

  if (numDataItemsBuffered_ == totalBufferCapacity_) {

    flushLargestBucket(personalizedBuffers_, numRows_, myRowIndex_, numColumns_);
    flushLargestBucket(columnBuffers_, numColumns_, myColumnIndex_, 1);
    flushLargestBucket(planeBuffers_, numPlanes_, myPlaneIndex_, planeSize_);

  }

}

void MeshStreamer::insertData(void *data, int destinationPe) {

  int planeIndex, columnIndex, rowIndex; // location of destination
  int indexWithinPlane; 

  MeshStreamerMessageType msgType; 

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
}

void MeshStreamer::receiveAggregateData(MeshStreamerMessage *msg) {

  int rowIndex, columnIndex, planeIndex, destinationPe; 
  MeshStreamerMessageType msgType;   

  for (int i = 0; i < msg->numDataItems; i++) {
    destinationPe = msg->destinationPes[i];
    void *dataItem = msg->getDataItem(i);
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
                 columnIndex, planeIndex, msgType, dataItem);
    
  }

  delete msg;

}

/*
void MeshStreamer::receivePersonalizedData(MeshStreamerMessage *msg) {

  // sort data items into messages for each core on this node

  LocalMessage *localMsgs[numPesPerNode_];
  int dataSize = bucketSize_ * dataItemSize_;

  for (int i = 0; i < numPesPerNode_; i++) {
    localMsgs[i] = new (dataSize) LocalMessage(dataItemSize_);
  }

  int destinationPe;
  for (int i = 0; i < msg->numDataItems; i++) {

    destinationPe = msg->destinationPes[i]; 
    void *dataItem = msg->getDataItem(i);   
    localMsgs[destinationPe % numPesPerNode_]->addDataItem(dataItem);

  }

  for (int i = 0; i < numPesPerNode_; i++) {
    if (localMsgs[i]->numDataItems > 0) {
      clientProxy_[myNodeIndex_ * numPesPerNode_ + i].receiveCombinedData(localMsgs[i]);
    }
    else {
      delete localMsgs[i];
    }
  }

  delete msg; 

}
*/

void MeshStreamer::flushLargestBucket(MeshStreamerMessage **messageBuffers,
                                      const int numBuffers, const int myIndex, 
                                      const int dimensionFactor) {

  int flushIndex, maxSize, destinationIndex;
  MeshStreamerMessage *destinationBucket; 
  maxSize = 0;
  for (int i = 0; i < numBuffers; i++) {
    if (messageBuffers[i] != NULL && messageBuffers[i]->numDataItems > maxSize) {
      maxSize = messageBuffers[i]->numDataItems;
      flushIndex = i;
    } 
  }
  if (maxSize > 0) {
    destinationBucket = messageBuffers[flushIndex];
    destinationIndex = myNodeIndex_ + (flushIndex - myIndex) * dimensionFactor;
    if (destinationBucket != NULL) {
      numDataItemsBuffered_ -= destinationBucket->numDataItems;
    }
    if (messageBuffers == personalizedBuffers_) {
      clientProxy_[destinationIndex].receiveCombinedData(destinationBucket);
    }
    else {
      thisProxy[destinationIndex].receiveAggregateData(destinationBucket);
    }
    messageBuffers[flushIndex] = NULL;
  }
}

void MeshStreamer::flushBuckets(MeshStreamerMessage **messageBuffers, int numBuffers)
{

    for (int i = 0; i < numBuffers; i++) {
       if(messageBuffers[i] == NULL)
           continue;
       //flush all messages in i bucket
       numDataItemsBuffered_ -= messageBuffers[i]->numDataItems;
       if (messageBuffers == personalizedBuffers_) {
         int destinationPe = myNodeIndex_ + (i - myRowIndex_) * numColumns_; 
         clientProxy_[destinationPe].receiveCombinedData(messageBuffers[i]);
       }
       else {
         for (int j = 0; j < messageBuffers[i]->numDataItems; j++) {
           MeshStreamerMessage *directMsg = 
             new (0, dataItemSize_) MeshStreamerMessage(dataItemSize_);
#ifdef DEBUG_STREAMER
           CkAssert(directMsg != NULL);
#endif
           int destinationPe = messageBuffers[i]->destinationPes[j]; 
           void *dataItem = messageBuffers[i]->getDataItem(j);   
           directMsg->addDataItem(dataItem);
           clientProxy_[destinationPe].receiveCombinedData(directMsg);
           delete messageBuffers[i];
         }
       }
       messageBuffers[i] = NULL;
    }

#ifdef DEBUG_STREAMER
    CkPrintf("[%d] numDataItemsBuffered_: %d\n", CkMyPe(), numDataItemsBuffered_);
    CkAssert(numDataItemsBuffered_ == 0); 
#endif
}

void MeshStreamer::flushDirect(){
    flushBuckets(planeBuffers_, numPlanes_);
    flushBuckets(columnBuffers_, numColumns_);
    flushBuckets(personalizedBuffers_, numRows_);
}

#include "MeshStreamer.def.h"
