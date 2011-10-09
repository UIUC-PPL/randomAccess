#define MAX_TOTAL_PENDING_UPDATES 1024
#define LOCAL_BUFFER_SIZE MAX_TOTAL_PENDING_UPDATES

class DUMMYMSG : public CMessage_DUMMYMSG {
};

class Main : public CBase_Main {
public: 
    Main(CkArgMsg*);
    void verifyDone(CkReductionMsg *msg);
    void allUpdatesDone(DUMMYMSG *msg);
private:
    CkChareID mainhandle;
    double starttime;
    int phase;    
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
        for(int i=0; i<size; i++)
            data[i] = d[i];
    }
    u64Int* getBuffer()
    {
        return data;
    }
};

class Generator: public CBase_Generator{
public:
    Generator();
    Generator(CkMigrateMessage* m) {}
    void generateUpdates();
};

class Updater : public CBase_Updater {
public:
    Updater();
    Updater(CkMigrateMessage* m) {}
    void updateLocalTable(PassData* m); 
    void checkErrors();
};

