#ifndef _DECL_RandomAccess_H_
#define _DECL_RandomAccess_H_
#include "charm++.h"
/* DECLS: readonly CProxy_Main main;
 */

/* DECLS: readonly CProxy_DataTable table_array;
 */

/* DECLS: readonly CProxy_Updater updater_array;
 */

/* DECLS: readonly int num_table_entries;
 */

/* DECLS: readonly int num_table_chunks;
 */

/* DECLS: readonly int num_updaters;
 */

/* DECLS: readonly int chunk_size;
 */

/* DECLS: mainchare Main: Chare{
Main(CkArgMsg* impl_msg);
void done(void);
void collectVerification(int wrong_entries);
};
 */
 class Main;
 class CkIndex_Main;
 class CProxy_Main;
/* --------------- index object ------------------ */
class CkIndex_Main:public CProxy_Chare{
  public:
    typedef Main local_t;
    typedef CkIndex_Main index_t;
    typedef CProxy_Main proxy_t;
    typedef CProxy_Main element_t;

    static int __idx;
    static void __register(const char *s, size_t size);
/* DECLS: Main(CkArgMsg* impl_msg);
 */
    static int __idx_Main_CkArgMsg;
    static int ckNew(CkArgMsg* impl_msg) { return __idx_Main_CkArgMsg; }
    static void _call_Main_CkArgMsg(void* impl_msg,Main* impl_obj);

/* DECLS: void done(void);
 */
    static int __idx_done_void;
    static int done(void) { return __idx_done_void; }
    static void _call_done_void(void* impl_msg,Main* impl_obj);

/* DECLS: void collectVerification(int wrong_entries);
 */
    static int __idx_collectVerification_marshall3;
    static int collectVerification(int wrong_entries) { return __idx_collectVerification_marshall3; }
    static void _call_collectVerification_marshall3(void* impl_msg,Main* impl_obj);
    static int _callmarshall_collectVerification_marshall3(char* impl_buf,Main* impl_obj);
    static void _marshallmessagepup_collectVerification_marshall3(PUP::er &p,void *msg);

};
/* --------------- element proxy ------------------ */
class CProxy_Main:public CProxy_Chare{
  public:
    typedef Main local_t;
    typedef CkIndex_Main index_t;
    typedef CProxy_Main proxy_t;
    typedef CProxy_Main element_t;

    CProxy_Main(void) {};
    CProxy_Main(CkChareID __cid) : CProxy_Chare(__cid){  }
    CProxy_Main(const Chare *c) : CProxy_Chare(c){  }
int ckIsDelegated(void) const {return CProxy_Chare::ckIsDelegated();}
inline CkDelegateMgr *ckDelegatedTo(void) const {return CProxy_Chare::ckDelegatedTo();}
inline CkDelegateData *ckDelegatedPtr(void) const {return CProxy_Chare::ckDelegatedPtr();}
CkGroupID ckDelegatedIdx(void) const {return CProxy_Chare::ckDelegatedIdx();}
inline void ckCheck(void) const {CProxy_Chare::ckCheck();}
const CkChareID &ckGetChareID(void) const
{ return CProxy_Chare::ckGetChareID(); }
operator const CkChareID &(void) const {return ckGetChareID();}
    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL) {
      CProxy_Chare::ckDelegate(dTo,dPtr);
    }
    void ckUndelegate(void) {
      CProxy_Chare::ckUndelegate();
    }
    void pup(PUP::er &p) {
      CProxy_Chare::pup(p);
    }
    void ckSetChareID(const CkChareID &c) {
      CProxy_Chare::ckSetChareID(c);
    }
    Main *ckLocal(void) const
     { return (Main *)CkLocalChare(&ckGetChareID()); }
/* DECLS: Main(CkArgMsg* impl_msg);
 */
    static CkChareID ckNew(CkArgMsg* impl_msg, int onPE=CK_PE_ANY);
    static void ckNew(CkArgMsg* impl_msg, CkChareID* pcid, int onPE=CK_PE_ANY);
    CProxy_Main(CkArgMsg* impl_msg, int onPE=CK_PE_ANY);

/* DECLS: void done(void);
 */
    void done(void);

/* DECLS: void collectVerification(int wrong_entries);
 */
    void collectVerification(int wrong_entries, const CkEntryOptions *impl_e_opts=NULL);

};
PUPmarshall(CProxy_Main)
typedef CBaseT<Chare, CProxy_Main> CBase_Main;

/* DECLS: array DataTable: ArrayElement{
DataTable(CkMigrateMessage* impl_msg);
DataTable(int num_entries);
void doUpdates(const uint64_t *updates, int num_updates);
void verify(void);
};
 */
 class DataTable;
 class CkIndex_DataTable;
 class CProxy_DataTable;
 class CProxyElement_DataTable;
 class CProxySection_DataTable;
