//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   ivtentry.cpp
//===================================================

#include "..\INCLUDE\IVTENTRY.H"
#include "..\INCLUDE\KERNEL.H"
#include "..\INCLUDE\KERNELEV.H"

static IVTEntry* g_IVTEntryMap[256] = {0};

IVTEntry::IVTEntry(IVTNo ivtNo, InterruptHandler ih):
  m_KernelEventID(KernelEvent::getByIvtNo(ivtNo)),
  m_ivtNo(ivtNo),
  m_OriginalHandler(Kernel::swapHandlers(ivtNo,ih)) {
  g_IVTEntryMap[ivtNo] = this;
}

IVTEntry::~IVTEntry(){
  g_IVTEntryMap[m_ivtNo] = NULL;
  Kernel::swapHandlers(m_ivtNo,m_OriginalHandler);
}

void IVTEntry::execOriginalInterruptHandler(){
  (*m_OriginalHandler)();
}

IVTEntry* IVTEntry::getByIvtNo(IVTNo ivtNo){
  return g_IVTEntryMap[ivtNo];
}
