//PCB.CPP

#include "..\INCLUDE\PCB.H"
#include "..\INCLUDE\SCHEDULE.H"
#include "..\INCLUDE\KERNEL.H"


#include <iostream.h>

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
    cout << "PCB " << (void*) this << " created" << endl;
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
  if(this != Kernel::s_UserThreadPCB){
    asmwait();
  }
}

void PCB::block(){
  lock();
  //cout << "PCB ID " << m_ID << " blocking PCB ID " << Kernel::runningPCB->m_ID << endl;
  if(Kernel::runningPCB){
    Kernel::runningPCB->state = PCB::BLOCKED;
    m_Waiting.addToBack((PCB*)Kernel::runningPCB);
  }
  //Kernel::runningPCB = (volatile PCB*)Scheduler::get();
  //cout << "Now " << Kernel::runningPCB->m_ID << " is running with state " << Kernel::runningPCB->state << endl;
  unlock();
}

void PCB::signal(){
  PCB* temp=NULL;
  while (!m_Waiting.isEmpty())
  {
    temp = m_Waiting.getFromFront();
    state = PCB::READY;
    //cout << "Scheduling " << temp->m_ID << endl;
    Scheduler::put(temp);
  }
  state = PCB::TERMINATED;
}

int PCB::shouldBeDispatched(){
  if (m_timeQuantsToRun != 0){
    m_timeQuantsRan ++;
    if (m_timeQuantsRan>=m_timeQuantsToRun){
      //cout << "PCB ID " << m_ID << "should be dispatched because " << m_timeQuantsRan << ">=" << m_timeQuantsToRun << endl;
      m_timeQuantsRan=0;
      return 1;
    }else{
      //cout << "PCB ID " << m_ID << "should NOT be dispatched yet because " << m_timeQuantsRan << "<" << m_timeQuantsToRun << endl;
    }
  }else{
    //cout << "PCB ID " << m_ID << " should run forever, no dispatch allowed" << endl;
  }
  return 0;
}

KernelObj::createPCB(Thread* thread, StackSize sz, Time t, void* threadwrapper){
  PCB* npcb = new PCB(thread,sz,t,threadwrapper);
  return npcb->m_ID;
}
