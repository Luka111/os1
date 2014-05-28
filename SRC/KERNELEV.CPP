#include "..\INCLUDE\KERNELEV.H"
#include "..\INCLUDE\KERNEL.H"
#include "..\INCLUDE\LOCK.H"
#include "..\INCLUDE\IVTENTRY.H"


#include <iostream.h>

static ID g_KernelEventMap[256] = {0};

KernelEvent::KernelEvent(IVTNo ivtNo):
  KernelObj(),
  m_ivtNo(ivtNo),
  m_ParentPCB(Kernel::runningPCB){
    g_KernelEventMap[m_ivtNo] = m_ID;
    volatile IVTEntry* ivtentry = IVTEntry::getByIvtNo(ivtNo);
    if(ivtentry){
      cout << "installing my id " << m_ID << " on IVTEntry " << (void*) ivtentry << endl;
      ivtentry->m_KernelEventID = m_ID;
    }else{
      cout << "NO IVTEntry on " << (int)ivtNo << " ?" << endl;
    }
}

KernelEvent::~KernelEvent(){
  g_KernelEventMap[m_ivtNo] = 0;
}

void KernelEvent::wait(){
  if(m_ParentPCB==Kernel::runningPCB){
    asmwait();
  }
}

void KernelEvent::signal(){
  Kernel::deblock(m_ParentPCB);
}

ID KernelEvent::getByIvtNo(IVTNo ivtNo){
  return g_KernelEventMap[ivtNo];
}

ID KernelObj::createKernelEvent(IVTNo ivtNo){
  KernelEvent* nke = new KernelEvent(ivtNo);
  cout << "KernelEvent on ivtNo " << (int)ivtNo << endl;
  return nke->m_ID;
}
