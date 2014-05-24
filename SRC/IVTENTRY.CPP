#include "..\INCLUDE\IVTENTRY.H"
#include "..\INCLUDE\KERNEL.H"
#include "..\INCLUDE\KERNELEV.H"

IVTEntry::IVTEntry(IVTNo ivtNo, InterruptHandler ih):
  m_ivtNo(ivtNo),
  m_OriginalHandler(Kernel::swapHandlers(ivtNo,ih)) {
  
}

void IVTEntry::signal(){
  KernelEvent::getByIvtNo(m_ivtNo)->signal();
}

void IVTEntry::execOriginalInterruptHandler(){
  (*m_OriginalHandler)();
}
