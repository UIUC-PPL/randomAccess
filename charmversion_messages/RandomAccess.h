#define MAX_TOTAL_PENDING_UPDATES 1024
#define LOCAL_BUFFER_SIZE MAX_TOTAL_PENDING_UPDATES

extern "C" u64Int nth_random(int64_t n);
class DUMMYMSG : public CMessage_DUMMYMSG {
public:
};

class Main : public CBase_Main {
    public: 
        Main(CkArgMsg*);
        void collectVerification(int errors);
        void Quiescence1(DUMMYMSG *msg);
        void SingleUpdate();
    private:
        int verification_checkins;
        CkChareID mainhandle;

        double starttime;
        double startverifytime;

        int verifydonenum;
        u64Int GlbNumErrors;
    
        int whichQuiescence;

};

class PassData : public CMessage_PassData {
public:
    int size;
    int src;
    u64Int* data;

    PassData(int s, int src1)
    {
        size = s;
        src = src1;
    }
    void fillData(u64Int* d)
    {
        int i;
        for(i=0; i<size; i++)
            data[i] = d[i];
    }
    u64Int* getBuffer()
    {
        return data;
    }
};

class Updater : public CBase_Updater {
    public:
        Updater(int base_index);
        Updater(CkMigrateMessage* m) {}
        void generateUpdates();
        void updatefromremote(PassData* m); 
    
        void verify();
        void verifyfromremote(PassData* remotedata);
        void verifysentDone(int src, u64Int num); 
    private:
        int initialize_done;
        int iterations;
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

        int numofdone;
        
        u64Int NumErrors;

        u64Int* shouldReceive;
        u64Int* alreadyReceive;
        int verifydone;
};

