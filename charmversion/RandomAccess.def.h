/* DEFS: readonly CProxy_Main main;
 */
extern CProxy_Main main;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_main(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|main;
}
#endif

/* DEFS: readonly CProxy_DataTable table_array;
 */
extern CProxy_DataTable table_array;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_table_array(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|table_array;
}
#endif

/* DEFS: readonly CProxy_Updater updater_array;
 */
extern CProxy_Updater updater_array;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_updater_array(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|updater_array;
}
#endif

/* DEFS: readonly int num_table_entries;
 */
extern int num_table_entries;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_num_table_entries(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|num_table_entries;
}
#endif

/* DEFS: readonly int num_table_chunks;
 */
extern int num_table_chunks;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_num_table_chunks(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|num_table_chunks;
}
#endif

/* DEFS: readonly int num_updaters;
 */
extern int num_updaters;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_num_updaters(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|num_updaters;
}
#endif

/* DEFS: readonly int chunk_size;
 */
extern int chunk_size;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_chunk_size(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|chunk_size;
}
#endif

/* DEFS: message PassDate{
u64Int data[];
}
;
 */
#ifndef CK_TEMPLATES_ONLY
void *CMessage_PassDate::operator new(size_t s){
  return PassDate::alloc(__idx, s, 0, 0);
}
void *CMessage_PassDate::operator new(size_t s, int* sz){
  return PassDate::alloc(__idx, s, sz, 0);
}
void *CMessage_PassDate::operator new(size_t s, int* sz,const int pb){
  return PassDate::alloc(__idx, s, sz, pb);
}
void *CMessage_PassDate::operator new(size_t s, int sz0) {
  int sizes[1];
  sizes[0] = sz0;
  return PassDate::alloc(__idx, s, sizes, 0);
}
void *CMessage_PassDate::operator new(size_t s, int sz0, const int p) {
  int sizes[1];
  sizes[0] = sz0;
  return PassDate::alloc(__idx, s, sizes, p);
}
void* CMessage_PassDate::alloc(int msgnum, size_t sz, int *sizes, int pb) {
  size_t offsets[2];
  offsets[0] = ALIGN8(sz);
  if(sizes==0)
    offsets[1] = offsets[0];
  else
    offsets[1] = offsets[0] + ALIGN8(sizeof(u64Int)*sizes[0]);
  PassDate *newmsg = (PassDate *) CkAllocMsg(msgnum, offsets[1], pb);
  newmsg->data = (u64Int *) ((char *)newmsg + offsets[0]);
  return (void *) newmsg;
}
void CMessage_PassDate::dealloc(void *p) {
  CkFreeMsg(p);
}
void* CMessage_PassDate::pack(PassDate *msg) {
  msg->data = (u64Int *) ((char *)msg->data - (char *)msg);
  return (void *) msg;
}
PassDate* CMessage_PassDate::unpack(void* buf) {
  PassDate *msg = (PassDate *) buf;
  msg->data = (u64Int *) ((size_t)msg->data + (char *)msg);
  return msg;
}
int CMessage_PassDate::__idx=0;
#endif

/* DEFS: mainchare Main: Chare{
Main(CkArgMsg* impl_msg);
void done(void);
void collectVerification(int wrong_entries);
};
 */
#ifndef CK_TEMPLATES_ONLY
 int CkIndex_Main::__idx=0;
#endif
#ifndef CK_TEMPLATES_ONLY
/* DEFS: Main(CkArgMsg* impl_msg);
 */
CkChareID CProxy_Main::ckNew(CkArgMsg* impl_msg, int impl_onPE)
{
  CkChareID impl_ret;
  CkCreateChare(CkIndex_Main::__idx, CkIndex_Main::__idx_Main_CkArgMsg, impl_msg, &impl_ret, impl_onPE);
  return impl_ret;
}
void CProxy_Main::ckNew(CkArgMsg* impl_msg, CkChareID* pcid, int impl_onPE)
{
  CkCreateChare(CkIndex_Main::__idx, CkIndex_Main::__idx_Main_CkArgMsg, impl_msg, pcid, impl_onPE);
}
  CProxy_Main::CProxy_Main(CkArgMsg* impl_msg, int impl_onPE)
{
  CkChareID impl_ret;
  CkCreateChare(CkIndex_Main::__idx, CkIndex_Main::__idx_Main_CkArgMsg, impl_msg, &impl_ret, impl_onPE);
  ckSetChareID(impl_ret);
}
 int CkIndex_Main::__idx_Main_CkArgMsg=0;
