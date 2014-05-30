#include "..\INCLUDE\KRNELSEM.H"
#include "..\INCLUDE\KERNEL.H"

#include <iostream.h>

KernelSemaphore::KernelSemaphore(int val):
  KernelObj(),
  m_Val(val){
}

KernelSemaphore::~KernelSemaphore(){
  //cout << m_BlockedPCBs.size() << "blocked PCBs on KernelSemaphore death" << endl;
  while(!m_BlockedPCBs.isEmpty()){
    signal();
  }
}

void KernelSemaphore::wait(){
  if(--m_Val<0){
    asmwait();
  }
}

void KernelSemaphore::block(){
  //cout << "KernelSemaphore " << m_ID << " blocking" << endl;
  m_BlockedPCBs.addToBack(Kernel::block());
}

void KernelSemaphore::signal(){
  if(m_Val++<0){
    Kernel::deblock((volatile PCB*)m_BlockedPCBs.getFromFront());
  }
}

ID KernelObj::createKernelSemaphore(int val){
  KernelSemaphore* nke = new KernelSemaphore(val);
  return nke->m_ID;
}
