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
        void doUpdates(uint64_t* updates, int num_updates);
        void verify();

    private:
        int base_index;
        int num_entries;
        uint64_t* table;
};

class Updater : public CBase_Updater {
    public:
        Updater(int base_index);
        Updater(CkMigrateMessage* m) {}
        void generateUpdates(int updates);
        void updatefromremote(int size, void* data); 
        uint64_t nth_random(int64_t n);

    private:
        int iterations;
        int chunk_size;
        uint64_t ran;

        uint64_t LocalSendBuffer[LOCAL_BUFFER_SIZE];
        uint64_t LocalOffset;
        Bucket_Ptr Buckets;
        int pendingUpdates;
        int maxPendingUpdates;
        int localBufferSize;
        int pe;
        int peUpdates;
        int Updatesnum;

};