void CkIndex_Main::_call_Main_CkArgMsg(void* impl_msg,Main * impl_obj)
{
  new (impl_obj) Main((CkArgMsg*)impl_msg);
}

/* DEFS: void done(void);
 */
void CProxy_Main::done(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_Main::__idx_done_void, impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_Main::__idx_done_void, impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_Main::__idx_done_void, impl_msg, &ckGetChareID(),0);
}
 int CkIndex_Main::__idx_done_void=0;
void CkIndex_Main::_call_done_void(void* impl_msg,Main * impl_obj)
{
  CkFreeSysMsg(impl_msg);
  impl_obj->done();
}

/* DEFS: void collectVerification(int wrong_entries);
 */
void CProxy_Main::collectVerification(int wrong_entries, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: int wrong_entries
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|wrong_entries;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|wrong_entries;
  }
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_Main::__idx_collectVerification_marshall3, impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_Main::__idx_collectVerification_marshall3, impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_Main::__idx_collectVerification_marshall3, impl_msg, &ckGetChareID(),0);
}
 int CkIndex_Main::__idx_collectVerification_marshall3=0;
void CkIndex_Main::_call_collectVerification_marshall3(void* impl_msg,Main * impl_obj)
{
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int wrong_entries*/
  PUP::fromMem implP(impl_buf);
  int wrong_entries; implP|wrong_entries;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->collectVerification(wrong_entries);
}
int CkIndex_Main::_callmarshall_collectVerification_marshall3(char* impl_buf,Main * impl_obj) {
  /*Unmarshall pup'd fields: int wrong_entries*/
  PUP::fromMem implP(impl_buf);
  int wrong_entries; implP|wrong_entries;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->collectVerification(wrong_entries);
  return implP.size();
}
void CkIndex_Main::_marshallmessagepup_collectVerification_marshall3(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int wrong_entries*/
  PUP::fromMem implP(impl_buf);
  int wrong_entries; implP|wrong_entries;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("wrong_entries");
  implDestP|wrong_entries;
}

#endif /*CK_TEMPLATES_ONLY*/
#ifndef CK_TEMPLATES_ONLY
void CkIndex_Main::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeMainChare);
  CkRegisterBase(__idx, CkIndex_Chare::__idx);
// REG: Main(CkArgMsg* impl_msg);
  __idx_Main_CkArgMsg = CkRegisterEp("Main(CkArgMsg* impl_msg)",
     (CkCallFnPtr)_call_Main_CkArgMsg, CMessage_CkArgMsg::__idx, __idx, 0);
  CkRegisterMainChare(__idx, __idx_Main_CkArgMsg);

// REG: void done(void);
  __idx_done_void = CkRegisterEp("done(void)",
     (CkCallFnPtr)_call_done_void, 0, __idx, 0);

// REG: void collectVerification(int wrong_entries);
  __idx_collectVerification_marshall3 = CkRegisterEp("collectVerification(int wrong_entries)",
     (CkCallFnPtr)_call_collectVerification_marshall3, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(__idx_collectVerification_marshall3,(CkMarshallUnpackFn)_callmarshall_collectVerification_marshall3);
  CkRegisterMessagePupFn(__idx_collectVerification_marshall3,(CkMessagePupFn)_marshallmessagepup_collectVerification_marshall3);

}
#endif

/* DEFS: array DataTable: ArrayElement{
DataTable(CkMigrateMessage* impl_msg);
DataTable(int num_entries);
void doUpdates(const u64Int *updates, int num_updates);
void verify(void);
};
 */
