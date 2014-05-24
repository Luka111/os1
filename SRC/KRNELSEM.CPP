#include "..\INCLUDE\KRNELSEM.H"
#include "..\INCLUDE\KERNEL.H"

KernelSemaphore::KernelSemaphore(int val):
  KernelObj(),
  m_Val(val){
}

void KernelSemaphore::wait(){
  if(--m_Val<0){
    asmwait();
  }
}

void KernelSemaphore::block(){
  m_BlockedPCBs.addToBack(Kernel::runningPCB);
  KernelObj::block();
}

void KernelSemaphore::signal(){
  if(m_Val++<0){
    Kernel::deblock(m_BlockedPCBs.getFromFront());
  }
}

ID KernelObj::createKernelSemaphore(int val){
  return KernelObj::putObj(new KernelSemaphore(val));
}
