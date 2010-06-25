#include "RandomAccess.decl.h"

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
        uint64_t nth_random(int64_t n);

    private:
        int iterations;
        int chunk_size;
        uint64_t* buffer;
        uint64_t ran;
};