/* --------------- index object ------------------ */
class CkIndex_DataTable:public CProxyElement_ArrayElement{
  public:
    typedef DataTable local_t;
    typedef CkIndex_DataTable index_t;
    typedef CProxy_DataTable proxy_t;
    typedef CProxyElement_DataTable element_t;
    typedef CProxySection_DataTable section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
/* DECLS: DataTable(CkMigrateMessage* impl_msg);
 */
    static int __idx_DataTable_CkMigrateMessage;
    static int ckNew(CkMigrateMessage* impl_msg) { return __idx_DataTable_CkMigrateMessage; }
    static void _call_DataTable_CkMigrateMessage(void* impl_msg,DataTable* impl_obj);

/* DECLS: DataTable(int num_entries);
 */
    static int __idx_DataTable_marshall1;
    static int ckNew(int num_entries) { return __idx_DataTable_marshall1; }
    static void _call_DataTable_marshall1(void* impl_msg,DataTable* impl_obj);
    static int _callmarshall_DataTable_marshall1(char* impl_buf,DataTable* impl_obj);
    static void _marshallmessagepup_DataTable_marshall1(PUP::er &p,void *msg);

/* DECLS: void doUpdates(const uint64_t *updates, int num_updates);
 */
    static int __idx_doUpdates_marshall2;
    static int doUpdates(const uint64_t *updates, int num_updates) { return __idx_doUpdates_marshall2; }
    static void _call_doUpdates_marshall2(void* impl_msg,DataTable* impl_obj);
    static int _callmarshall_doUpdates_marshall2(char* impl_buf,DataTable* impl_obj);
    static void _marshallmessagepup_doUpdates_marshall2(PUP::er &p,void *msg);

/* DECLS: void verify(void);
 */
    static int __idx_verify_void;
    static int verify(void) { return __idx_verify_void; }
    static void _call_verify_void(void* impl_msg,DataTable* impl_obj);

};
/* --------------- element proxy ------------------ */
 class CProxyElement_DataTable : public CProxyElement_ArrayElement{
  public:
    typedef DataTable local_t;
    typedef CkIndex_DataTable index_t;
    typedef CProxy_DataTable proxy_t;
    typedef CProxyElement_DataTable element_t;
    typedef CProxySection_DataTable section_t;

    CProxyElement_DataTable(void) {}
    CProxyElement_DataTable(const ArrayElement *e) : CProxyElement_ArrayElement(e){  }
    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL) {
      CProxyElement_ArrayElement::ckDelegate(dTo,dPtr);
    }
    void ckUndelegate(void) {
      CProxyElement_ArrayElement::ckUndelegate();
    }
    void pup(PUP::er &p) {
      CProxyElement_ArrayElement::pup(p);
    }
int ckIsDelegated(void) const {return CProxyElement_ArrayElement::ckIsDelegated();}
inline CkDelegateMgr *ckDelegatedTo(void) const {return CProxyElement_ArrayElement::ckDelegatedTo();}
inline CkDelegateData *ckDelegatedPtr(void) const {return CProxyElement_ArrayElement::ckDelegatedPtr();}
CkGroupID ckDelegatedIdx(void) const {return CProxyElement_ArrayElement::ckDelegatedIdx();}
inline void ckCheck(void) const {CProxyElement_ArrayElement::ckCheck();}
inline operator CkArrayID () const {return ckGetArrayID();}
inline static CkArrayID ckCreateEmptyArray(void){ return CProxyElement_ArrayElement::ckCreateEmptyArray(); }
inline static CkArrayID ckCreateArray(CkArrayMessage *m,int ctor,const CkArrayOptions &opts){ return CProxyElement_ArrayElement::ckCreateArray(m,ctor,opts); }
inline void ckInsertIdx(CkArrayMessage *m,int ctor,int onPe,const CkArrayIndex &idx){ CProxyElement_ArrayElement::ckInsertIdx(m,ctor,onPe,idx); }
inline void ckBroadcast(CkArrayMessage *m, int ep, int opts=0) const{ CProxyElement_ArrayElement::ckBroadcast(m,ep,opts); }
inline CkArrayID ckGetArrayID(void) const{ return CProxyElement_ArrayElement::ckGetArrayID();}
inline CkArray *ckLocalBranch(void) const{ return CProxyElement_ArrayElement::ckLocalBranch(); }
inline CkLocMgr *ckLocMgr(void) const{ return CProxyElement_ArrayElement::ckLocMgr(); }
inline void doneInserting(void) { CProxyElement_ArrayElement::doneInserting(); }
inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxyElement_ArrayElement::setReductionClient(fn,param); }
inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxyElement_ArrayElement::ckSetReductionClient(fn,param); }
inline void ckSetReductionClient(CkCallback *cb) const
{ CProxyElement_ArrayElement::ckSetReductionClient(cb); }
inline void ckInsert(CkArrayMessage *m,int ctor,int onPe)
  { CProxyElement_ArrayElement::ckInsert(m,ctor,onPe); }
inline void ckSend(CkArrayMessage *m, int ep, int opts = 0) const
  { CProxyElement_ArrayElement::ckSend(m,ep,opts); }
inline void *ckSendSync(CkArrayMessage *m, int ep) const
  { return CProxyElement_ArrayElement::ckSendSync(m,ep); }
