//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   thread.cpp
//===================================================

#include "..\INCLUDE\THREAD.H"
#include "..\INCLUDE\PCB.H"
#include "..\INCLUDE\KERNEL.H"
#include "..\INCLUDE\LOCK.H"
#include "..\INCLUDE\SCHEDULE.H"
#include "..\INCLUDE\SEGMENT.H"

Thread::Thread(StackSize stackSize, Time timeSlice):
m_PCBId(KernelObj::createPCB(this,stackSize,timeSlice,(void*)(&(Thread::wrapper)))){
}

Thread::~Thread(){
  waitToComplete();
  KernelObj::destroy(m_PCBId);
}

void Thread::start(){
  lock();
  PCB* myPCB = (PCB*)KernelObj::get(m_PCBId);
  if(myPCB->state == PCB::NEW){
    myPCB->state = PCB::READY;
    Scheduler::put(myPCB);
  }
  unlock();
}

void Thread::waitToComplete(){
  lock();
  KernelObj* myPCB = KernelObj::get(m_PCBId);
  myPCB->wait();
  unlock();
}

void Thread::wrapper(Thread *running){
  running->run();
  lock();
  KernelObj::get(running->m_PCBId)->signal();
  Kernel::dispatch();
  unlock();
}

void dispatch(){
  Kernel::dispatch();
}