#ifndef CK_TEMPLATES_ONLY
 int CkIndex_DataTable::__idx=0;
#endif
#ifndef CK_TEMPLATES_ONLY
/* DEFS: DataTable(CkMigrateMessage* impl_msg);
 */

/* DEFS: DataTable(int num_entries);
 */
void CProxyElement_DataTable::insert(int num_entries, int onPE, const CkEntryOptions *impl_e_opts)
{ 
  //Marshall: int num_entries
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|num_entries;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|num_entries;
  }
   ckInsert((CkArrayMessage *)impl_msg,CkIndex_DataTable::__idx_DataTable_marshall1,onPE);
}

/* DEFS: void doUpdates(const u64Int *updates, int num_updates);
 */
void CProxyElement_DataTable::doUpdates(const u64Int *updates, int num_updates, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const u64Int *updates, int num_updates
  int impl_off=0;
  int impl_arrstart=0;
  int impl_off_updates, impl_cnt_updates;
  impl_off_updates=impl_off=CK_ALIGN(impl_off,sizeof(u64Int));
  impl_off+=(impl_cnt_updates=sizeof(u64Int)*(num_updates));
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|impl_off_updates;
    implP|impl_cnt_updates;
    implP|num_updates;
    impl_arrstart=CK_ALIGN(implP.size(),16);
    impl_off+=impl_arrstart;
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|impl_off_updates;
    implP|impl_cnt_updates;
    implP|num_updates;
  }
  char *impl_buf=impl_msg->msgBuf+impl_arrstart;
  memcpy(impl_buf+impl_off_updates,updates,impl_cnt_updates);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_DataTable::__idx_doUpdates_marshall2,0);
}

/* DEFS: void verify(void);
 */
void CProxyElement_DataTable::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_DataTable::__idx_verify_void,0);
}

/* DEFS: DataTable(CkMigrateMessage* impl_msg);
 */
 int CkIndex_DataTable::__idx_DataTable_CkMigrateMessage=0;
void CkIndex_DataTable::_call_DataTable_CkMigrateMessage(void* impl_msg,DataTable * impl_obj)
{
  new (impl_obj) DataTable((CkMigrateMessage*)impl_msg);
}

/* DEFS: DataTable(int num_entries);
 */
CkArrayID CProxy_DataTable::ckNew(int num_entries, const CkArrayOptions &opts, const CkEntryOptions *impl_e_opts)
{ 
  //Marshall: int num_entries
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|num_entries;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|num_entries;
  }
   return ckCreateArray((CkArrayMessage *)impl_msg,CkIndex_DataTable::__idx_DataTable_marshall1,opts);
}
CkArrayID CProxy_DataTable::ckNew(int num_entries, const int s1, const CkEntryOptions *impl_e_opts)
{ 
  //Marshall: int num_entries
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|num_entries;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|num_entries;
  }
   return ckCreateArray((CkArrayMessage *)impl_msg,CkIndex_DataTable::__idx_DataTable_marshall1,CkArrayOptions(s1));
}
 int CkIndex_DataTable::__idx_DataTable_marshall1=0;
void CkIndex_DataTable::_call_DataTable_marshall1(void* impl_msg,DataTable * impl_obj)
{
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int num_entries*/
  PUP::fromMem implP(impl_buf);
  int num_entries; implP|num_entries;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) DataTable(num_entries);
}
int CkIndex_DataTable::_callmarshall_DataTable_marshall1(char* impl_buf,DataTable * impl_obj) {
  /*Unmarshall pup'd fields: int num_entries*/
  PUP::fromMem implP(impl_buf);
  int num_entries; implP|num_entries;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) DataTable(num_entries);
  return implP.size();
}
void CkIndex_DataTable::_marshallmessagepup_DataTable_marshall1(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int num_entries*/
  PUP::fromMem implP(impl_buf);
  int num_entries; implP|num_entries;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("num_entries");
  implDestP|num_entries;
}

/* DEFS: void doUpdates(const u64Int *updates, int num_updates);
 */
