#define MAX_TOTAL_PENDING_UPDATES 1024
#define LOCAL_BUFFER_SIZE MAX_TOTAL_PENDING_UPDATES

class DUMMYMSG : public CMessage_DUMMYMSG {
};

class Main : public CBase_Main {
public: 
    Main(CkArgMsg*);
    void verifyDone(CkReductionMsg *msg);
    void allUpdatesDone(DUMMYMSG *msg);
    void start(CkReductionMsg *msg);
private:
    CkChareID mainhandle;
    double starttime;
    int phase;    
};


class Updater : public CBase_Updater {
    u64Int *HPCC_Table;
public:
    Updater();
    Updater(CkMigrateMessage* m) {}
    void generateUpdates();
    void initialize();
    void updateLocalTable(u64Int); 
    void checkErrors();
};

