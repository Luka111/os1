//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   krnelsem.cpp
//===================================================

#include "..\INCLUDE\KRNELSEM.H"
#include "..\INCLUDE\KERNEL.H"

KernelSemaphore::KernelSemaphore(int val):
  KernelObj(),
  m_Val(val){
}

KernelSemaphore::~KernelSemaphore(){
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