void CProxy_DataTable::doUpdates(const u64Int *updates, int num_updates, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const u64Int *updates, int num_updates
  int impl_off=0;
  int impl_arrstart=0;
  int impl_off_updates, impl_cnt_updates;
  impl_off_updates=impl_off=CK_ALIGN(impl_off,sizeof(u64Int));
  impl_off+=(impl_cnt_updates=sizeof(u64Int)*(num_updates));
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|impl_off_updates;
    implP|impl_cnt_updates;
    implP|num_updates;
    impl_arrstart=CK_ALIGN(implP.size(),16);
    impl_off+=impl_arrstart;
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|impl_off_updates;
    implP|impl_cnt_updates;
    implP|num_updates;
  }
  char *impl_buf=impl_msg->msgBuf+impl_arrstart;
  memcpy(impl_buf+impl_off_updates,updates,impl_cnt_updates);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_DataTable::__idx_doUpdates_marshall2,0);
}
 int CkIndex_DataTable::__idx_doUpdates_marshall2=0;
void CkIndex_DataTable::_call_doUpdates_marshall2(void* impl_msg,DataTable * impl_obj)
{
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const u64Int *updates, int num_updates*/
  PUP::fromMem implP(impl_buf);
  int impl_off_updates, impl_cnt_updates; 
  implP|impl_off_updates;
  implP|impl_cnt_updates;
  int num_updates; implP|num_updates;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  u64Int *updates=(u64Int *)(impl_buf+impl_off_updates);
  impl_obj->doUpdates(updates, num_updates);
}
int CkIndex_DataTable::_callmarshall_doUpdates_marshall2(char* impl_buf,DataTable * impl_obj) {
  /*Unmarshall pup'd fields: const u64Int *updates, int num_updates*/
  PUP::fromMem implP(impl_buf);
  int impl_off_updates, impl_cnt_updates; 
  implP|impl_off_updates;
  implP|impl_cnt_updates;
  int num_updates; implP|num_updates;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  u64Int *updates=(u64Int *)(impl_buf+impl_off_updates);
  impl_obj->doUpdates(updates, num_updates);
  return implP.size();
}
void CkIndex_DataTable::_marshallmessagepup_doUpdates_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const u64Int *updates, int num_updates*/
  PUP::fromMem implP(impl_buf);
  int impl_off_updates, impl_cnt_updates; 
  implP|impl_off_updates;
  implP|impl_cnt_updates;
  int num_updates; implP|num_updates;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  u64Int *updates=(u64Int *)(impl_buf+impl_off_updates);
  if (implDestP.hasComments()) implDestP.comment("updates");
  implDestP.synchronize(PUP::sync_begin_array);
  { for (int impl_i=0;impl_i*(sizeof(*updates))<impl_cnt_updates;impl_i++) { 
      implDestP.synchronize(PUP::sync_item);
      implDestP|updates[impl_i];
  } } 
  implDestP.synchronize(PUP::sync_end_array);
  if (implDestP.hasComments()) implDestP.comment("num_updates");
  implDestP|num_updates;
}

/* DEFS: void verify(void);
 */
void CProxy_DataTable::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_DataTable::__idx_verify_void,0);
}
 int CkIndex_DataTable::__idx_verify_void=0;
void CkIndex_DataTable::_call_verify_void(void* impl_msg,DataTable * impl_obj)
{
  CkFreeSysMsg(impl_msg);
  impl_obj->verify();
}

/* DEFS: DataTable(CkMigrateMessage* impl_msg);
 */

/* DEFS: DataTable(int num_entries);
 */

/* DEFS: void doUpdates(const u64Int *updates, int num_updates);
 */
void CProxySection_DataTable::doUpdates(const u64Int *updates, int num_updates, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const u64Int *updates, int num_updates
  int impl_off=0;
  int impl_arrstart=0;
  int impl_off_updates, impl_cnt_updates;
  impl_off_updates=impl_off=CK_ALIGN(impl_off,sizeof(u64Int));
  impl_off+=(impl_cnt_updates=sizeof(u64Int)*(num_updates));
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|impl_off_updates;
    implP|impl_cnt_updates;
    implP|num_updates;
    impl_arrstart=CK_ALIGN(implP.size(),16);
    impl_off+=impl_arrstart;
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|impl_off_updates;
    implP|impl_cnt_updates;
    implP|num_updates;
  }
  char *impl_buf=impl_msg->msgBuf+impl_arrstart;
  memcpy(impl_buf+impl_off_updates,updates,impl_cnt_updates);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_DataTable::__idx_doUpdates_marshall2,0);
}

