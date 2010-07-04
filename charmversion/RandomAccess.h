#define MAX_TOTAL_PENDING_UPDATES 1024
#define LOCAL_BUFFER_SIZE MAX_TOTAL_PENDING_UPDATES

class Main : public CBase_Main {
    public: 
        Main(CkArgMsg*);
        void done();
        void collectVerification(int wrong_entries);

    private:
        int total_wrong_entries;
        int verification_checkins;
};

class DataTable : public CBase_DataTable {
    public:
        DataTable(int num_entries);
        DataTable(CkMigrateMessage* m) {}
        void doUpdates(u64Int* updates, int num_updates);
        void verify();

    private:
        int base_index;
        int num_entries;
        u64Int* table;
};

class PassData : public CMessage_PassDate {
public:
    int size;
    u64Int* data;

    PassData(int s)
    {
        size = s;
    }
    void fillData(u64Int* d)
    {
        int i;
        for(i=0; i<size; i++)
            data[i] = d[i];
    }
};

class Updater : public CBase_Updater {
    public:
        Updater(int base_index);
        Updater(CkMigrateMessage* m) {}
        void generateUpdates(int updates);
        void updatefromremote(PassData* m); 
        u64Int nth_random(int64_t n);

    private:
        int iterations;
        int chunk_size;
        u64Int ran;

        u64Int LocalSendBuffer[LOCAL_BUFFER_SIZE];
        u64Int LocalOffset;
        Bucket_Ptr Buckets;
        int pendingUpdates;
        int maxPendingUpdates;
        int localBufferSize;
        int pe;
        int peUpdates;
        int Updatesnum;

        u64Int *HPCC_Table;

        int GlobalStartMyProc;
};
