//PCB.CPP

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
}

PCB::~PCB(){
  Kernel::AllThreadPCBs.remove(this);
  delete stack;
}

void PCB::wait(){
  asmwait();
}

void PCB::signal(){
  PCB* temp=NULL;
  while (!m_Waiting.isEmpty())
  {
    temp = m_Waiting.getFromFront();
    state = PCB::READY;
    Scheduler::put(temp);
  }
  state = PCB::TERMINATED;
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
  return KernelObj::putObj(new PCB(thread,sz,t,threadwrapper));
}