/* DEFS: void verify(void);
 */
void CProxySection_DataTable::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_DataTable::__idx_verify_void,0);
}

#endif /*CK_TEMPLATES_ONLY*/
#ifndef CK_TEMPLATES_ONLY
void CkIndex_DataTable::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeArray);
  CkRegisterBase(__idx, CkIndex_ArrayElement::__idx);
// REG: DataTable(CkMigrateMessage* impl_msg);
  __idx_DataTable_CkMigrateMessage = CkRegisterEp("DataTable(CkMigrateMessage* impl_msg)",
     (CkCallFnPtr)_call_DataTable_CkMigrateMessage, 0, __idx, 0);
  CkRegisterMigCtor(__idx, __idx_DataTable_CkMigrateMessage);

// REG: DataTable(int num_entries);
  __idx_DataTable_marshall1 = CkRegisterEp("DataTable(int num_entries)",
     (CkCallFnPtr)_call_DataTable_marshall1, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(__idx_DataTable_marshall1,(CkMarshallUnpackFn)_callmarshall_DataTable_marshall1);
  CkRegisterMessagePupFn(__idx_DataTable_marshall1,(CkMessagePupFn)_marshallmessagepup_DataTable_marshall1);

// REG: void doUpdates(const u64Int *updates, int num_updates);
  __idx_doUpdates_marshall2 = CkRegisterEp("doUpdates(const u64Int *updates, int num_updates)",
     (CkCallFnPtr)_call_doUpdates_marshall2, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(__idx_doUpdates_marshall2,(CkMarshallUnpackFn)_callmarshall_doUpdates_marshall2);
  CkRegisterMessagePupFn(__idx_doUpdates_marshall2,(CkMessagePupFn)_marshallmessagepup_doUpdates_marshall2);

// REG: void verify(void);
  __idx_verify_void = CkRegisterEp("verify(void)",
     (CkCallFnPtr)_call_verify_void, 0, __idx, 0);

}
#endif

/* DEFS: array Updater: ArrayElement{
Updater(CkMigrateMessage* impl_msg);
Updater(int base_index);
void generateUpdates(int updates);
void updatefromremote(PassDate* impl_msg);
};
 */
#ifndef CK_TEMPLATES_ONLY
 int CkIndex_Updater::__idx=0;
#endif
#ifndef CK_TEMPLATES_ONLY
/* DEFS: Updater(CkMigrateMessage* impl_msg);
 */

/* DEFS: Updater(int base_index);
 */
void CProxyElement_Updater::insert(int base_index, int onPE, const CkEntryOptions *impl_e_opts)
{ 
  //Marshall: int base_index
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|base_index;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|base_index;
  }
   ckInsert((CkArrayMessage *)impl_msg,CkIndex_Updater::__idx_Updater_marshall1,onPE);
}

/* DEFS: void generateUpdates(int updates);
 */
void CProxyElement_Updater::generateUpdates(int updates, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: int updates
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|updates;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|updates;
  }
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Updater::__idx_generateUpdates_marshall2,0);
}

/* DEFS: void updatefromremote(PassDate* impl_msg);
 */
void CProxyElement_Updater::updatefromremote(PassDate* impl_msg) 
{
  ckCheck();
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Updater::__idx_updatefromremote_PassDate,0);
}

/* DEFS: Updater(CkMigrateMessage* impl_msg);
 */
 int CkIndex_Updater::__idx_Updater_CkMigrateMessage=0;
void CkIndex_Updater::_call_Updater_CkMigrateMessage(void* impl_msg,Updater * impl_obj)
{
  new (impl_obj) Updater((CkMigrateMessage*)impl_msg);
}