inline const CkArrayIndex &ckGetIndex() const
  { return CProxyElement_ArrayElement::ckGetIndex(); }
    DataTable *ckLocal(void) const
      { return (DataTable *)CProxyElement_ArrayElement::ckLocal(); }
    CProxyElement_DataTable(const CkArrayID &aid,const CkArrayIndex1D &idx,CK_DELCTOR_PARAM)
        :CProxyElement_ArrayElement(aid,idx,CK_DELCTOR_ARGS) {}
    CProxyElement_DataTable(const CkArrayID &aid,const CkArrayIndex1D &idx)
        :CProxyElement_ArrayElement(aid,idx) {}
/* DECLS: DataTable(CkMigrateMessage* impl_msg);
 */

/* DECLS: DataTable(int num_entries);
 */
    void insert(int num_entries, int onPE=-1, const CkEntryOptions *impl_e_opts=NULL);
/* DECLS: void doUpdates(const uint64_t *updates, int num_updates);
 */
    void doUpdates(const uint64_t *updates, int num_updates, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void verify(void);
 */
    void verify(void) ;

};
PUPmarshall(CProxyElement_DataTable)
/* ---------------- collective proxy -------------- */
 class CProxy_DataTable : public CProxy_ArrayElement{
  public:
    typedef DataTable local_t;
    typedef CkIndex_DataTable index_t;
    typedef CProxy_DataTable proxy_t;
    typedef CProxyElement_DataTable element_t;
    typedef CProxySection_DataTable section_t;

    CProxy_DataTable(void) {}
    CProxy_DataTable(const ArrayElement *e) : CProxy_ArrayElement(e){  }
    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL) {
      CProxy_ArrayElement::ckDelegate(dTo,dPtr);
    }
    void ckUndelegate(void) {
      CProxy_ArrayElement::ckUndelegate();
    }
    void pup(PUP::er &p) {
      CProxy_ArrayElement::pup(p);
    }
int ckIsDelegated(void) const {return CProxy_ArrayElement::ckIsDelegated();}
inline CkDelegateMgr *ckDelegatedTo(void) const {return CProxy_ArrayElement::ckDelegatedTo();}
inline CkDelegateData *ckDelegatedPtr(void) const {return CProxy_ArrayElement::ckDelegatedPtr();}
CkGroupID ckDelegatedIdx(void) const {return CProxy_ArrayElement::ckDelegatedIdx();}
inline void ckCheck(void) const {CProxy_ArrayElement::ckCheck();}
inline operator CkArrayID () const {return ckGetArrayID();}
inline static CkArrayID ckCreateEmptyArray(void){ return CProxy_ArrayElement::ckCreateEmptyArray(); }
inline static CkArrayID ckCreateArray(CkArrayMessage *m,int ctor,const CkArrayOptions &opts){ return CProxy_ArrayElement::ckCreateArray(m,ctor,opts); }
inline void ckInsertIdx(CkArrayMessage *m,int ctor,int onPe,const CkArrayIndex &idx){ CProxy_ArrayElement::ckInsertIdx(m,ctor,onPe,idx); }
inline void ckBroadcast(CkArrayMessage *m, int ep, int opts=0) const{ CProxy_ArrayElement::ckBroadcast(m,ep,opts); }
inline CkArrayID ckGetArrayID(void) const{ return CProxy_ArrayElement::ckGetArrayID();}
inline CkArray *ckLocalBranch(void) const{ return CProxy_ArrayElement::ckLocalBranch(); }
inline CkLocMgr *ckLocMgr(void) const{ return CProxy_ArrayElement::ckLocMgr(); }
inline void doneInserting(void) { CProxy_ArrayElement::doneInserting(); }
inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxy_ArrayElement::setReductionClient(fn,param); }
inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxy_ArrayElement::ckSetReductionClient(fn,param); }
inline void ckSetReductionClient(CkCallback *cb) const
{ CProxy_ArrayElement::ckSetReductionClient(cb); }
    static CkArrayID ckNew(void) {return ckCreateEmptyArray();}
