#ifndef _MESH_STREAMER_H_
#define _MESH_STREAMER_H_

#include "MeshStreamer.decl.h"

enum MeshStreamerMessageType {PlaneMessage, ColumnMessage, PersonalizedMessage};

class LocalMessage : public CMessage_LocalMessage {
public:
    int numDataItems; 
    int dataItemSize; 
    char *data;

    LocalMessage(int dataItemSizeInBytes) {
        numDataItems = 0; 
        dataItemSize = dataItemSizeInBytes; 
    }

    int addDataItem(void *dataItem) {
        CmiMemcpy(&data[numDataItems * dataItemSize], dataItem, dataItemSize);
        return ++numDataItems; 
    } 

    void *getDataItem(int index) {
        return (void *) (&data[index * dataItemSize]);  
    }

};

class MeshStreamerMessage : public CMessage_MeshStreamerMessage {
public:
    int numDataItems;
    int dataItemSize;
    int *destinationPes;
    char *data;

    MeshStreamerMessage(int dataItemSizeInBytes) {    
        numDataItems = 0; 
        dataItemSize = dataItemSizeInBytes;
    }

    int addDataItem(void *dataItem, int destinationPe) {
        CmiMemcpy(&data[numDataItems * dataItemSize], dataItem, dataItemSize);
        destinationPes[numDataItems] = destinationPe;
        return ++numDataItems; 
    }

    void *getDataItem(int index) {
        return (void *) (&data[index * dataItemSize]);  
    }
};

class MeshStreamerClient : public CBase_MeshStreamerClient {
 public:
     MeshStreamerClient();
     virtual void receiveCombinedData(LocalMessage *msg);

};

class MeshStreamer : public CBase_MeshStreamer {

private:
    int dataItemSize_;
    int bucketSize_; 

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

    MeshStreamer(int dataItemSize, int totalBufferCapacity, int numRows, 
        int numColumns, int numPlanes, int numPesPerNode,
        const CProxy_MeshStreamerClient &clientProxy);

    ~MeshStreamer();

    void insertData(CmiUInt8, int); 
    void receiveAggregateData(MeshStreamerMessage *msg);
    void receivePersonalizedData(MeshStreamerMessage *msg);

    void flushBuckets(MeshStreamerMessage **messageBuffers, int);
    void flushDirect();
};

#endif
