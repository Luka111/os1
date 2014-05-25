//Thread.cpp
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
  unsigned helpSeg,helpOff;
  asm{
    mov helpSeg, es
    mov helpOff, bx
    mov bx,0
  }
  unsigned bla;
  asm {
    push ax
    mov ax, [bp+0]
    mov bla, ax
    mov ax, [bp+1]
    mov bla, ax
    mov ax, [bp+2]
    mov bla, ax
    mov ax, [bp+3]
    mov bla, ax
    mov ax, [bp+4]
    mov bla, ax
    mov ax, [bp+5]
    mov bla, ax
    mov ax, [bp+6]
    mov bla, ax
    mov ax, [bp+7]
    mov bla, ax
    pop ax
  }
  running = (Thread*)MK_FP(helpSeg,helpOff);
  running->run();
  lock();
  KernelObj::get(running->m_PCBId)->signal();
  Kernel::dispatch();
  unlock();
}

void dispatch(){
  Kernel::dispatch();
}
