#ifndef _MESH_STREAMER_H_
#define _MESH_STREAMER_H_

#include "MeshStreamer.decl.h"

enum MeshStreamerMessageType {PlaneMessage, ColumnMessage, PersonalizedMessage};

class MeshStreamerMessage : public CMessage_MeshStreamerMessage {
 public:
  int numElements;
  int fragmentSize;
  int *destinationPes;
  char *data;

  MeshStreamerMessage(int fragmentSizeInBytes) {    
    numElements = 0; 
    fragmentSize = fragmentSizeInBytes;
  }

  int addData(void *payload, int destinationPe) {
    memcpy(&data[numElements * fragmentSize], payload, fragmentSize);
    destinationPes[numElements] = destinationPe;
    return ++numElements; 
  }

  void *getFragment(int index) {
    return (void *) (data[index * fragmentSize]);  
  }
};

class LocalMessage : public CMessage_LocalMessage {
 public:
  int numElements; 
  int fragmentSize; 
  char *data; 

  LocalMessage(int fragmentSizeInBytes) {
    numElements = 0; 
    fragmentSize = fragmentSizeInBytes; 
  }

  int addData(void *payload) {
    memcpy(&data[numElements * fragmentSize], payload, fragmentSize);
    return ++numElements; 
  }  

};

class MeshStreamer : public CBase_MeshStreamer {

 private:
  int payloadSize_;
  int bucketSize_; 
  int flushPeriod_; 

  int numNodes_; 
  int numRows_; 
  int numColumns_; 
  int numPlanes_; 
  int planeSize_;
  int numPesPerNode_;

  CProxy_MeshStreamerClient clientProxy_;

  int myNodeIndex_;
  int myPlaneIndex_;
  int myColumnIndex_; 
  int myRowIndex_; 

  MeshStreamerMessage **personalizedBuffers_; 
  MeshStreamerMessage **columnBuffers_; 
  MeshStreamerMessage **planeBuffers_; 

  void determineLocation(const int destinationPe, int &row, int &column, 
			 int &plane, MeshStreamerMessageType &msgType);

  void storeMessage(MeshStreamerMessage **messageBuffers, 
		    const int bucketIndex, const int destinationPe, 
		    const int rowIndex, const int columnIndex, 
		    const int planeIndex,
		    const MeshStreamerMessageType msgType, void *data);
 public:

  MeshStreamer(int payloadSize, int bucketSize, int numRows, 
	       int numColumns, int numPlanes, int numNodes, 
	       CProxy_MeshStreamerClient clientProxy, int numPesPerNode,
	       int flushPeriodInMs);

  ~MeshStreamer();

  void insertData(void *data, int destinationPe); 

  void receiveAggregateData(MeshStreamerMessage *msg);
  void receivePersonalizedData(MeshStreamerMessage *msg);
  void flush();

  void registerPeriodicFlush();

};

class MeshStreamerClient : public CBase_MeshStreamerClient {

  MeshStreamerClient();
  void receive(LocalMessage *msg);

};

#endif