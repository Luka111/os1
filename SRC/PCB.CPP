//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   pcb.cpp
//===================================================

#include "..\INCLUDE\PCB.H"
#include "..\INCLUDE\SCHEDULE.H"
#include "..\INCLUDE\KERNEL.H"

PCB::PCB(Thread *thr, StackSize stackSize , Time timeSlice, void* threadwrapper):
  state(NEW),
  m_Thread(thr),
  stack(0),
  sp(0),
  ss(0),
  m_timeQuantsToRun(timeSlice),
  m_timeQuantsRan(0),
  m_Waiting(){
    Kernel::createStack(this,stackSize,threadwrapper);
    Kernel::AllThreadPCBs.addToBack(this);
  }

PCB::PCB(StackSize stackSize, void* functionpointer):
  state(SYSTEM),
  m_Thread(0),
  stack(0),
  sp(0),
  ss(0),
  m_timeQuantsToRun(0),
  m_timeQuantsRan(0),
  m_Waiting(){
    Kernel::createSystemStack(this,stackSize,functionpointer);
  }

PCB::~PCB(){
  Kernel::AllThreadPCBs.remove(this);
  delete stack;
}

PCB* PCB::createSystemPCB(StackSize stackSize, void* systemCallFunctionPointer){
  return new PCB(stackSize,systemCallFunctionPointer);
}

void PCB::wait(){
  if(state==PCB::TERMINATED){
    signal();
    return;
  }
  if(this != Kernel::runningPCB){
    asmwait();
  }
}

void PCB::block(){
  lock();
  m_Waiting.addToBack(Kernel::block());
  unlock();
}

void PCB::signal(){
  while (!m_Waiting.isEmpty())
  {
    Kernel::deblock(m_Waiting.getFromFront());
  }
  state = PCB::TERMINATED;
}

void PCB::addToWaitingQueue(PCB* blockingPCB){
  m_Waiting.addToBack(blockingPCB);
}

int PCB::shouldBeDispatched(){
  if (m_timeQuantsToRun != 0){
    m_timeQuantsRan ++;
    if (m_timeQuantsRan>=m_timeQuantsToRun){
      m_timeQuantsRan=0;
      return 1;
    }
  }
  return 0;
}

KernelObj::createPCB(Thread* thread, StackSize sz, Time t, void* threadwrapper){
  PCB* npcb = new PCB(thread,sz,t,threadwrapper);
  return npcb->m_ID;
}