/* DEFS: Updater(int base_index);
 */
CkArrayID CProxy_Updater::ckNew(int base_index, const CkArrayOptions &opts, const CkEntryOptions *impl_e_opts)
{ 
  //Marshall: int base_index
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|base_index;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|base_index;
  }
   return ckCreateArray((CkArrayMessage *)impl_msg,CkIndex_Updater::__idx_Updater_marshall1,opts);
}
CkArrayID CProxy_Updater::ckNew(int base_index, const int s1, const CkEntryOptions *impl_e_opts)
{ 
  //Marshall: int base_index
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|base_index;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|base_index;
  }
   return ckCreateArray((CkArrayMessage *)impl_msg,CkIndex_Updater::__idx_Updater_marshall1,CkArrayOptions(s1));
}
 int CkIndex_Updater::__idx_Updater_marshall1=0;
void CkIndex_Updater::_call_Updater_marshall1(void* impl_msg,Updater * impl_obj)
{
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int base_index*/
  PUP::fromMem implP(impl_buf);
  int base_index; implP|base_index;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) Updater(base_index);
}
int CkIndex_Updater::_callmarshall_Updater_marshall1(char* impl_buf,Updater * impl_obj) {
  /*Unmarshall pup'd fields: int base_index*/
  PUP::fromMem implP(impl_buf);
  int base_index; implP|base_index;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) Updater(base_index);
  return implP.size();
}
void CkIndex_Updater::_marshallmessagepup_Updater_marshall1(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int base_index*/
  PUP::fromMem implP(impl_buf);
  int base_index; implP|base_index;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("base_index");
  implDestP|base_index;
}

/* DEFS: void generateUpdates(int updates);
 */
void CProxy_Updater::generateUpdates(int updates, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: int updates
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|updates;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|updates;
  }
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_Updater::__idx_generateUpdates_marshall2,0);
}
 int CkIndex_Updater::__idx_generateUpdates_marshall2=0;
void CkIndex_Updater::_call_generateUpdates_marshall2(void* impl_msg,Updater * impl_obj)
{
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int updates*/
  PUP::fromMem implP(impl_buf);
  int updates; implP|updates;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->generateUpdates(updates);
}
int CkIndex_Updater::_callmarshall_generateUpdates_marshall2(char* impl_buf,Updater * impl_obj) {
  /*Unmarshall pup'd fields: int updates*/
  PUP::fromMem implP(impl_buf);
  int updates; implP|updates;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->generateUpdates(updates);
  return implP.size();
}
void CkIndex_Updater::_marshallmessagepup_generateUpdates_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int updates*/
  PUP::fromMem implP(impl_buf);
  int updates; implP|updates;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("updates");
  implDestP|updates;
}

/* DEFS: void updatefromremote(PassDate* impl_msg);
 */
void CProxy_Updater::updatefromremote(PassDate* impl_msg) 
{
  ckCheck();
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_Updater::__idx_updatefromremote_PassDate,0);
}
 int CkIndex_Updater::__idx_updatefromremote_PassDate=0;
void CkIndex_Updater::_call_updatefromremote_PassDate(void* impl_msg,Updater * impl_obj)
{
  impl_obj->updatefromremote((PassDate*)impl_msg);
}

/* DEFS: Updater(CkMigrateMessage* impl_msg);
 */

/* DEFS: Updater(int base_index);
 */

/* DEFS: void generateUpdates(int updates);
 */
void CProxySection_Updater::generateUpdates(int updates, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: int updates
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|updates;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|updates;
  }
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Updater::__idx_generateUpdates_marshall2,0);
}

/* DEFS: void updatefromremote(PassDate* impl_msg);
 */
void CProxySection_Updater::updatefromremote(PassDate* impl_msg) 
{
  ckCheck();
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Updater::__idx_updatefromremote_PassDate,0);
}

#endif /*CK_TEMPLATES_ONLY*/
#ifndef CK_TEMPLATES_ONLY
void CkIndex_Updater::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeArray);
  CkRegisterBase(__idx, CkIndex_ArrayElement::__idx);
