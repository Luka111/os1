#include "..\INCLUDE\KERNELEV.H"
#include "..\INCLUDE\KERNEL.H"
#include "..\INCLUDE\LOCK.H"

static KernelEvent* g_KernelEventMap[256] = {0};

KernelEvent::KernelEvent(IVTNo ivtNo):
  KernelObj(),
  m_ivtNo(ivtNo),
  m_ParentPCB(Kernel::runningPCB){
    g_KernelEventMap[m_ivtNo] = this;
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

KernelEvent* KernelEvent::getByIvtNo(IVTNo ivtNo){
  return g_KernelEventMap[ivtNo];
}

ID KernelObj::createKernelEvent(IVTNo ivtNo){
  return putObj(new KernelEvent(ivtNo));
}