//Generalized array indexing:
    CProxyElement_DataTable operator [] (const CkArrayIndex1D &idx) const
        {return CProxyElement_DataTable(ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_DataTable operator() (const CkArrayIndex1D &idx) const
        {return CProxyElement_DataTable(ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_DataTable operator [] (int idx) const 
        {return CProxyElement_DataTable(ckGetArrayID(), CkArrayIndex1D(idx), CK_DELCTOR_CALL);}
    CProxyElement_DataTable operator () (int idx) const 
        {return CProxyElement_DataTable(ckGetArrayID(), CkArrayIndex1D(idx), CK_DELCTOR_CALL);}
    CProxy_DataTable(const CkArrayID &aid,CK_DELCTOR_PARAM) 
        :CProxy_ArrayElement(aid,CK_DELCTOR_ARGS) {}
    CProxy_DataTable(const CkArrayID &aid) 
        :CProxy_ArrayElement(aid) {}
/* DECLS: DataTable(CkMigrateMessage* impl_msg);
 */

/* DECLS: DataTable(int num_entries);
 */
    static CkArrayID ckNew(int num_entries, const CkArrayOptions &opts, const CkEntryOptions *impl_e_opts=NULL);
    static CkArrayID ckNew(int num_entries, const int s1, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void doUpdates(const uint64_t *updates, int num_updates);
 */
    void doUpdates(const uint64_t *updates, int num_updates, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void verify(void);
 */
    void verify(void) ;

};
PUPmarshall(CProxy_DataTable)
/* ---------------- section proxy -------------- */
 class CProxySection_DataTable : public CProxySection_ArrayElement{
  public:
    typedef DataTable local_t;
    typedef CkIndex_DataTable index_t;
    typedef CProxy_DataTable proxy_t;
    typedef CProxyElement_DataTable element_t;
    typedef CProxySection_DataTable section_t;

    CProxySection_DataTable(void) {}
    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL) {
      CProxySection_ArrayElement::ckDelegate(dTo,dPtr);
    }
    void ckUndelegate(void) {
      CProxySection_ArrayElement::ckUndelegate();
    }
    void pup(PUP::er &p) {
      CProxySection_ArrayElement::pup(p);
    }
int ckIsDelegated(void) const {return CProxySection_ArrayElement::ckIsDelegated();}
inline CkDelegateMgr *ckDelegatedTo(void) const {return CProxySection_ArrayElement::ckDelegatedTo();}
inline CkDelegateData *ckDelegatedPtr(void) const {return CProxySection_ArrayElement::ckDelegatedPtr();}
CkGroupID ckDelegatedIdx(void) const {return CProxySection_ArrayElement::ckDelegatedIdx();}
inline void ckCheck(void) const {CProxySection_ArrayElement::ckCheck();}
inline operator CkArrayID () const {return ckGetArrayID();}
inline static CkArrayID ckCreateEmptyArray(void){ return CProxySection_ArrayElement::ckCreateEmptyArray(); }
inline static CkArrayID ckCreateArray(CkArrayMessage *m,int ctor,const CkArrayOptions &opts){ return CProxySection_ArrayElement::ckCreateArray(m,ctor,opts); }
inline void ckInsertIdx(CkArrayMessage *m,int ctor,int onPe,const CkArrayIndex &idx){ CProxySection_ArrayElement::ckInsertIdx(m,ctor,onPe,idx); }
inline void ckBroadcast(CkArrayMessage *m, int ep, int opts=0) const{ CProxySection_ArrayElement::ckBroadcast(m,ep,opts); }
inline CkArrayID ckGetArrayID(void) const{ return CProxySection_ArrayElement::ckGetArrayID();}
inline CkArray *ckLocalBranch(void) const{ return CProxySection_ArrayElement::ckLocalBranch(); }
inline CkLocMgr *ckLocMgr(void) const{ return CProxySection_ArrayElement::ckLocMgr(); }
inline void doneInserting(void) { CProxySection_ArrayElement::doneInserting(); }
inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxySection_ArrayElement::setReductionClient(fn,param); }
inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxySection_ArrayElement::ckSetReductionClient(fn,param); }
inline void ckSetReductionClient(CkCallback *cb) const
{ CProxySection_ArrayElement::ckSetReductionClient(cb); }
inline void ckSend(CkArrayMessage *m, int ep, int opts = 0)
 { CProxySection_ArrayElement::ckSend(m,ep,opts); }
inline CkSectionInfo &ckGetSectionInfo()
  { return CProxySection_ArrayElement::ckGetSectionInfo(); }
inline CkSectionID *ckGetSectionIDs()
  { return CProxySection_ArrayElement::ckGetSectionIDs(); }
inline CkSectionID &ckGetSectionID()
  { return CProxySection_ArrayElement::ckGetSectionID(); }
inline CkSectionID &ckGetSectionID(int i)
  { return CProxySection_ArrayElement::ckGetSectionID(i); }
inline CkArrayID ckGetArrayIDn(int i) const
{return CProxySection_ArrayElement::ckGetArrayIDn(i); } 
inline CkArrayIndexMax *ckGetArrayElements() const
  { return CProxySection_ArrayElement::ckGetArrayElements(); }
inline CkArrayIndexMax *ckGetArrayElements(int i) const
{return CProxySection_ArrayElement::ckGetArrayElements(i); }
inline int ckGetNumElements() const
  { return CProxySection_ArrayElement::ckGetNumElements(); } 
inline int ckGetNumElements(int i) const
{return CProxySection_ArrayElement::ckGetNumElements(i); } 
//Generalized array indexing:
    CProxyElement_DataTable operator [] (const CkArrayIndex1D &idx) const
        {return CProxyElement_DataTable(ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_DataTable operator() (const CkArrayIndex1D &idx) const
        {return CProxyElement_DataTable(ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_DataTable operator [] (int idx) const 
        {return CProxyElement_DataTable(ckGetArrayID(), *(CkArrayIndex1D*)&ckGetArrayElements()[idx], CK_DELCTOR_CALL);}
    CProxyElement_DataTable operator () (int idx) const 
        {return CProxyElement_DataTable(ckGetArrayID(), *(CkArrayIndex1D*)&ckGetArrayElements()[idx], CK_DELCTOR_CALL);}
    static CkSectionID ckNew(const CkArrayID &aid, CkArrayIndex1D *elems, int nElems) {
      return CkSectionID(aid, elems, nElems);
    } 
    static CkSectionID ckNew(const CkArrayID &aid, int l, int u, int s) {
      CkVec<CkArrayIndex1D> al;
      for (int i=l; i<=u; i+=s) al.push_back(CkArrayIndex1D(i));
      return CkSectionID(aid, al.getVec(), al.size());
    } 
    CProxySection_DataTable(const CkArrayID &aid, CkArrayIndexMax *elems, int nElems, CK_DELCTOR_PARAM) 
        :CProxySection_ArrayElement(aid,elems,nElems,CK_DELCTOR_ARGS) {}
    CProxySection_DataTable(const CkArrayID &aid, CkArrayIndexMax *elems, int nElems) 
        :CProxySection_ArrayElement(aid,elems,nElems) {}
    CProxySection_DataTable(const CkSectionID &sid)       :CProxySection_ArrayElement(sid) {}
    CProxySection_DataTable(int n, const CkArrayID *aid, CkArrayIndexMax const * const *elems, const int *nElems, CK_DELCTOR_PARAM) 
        :CProxySection_ArrayElement(n,aid,elems,nElems,CK_DELCTOR_ARGS) {}
    CProxySection_DataTable(int n, const CkArrayID *aid, CkArrayIndexMax const * const *elems, const int *nElems) 
        :CProxySection_ArrayElement(n,aid,elems,nElems) {}
    static CkSectionID ckNew(const CkArrayID &aid, CkArrayIndexMax *elems, int nElems) {
      return CkSectionID(aid, elems, nElems);
    } 
/* DECLS: DataTable(CkMigrateMessage* impl_msg);
 */

/* DECLS: DataTable(int num_entries);
 */

/* DECLS: void doUpdates(const uint64_t *updates, int num_updates);
 */
    void doUpdates(const uint64_t *updates, int num_updates, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void verify(void);
 */
    void verify(void) ;

};
PUPmarshall(CProxySection_DataTable)
typedef CBaseT<ArrayElementT<CkIndex1D>, CProxy_DataTable> CBase_DataTable;

/* DECLS: array Updater: ArrayElement{
Updater(CkMigrateMessage* impl_msg);
Updater(int base_index);
void generateUpdates(int updates);
void updatefromremote(int size, void* data);
};
 */
 class Updater;
 class CkIndex_Updater;
 class CProxy_Updater;
 class CProxyElement_Updater;
 class CProxySection_Updater;
/* --------------- index object ------------------ */
class CkIndex_Updater:public CProxyElement_ArrayElement{
  public:
    typedef Updater local_t;
    typedef CkIndex_Updater index_t;
    typedef CProxy_Updater proxy_t;
    typedef CProxyElement_Updater element_t;
    typedef CProxySection_Updater section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
/* DECLS: Updater(CkMigrateMessage* impl_msg);
 */
    static int __idx_Updater_CkMigrateMessage;
    static int ckNew(CkMigrateMessage* impl_msg) { return __idx_Updater_CkMigrateMessage; }
    static void _call_Updater_CkMigrateMessage(void* impl_msg,Updater* impl_obj);

/* DECLS: Updater(int base_index);
 */
    static int __idx_Updater_marshall1;
    static int ckNew(int base_index) { return __idx_Updater_marshall1; }
    static void _call_Updater_marshall1(void* impl_msg,Updater* impl_obj);
    static int _callmarshall_Updater_marshall1(char* impl_buf,Updater* impl_obj);
    static void _marshallmessagepup_Updater_marshall1(PUP::er &p,void *msg);

/* DECLS: void generateUpdates(int updates);
 */
    static int __idx_generateUpdates_marshall2;
    static int generateUpdates(int updates) { return __idx_generateUpdates_marshall2; }
    static void _call_generateUpdates_marshall2(void* impl_msg,Updater* impl_obj);
    static int _callmarshall_generateUpdates_marshall2(char* impl_buf,Updater* impl_obj);
    static void _marshallmessagepup_generateUpdates_marshall2(PUP::er &p,void *msg);

/* DECLS: void updatefromremote(int size, void* data);
 */
    static int __idx_updatefromremote_marshall3;
    static int updatefromremote(int size, void* data) { return __idx_updatefromremote_marshall3; }
    static void _call_updatefromremote_marshall3(void* impl_msg,Updater* impl_obj);
    static int _callmarshall_updatefromremote_marshall3(char* impl_buf,Updater* impl_obj);
    static void _marshallmessagepup_updatefromremote_marshall3(PUP::er &p,void *msg);

};
/* --------------- element proxy ------------------ */
 class CProxyElement_Updater : public CProxyElement_ArrayElement{
  public:
    typedef Updater local_t;
    typedef CkIndex_Updater index_t;
    typedef CProxy_Updater proxy_t;
    typedef CProxyElement_Updater element_t;
    typedef CProxySection_Updater section_t;

    CProxyElement_Updater(void) {}
    CProxyElement_Updater(const ArrayElement *e) : CProxyElement_ArrayElement(e){  }
    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL) {
      CProxyElement_ArrayElement::ckDelegate(dTo,dPtr);
    }
    void ckUndelegate(void) {
      CProxyElement_ArrayElement::ckUndelegate();
    }
    void pup(PUP::er &p) {
      CProxyElement_ArrayElement::pup(p);
    }
int ckIsDelegated(void) const {return CProxyElement_ArrayElement::ckIsDelegated();}
inline CkDelegateMgr *ckDelegatedTo(void) const {return CProxyElement_ArrayElement::ckDelegatedTo();}
inline CkDelegateData *ckDelegatedPtr(void) const {return CProxyElement_ArrayElement::ckDelegatedPtr();}
CkGroupID ckDelegatedIdx(void) const {return CProxyElement_ArrayElement::ckDelegatedIdx();}
inline void ckCheck(void) const {CProxyElement_ArrayElement::ckCheck();}
inline operator CkArrayID () const {return ckGetArrayID();}
inline static CkArrayID ckCreateEmptyArray(void){ return CProxyElement_ArrayElement::ckCreateEmptyArray(); }
inline static CkArrayID ckCreateArray(CkArrayMessage *m,int ctor,const CkArrayOptions &opts){ return CProxyElement_ArrayElement::ckCreateArray(m,ctor,opts); }
inline void ckInsertIdx(CkArrayMessage *m,int ctor,int onPe,const CkArrayIndex &idx){ CProxyElement_ArrayElement::ckInsertIdx(m,ctor,onPe,idx); }
inline void ckBroadcast(CkArrayMessage *m, int ep, int opts=0) const{ CProxyElement_ArrayElement::ckBroadcast(m,ep,opts); }
inline CkArrayID ckGetArrayID(void) const{ return CProxyElement_ArrayElement::ckGetArrayID();}
inline CkArray *ckLocalBranch(void) const{ return CProxyElement_ArrayElement::ckLocalBranch(); }
inline CkLocMgr *ckLocMgr(void) const{ return CProxyElement_ArrayElement::ckLocMgr(); }
inline void doneInserting(void) { CProxyElement_ArrayElement::doneInserting(); }
inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxyElement_ArrayElement::setReductionClient(fn,param); }
inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxyElement_ArrayElement::ckSetReductionClient(fn,param); }
inline void ckSetReductionClient(CkCallback *cb) const
{ CProxyElement_ArrayElement::ckSetReductionClient(cb); }
inline void ckInsert(CkArrayMessage *m,int ctor,int onPe)
  { CProxyElement_ArrayElement::ckInsert(m,ctor,onPe); }
inline void ckSend(CkArrayMessage *m, int ep, int opts = 0) const
  { CProxyElement_ArrayElement::ckSend(m,ep,opts); }
inline void *ckSendSync(CkArrayMessage *m, int ep) const
  { return CProxyElement_ArrayElement::ckSendSync(m,ep); }
inline const CkArrayIndex &ckGetIndex() const
  { return CProxyElement_ArrayElement::ckGetIndex(); }
    Updater *ckLocal(void) const
      { return (Updater *)CProxyElement_ArrayElement::ckLocal(); }
    CProxyElement_Updater(const CkArrayID &aid,const CkArrayIndex1D &idx,CK_DELCTOR_PARAM)
        :CProxyElement_ArrayElement(aid,idx,CK_DELCTOR_ARGS) {}
    CProxyElement_Updater(const CkArrayID &aid,const CkArrayIndex1D &idx)
        :CProxyElement_ArrayElement(aid,idx) {}
/* DECLS: Updater(CkMigrateMessage* impl_msg);
 */

/* DECLS: Updater(int base_index);
 */
    void insert(int base_index, int onPE=-1, const CkEntryOptions *impl_e_opts=NULL);
/* DECLS: void generateUpdates(int updates);
 */
    void generateUpdates(int updates, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void updatefromremote(int size, void* data);
 */
    void updatefromremote(int size, void* data, const CkEntryOptions *impl_e_opts=NULL) ;

};
PUPmarshall(CProxyElement_Updater)
/* ---------------- collective proxy -------------- */
 class CProxy_Updater : public CProxy_ArrayElement{
  public:
    typedef Updater local_t;
    typedef CkIndex_Updater index_t;
    typedef CProxy_Updater proxy_t;
    typedef CProxyElement_Updater element_t;
    typedef CProxySection_Updater section_t;

    CProxy_Updater(void) {}
    CProxy_Updater(const ArrayElement *e) : CProxy_ArrayElement(e){  }
    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL) {
      CProxy_ArrayElement::ckDelegate(dTo,dPtr);
    }
    void ckUndelegate(void) {
      CProxy_ArrayElement::ckUndelegate();
    }
    void pup(PUP::er &p) {
      CProxy_ArrayElement::pup(p);
    }
int ckIsDelegated(void) const {return CProxy_ArrayElement::ckIsDelegated();}
inline CkDelegateMgr *ckDelegatedTo(void) const {return CProxy_ArrayElement::ckDelegatedTo();}
inline CkDelegateData *ckDelegatedPtr(void) const {return CProxy_ArrayElement::ckDelegatedPtr();}
CkGroupID ckDelegatedIdx(void) const {return CProxy_ArrayElement::ckDelegatedIdx();}
inline void ckCheck(void) const {CProxy_ArrayElement::ckCheck();}
inline operator CkArrayID () const {return ckGetArrayID();}
inline static CkArrayID ckCreateEmptyArray(void){ return CProxy_ArrayElement::ckCreateEmptyArray(); }
inline static CkArrayID ckCreateArray(CkArrayMessage *m,int ctor,const CkArrayOptions &opts){ return CProxy_ArrayElement::ckCreateArray(m,ctor,opts); }
inline void ckInsertIdx(CkArrayMessage *m,int ctor,int onPe,const CkArrayIndex &idx){ CProxy_ArrayElement::ckInsertIdx(m,ctor,onPe,idx); }
inline void ckBroadcast(CkArrayMessage *m, int ep, int opts=0) const{ CProxy_ArrayElement::ckBroadcast(m,ep,opts); }
inline CkArrayID ckGetArrayID(void) const{ return CProxy_ArrayElement::ckGetArrayID();}
inline CkArray *ckLocalBranch(void) const{ return CProxy_ArrayElement::ckLocalBranch(); }
inline CkLocMgr *ckLocMgr(void) const{ return CProxy_ArrayElement::ckLocMgr(); }
inline void doneInserting(void) { CProxy_ArrayElement::doneInserting(); }
inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxy_ArrayElement::setReductionClient(fn,param); }
inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxy_ArrayElement::ckSetReductionClient(fn,param); }
inline void ckSetReductionClient(CkCallback *cb) const
{ CProxy_ArrayElement::ckSetReductionClient(cb); }
    static CkArrayID ckNew(void) {return ckCreateEmptyArray();}
//Generalized array indexing:
    CProxyElement_Updater operator [] (const CkArrayIndex1D &idx) const
        {return CProxyElement_Updater(ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_Updater operator() (const CkArrayIndex1D &idx) const
        {return CProxyElement_Updater(ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_Updater operator [] (int idx) const 
        {return CProxyElement_Updater(ckGetArrayID(), CkArrayIndex1D(idx), CK_DELCTOR_CALL);}
    CProxyElement_Updater operator () (int idx) const 
        {return CProxyElement_Updater(ckGetArrayID(), CkArrayIndex1D(idx), CK_DELCTOR_CALL);}
    CProxy_Updater(const CkArrayID &aid,CK_DELCTOR_PARAM) 
        :CProxy_ArrayElement(aid,CK_DELCTOR_ARGS) {}
    CProxy_Updater(const CkArrayID &aid) 
        :CProxy_ArrayElement(aid) {}
/* DECLS: Updater(CkMigrateMessage* impl_msg);
 */

/* DECLS: Updater(int base_index);
 */
    static CkArrayID ckNew(int base_index, const CkArrayOptions &opts, const CkEntryOptions *impl_e_opts=NULL);
    static CkArrayID ckNew(int base_index, const int s1, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void generateUpdates(int updates);
 */
    void generateUpdates(int updates, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void updatefromremote(int size, void* data);
 */
    void updatefromremote(int size, void* data, const CkEntryOptions *impl_e_opts=NULL) ;

};
PUPmarshall(CProxy_Updater)
/* ---------------- section proxy -------------- */
 class CProxySection_Updater : public CProxySection_ArrayElement{
  public:
    typedef Updater local_t;
    typedef CkIndex_Updater index_t;
    typedef CProxy_Updater proxy_t;
    typedef CProxyElement_Updater element_t;
    typedef CProxySection_Updater section_t;

    CProxySection_Updater(void) {}
    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL) {
      CProxySection_ArrayElement::ckDelegate(dTo,dPtr);
    }
    void ckUndelegate(void) {
      CProxySection_ArrayElement::ckUndelegate();
    }
    void pup(PUP::er &p) {
      CProxySection_ArrayElement::pup(p);
    }
int ckIsDelegated(void) const {return CProxySection_ArrayElement::ckIsDelegated();}
inline CkDelegateMgr *ckDelegatedTo(void) const {return CProxySection_ArrayElement::ckDelegatedTo();}
inline CkDelegateData *ckDelegatedPtr(void) const {return CProxySection_ArrayElement::ckDelegatedPtr();}
CkGroupID ckDelegatedIdx(void) const {return CProxySection_ArrayElement::ckDelegatedIdx();}
inline void ckCheck(void) const {CProxySection_ArrayElement::ckCheck();}
inline operator CkArrayID () const {return ckGetArrayID();}
inline static CkArrayID ckCreateEmptyArray(void){ return CProxySection_ArrayElement::ckCreateEmptyArray(); }
inline static CkArrayID ckCreateArray(CkArrayMessage *m,int ctor,const CkArrayOptions &opts){ return CProxySection_ArrayElement::ckCreateArray(m,ctor,opts); }
inline void ckInsertIdx(CkArrayMessage *m,int ctor,int onPe,const CkArrayIndex &idx){ CProxySection_ArrayElement::ckInsertIdx(m,ctor,onPe,idx); }
inline void ckBroadcast(CkArrayMessage *m, int ep, int opts=0) const{ CProxySection_ArrayElement::ckBroadcast(m,ep,opts); }
inline CkArrayID ckGetArrayID(void) const{ return CProxySection_ArrayElement::ckGetArrayID();}
inline CkArray *ckLocalBranch(void) const{ return CProxySection_ArrayElement::ckLocalBranch(); }
inline CkLocMgr *ckLocMgr(void) const{ return CProxySection_ArrayElement::ckLocMgr(); }
inline void doneInserting(void) { CProxySection_ArrayElement::doneInserting(); }
inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxySection_ArrayElement::setReductionClient(fn,param); }
inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
{ CProxySection_ArrayElement::ckSetReductionClient(fn,param); }
inline void ckSetReductionClient(CkCallback *cb) const
{ CProxySection_ArrayElement::ckSetReductionClient(cb); }
inline void ckSend(CkArrayMessage *m, int ep, int opts = 0)
 { CProxySection_ArrayElement::ckSend(m,ep,opts); }
inline CkSectionInfo &ckGetSectionInfo()
  { return CProxySection_ArrayElement::ckGetSectionInfo(); }
inline CkSectionID *ckGetSectionIDs()
  { return CProxySection_ArrayElement::ckGetSectionIDs(); }
inline CkSectionID &ckGetSectionID()
  { return CProxySection_ArrayElement::ckGetSectionID(); }
inline CkSectionID &ckGetSectionID(int i)
  { return CProxySection_ArrayElement::ckGetSectionID(i); }
inline CkArrayID ckGetArrayIDn(int i) const
{return CProxySection_ArrayElement::ckGetArrayIDn(i); } 
inline CkArrayIndexMax *ckGetArrayElements() const
  { return CProxySection_ArrayElement::ckGetArrayElements(); }
inline CkArrayIndexMax *ckGetArrayElements(int i) const
{return CProxySection_ArrayElement::ckGetArrayElements(i); }
inline int ckGetNumElements() const
  { return CProxySection_ArrayElement::ckGetNumElements(); } 
inline int ckGetNumElements(int i) const
{return CProxySection_ArrayElement::ckGetNumElements(i); } 
//Generalized array indexing:
    CProxyElement_Updater operator [] (const CkArrayIndex1D &idx) const
        {return CProxyElement_Updater(ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_Updater operator() (const CkArrayIndex1D &idx) const
        {return CProxyElement_Updater(ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_Updater operator [] (int idx) const 
        {return CProxyElement_Updater(ckGetArrayID(), *(CkArrayIndex1D*)&ckGetArrayElements()[idx], CK_DELCTOR_CALL);}
    CProxyElement_Updater operator () (int idx) const 
        {return CProxyElement_Updater(ckGetArrayID(), *(CkArrayIndex1D*)&ckGetArrayElements()[idx], CK_DELCTOR_CALL);}
    static CkSectionID ckNew(const CkArrayID &aid, CkArrayIndex1D *elems, int nElems) {
      return CkSectionID(aid, elems, nElems);
    } 
    static CkSectionID ckNew(const CkArrayID &aid, int l, int u, int s) {
      CkVec<CkArrayIndex1D> al;
      for (int i=l; i<=u; i+=s) al.push_back(CkArrayIndex1D(i));
      return CkSectionID(aid, al.getVec(), al.size());
    } 
    CProxySection_Updater(const CkArrayID &aid, CkArrayIndexMax *elems, int nElems, CK_DELCTOR_PARAM) 
        :CProxySection_ArrayElement(aid,elems,nElems,CK_DELCTOR_ARGS) {}
    CProxySection_Updater(const CkArrayID &aid, CkArrayIndexMax *elems, int nElems) 
        :CProxySection_ArrayElement(aid,elems,nElems) {}
    CProxySection_Updater(const CkSectionID &sid)       :CProxySection_ArrayElement(sid) {}
    CProxySection_Updater(int n, const CkArrayID *aid, CkArrayIndexMax const * const *elems, const int *nElems, CK_DELCTOR_PARAM) 
        :CProxySection_ArrayElement(n,aid,elems,nElems,CK_DELCTOR_ARGS) {}
    CProxySection_Updater(int n, const CkArrayID *aid, CkArrayIndexMax const * const *elems, const int *nElems) 
        :CProxySection_ArrayElement(n,aid,elems,nElems) {}
    static CkSectionID ckNew(const CkArrayID &aid, CkArrayIndexMax *elems, int nElems) {
      return CkSectionID(aid, elems, nElems);
    } 
/* DECLS: Updater(CkMigrateMessage* impl_msg);
 */

/* DECLS: Updater(int base_index);
 */

/* DECLS: void generateUpdates(int updates);
 */
    void generateUpdates(int updates, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void updatefromremote(int size, void* data);
 */
    void updatefromremote(int size, void* data, const CkEntryOptions *impl_e_opts=NULL) ;

};
PUPmarshall(CProxySection_Updater)
typedef CBaseT<ArrayElementT<CkIndex1D>, CProxy_Updater> CBase_Updater;

extern void _registerRandomAccess(void);
extern "C" void CkRegisterMainModule(void);
#endif
