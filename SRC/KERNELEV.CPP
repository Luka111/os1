//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   kernelev.cpp
//===================================================

#include "..\INCLUDE\KERNELEV.H"
#include "..\INCLUDE\KERNEL.H"
#include "..\INCLUDE\LOCK.H"
#include "..\INCLUDE\IVTENTRY.H"

static ID g_KernelEventMap[256] = {0};

KernelEvent::KernelEvent(IVTNo ivtNo):
  KernelObj(),
  m_ivtNo(ivtNo),
  m_ParentPCB(Kernel::runningPCB){
    g_KernelEventMap[m_ivtNo] = m_ID;
    volatile IVTEntry* ivtentry = IVTEntry::getByIvtNo(ivtNo);
    if(ivtentry){
      ivtentry->m_KernelEventID = m_ID;
    }
}

KernelEvent::~KernelEvent(){
  Kernel::deblock(m_ParentPCB);
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
  return nke->m_ID;
}