// REG: Updater(CkMigrateMessage* impl_msg);
  __idx_Updater_CkMigrateMessage = CkRegisterEp("Updater(CkMigrateMessage* impl_msg)",
     (CkCallFnPtr)_call_Updater_CkMigrateMessage, 0, __idx, 0);
  CkRegisterMigCtor(__idx, __idx_Updater_CkMigrateMessage);

// REG: Updater(int base_index);
  __idx_Updater_marshall1 = CkRegisterEp("Updater(int base_index)",
     (CkCallFnPtr)_call_Updater_marshall1, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(__idx_Updater_marshall1,(CkMarshallUnpackFn)_callmarshall_Updater_marshall1);
  CkRegisterMessagePupFn(__idx_Updater_marshall1,(CkMessagePupFn)_marshallmessagepup_Updater_marshall1);

// REG: void generateUpdates(int updates);
  __idx_generateUpdates_marshall2 = CkRegisterEp("generateUpdates(int updates)",
     (CkCallFnPtr)_call_generateUpdates_marshall2, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(__idx_generateUpdates_marshall2,(CkMarshallUnpackFn)_callmarshall_generateUpdates_marshall2);
  CkRegisterMessagePupFn(__idx_generateUpdates_marshall2,(CkMessagePupFn)_marshallmessagepup_generateUpdates_marshall2);

// REG: void updatefromremote(PassDate* impl_msg);
  __idx_updatefromremote_PassDate = CkRegisterEp("updatefromremote(PassDate* impl_msg)",
     (CkCallFnPtr)_call_updatefromremote_PassDate, CMessage_PassDate::__idx, __idx, 0);

}
#endif

#ifndef CK_TEMPLATES_ONLY
void _registerRandomAccess(void)
{
  static int _done = 0; if(_done) return; _done = 1;
  CkRegisterReadonly("main","CProxy_Main",sizeof(main),(void *) &main,__xlater_roPup_main);

  CkRegisterReadonly("table_array","CProxy_DataTable",sizeof(table_array),(void *) &table_array,__xlater_roPup_table_array);

  CkRegisterReadonly("updater_array","CProxy_Updater",sizeof(updater_array),(void *) &updater_array,__xlater_roPup_updater_array);

  CkRegisterReadonly("num_table_entries","int",sizeof(num_table_entries),(void *) &num_table_entries,__xlater_roPup_num_table_entries);

  CkRegisterReadonly("num_table_chunks","int",sizeof(num_table_chunks),(void *) &num_table_chunks,__xlater_roPup_num_table_chunks);

  CkRegisterReadonly("num_updaters","int",sizeof(num_updaters),(void *) &num_updaters,__xlater_roPup_num_updaters);

  CkRegisterReadonly("chunk_size","int",sizeof(chunk_size),(void *) &chunk_size,__xlater_roPup_chunk_size);

/* REG: message PassDate{
u64Int data[];
}
;
*/
CMessage_PassDate::__register("PassDate", sizeof(PassDate),(CkPackFnPtr) PassDate::pack,(CkUnpackFnPtr) PassDate::unpack);

/* REG: mainchare Main: Chare{
Main(CkArgMsg* impl_msg);
void done(void);
void collectVerification(int wrong_entries);
};
*/
  CkIndex_Main::__register("Main", sizeof(Main));

/* REG: array DataTable: ArrayElement{
DataTable(CkMigrateMessage* impl_msg);
DataTable(int num_entries);
void doUpdates(const u64Int *updates, int num_updates);
void verify(void);
};
*/
  CkIndex_DataTable::__register("DataTable", sizeof(DataTable));

/* REG: array Updater: ArrayElement{
Updater(CkMigrateMessage* impl_msg);
Updater(int base_index);
void generateUpdates(int updates);
void updatefromremote(PassDate* impl_msg);
};
*/
  CkIndex_Updater::__register("Updater", sizeof(Updater));

}
extern "C" void CkRegisterMainModule(void) {
  _registerRandomAccess();
}
#endif
