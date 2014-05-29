#include "..\INCLUDE\KERNEL.H"
#include "..\INCLUDE\SEGMENT.H"
#include "..\INCLUDE\LOCK.H"
#include "..\INCLUDE\KERNELEV.H"
#include "..\INCLUDE\SCHEDULE.H"
#include "..\INCLUDE\PCB.H"
#include "..\INCLUDE\THREAD.H"
#include "..\INCLUDE\USRTHRD.H"

#include <iostream.h>

const unsigned int systemPCBStackSize = 4096;

volatile PCB* Kernel::runningPCB = NULL;

static volatile PCB* tempPCB = NULL;

volatile PCB* Kernel::s_UserThreadPCB = NULL;

volatile PCB* Kernel::systemPCB = NULL;

List<PCB> Kernel::AllThreadPCBs;

static InterruptHandler originalTimerRoutine;

static InterruptHandler originalSystemRoutine;

static volatile unsigned sss, ssp, tws, twp, rts, rtp, oldSP, oldSS, kernelSS, kernelSP, systemEventid=0, systemOperation, dispatchActive=0;

void interrupt systemCallTrigerrer(){
}

void Kernel::triggerSystemCall(ID id, unsigned operation){
  while(dispatchActive && runningPCB) cout << "_";
  if(!runningPCB){return;}
  asm{
    mov ax, id
    mov dx, operation
    int 60h
  }
}

static void interrupt systemRoutine(){
  asm{
    mov systemEventid, ax
    mov systemOperation, dx
    mov sss, ss
    mov ssp, sp
  }
  Kernel::runningPCB->ss = sss;
  Kernel::runningPCB->sp = ssp;
  ssp = FP_OFF(Kernel::systemPCB->stack+systemPCBStackSize-1);
  sss = FP_SEG(Kernel::systemPCB->stack+systemPCBStackSize-1);
  tws = FP_SEG(&(Kernel::systemCall));
  twp = FP_OFF(&(Kernel::systemCall));
  asm{
    mov ax, systemEventid
    mov dx, systemOperation
    mov sp, ssp
    mov ss, sss
    push dx
    push ax
    mov ax, 1389
    push ax
    push ax
    // save PSW 
    pushf
    pop ax
    or ax, 1000000000b
    push ax
    // save CS of startfunction
    mov ax, tws
    push ax
    // save IP of startfunction
    mov ax, twp
    push ax
    //"unwind the stack"
    mov ax, 0
    push ax
    push ax
    push ax
    push ax
    push es
    push ds
    push si
    push di
    push bp
  }
}

extern void tick();

static void interrupt timerRoutine(){
  if(!systemEventid && Kernel::runningPCB && Kernel::runningPCB->shouldBeDispatched()){
    //cout << "d" << endl;
    Kernel::dispatch();
  }else{
    //cout << "no d " << systemEventid << endl;
  }
  tick();
  (*originalTimerRoutine)();
}

InterruptHandler Kernel::swapHandlers(IVTNo ivtNo, InterruptHandler newhandler){
  lock();
  unsigned ivtno = ivtNo*4;
  unsigned oldSEG, oldOFF;
  unsigned newSEG = FP_SEG(newhandler);
  unsigned newOFF = FP_OFF(newhandler);


  asm {
      push es
      push ax
      push bx

      mov ax, 0
      mov es, ax
      mov bx, word ptr ivtno 

      push word ptr es:bx+2
      pop word ptr oldSEG
      push word ptr es:bx
      pop word ptr oldOFF

      push word ptr newSEG
      pop word ptr es:bx+2
      push word ptr newOFF
      pop word ptr es:bx

      pop bx
      pop ax
      pop es
  }

  unlock();
  return (InterruptHandler) MK_FP(oldSEG,oldOFF);
}

void Kernel::createStack(PCB* newPCB, StackSize stackSize, void* startfunction)
{
  lock();
  if(!runningPCB){
    s_UserThreadPCB = newPCB;
  }
  byte* stack = new byte[stackSize];

  sss = FP_SEG(stack+stackSize-1);
  ssp = FP_OFF(stack+stackSize-1);
  tws = FP_SEG(startfunction);
  twp = FP_OFF(startfunction);
  rts = FP_SEG(newPCB->m_Thread);
  rtp = FP_OFF(newPCB->m_Thread);

  asm {
    push ax
    mov oldSS, ss
    mov oldSP, sp
    mov ss, sss
    mov sp, ssp
    // save CS of the parametar
    mov ax, rts
    push ax
    // save IP of the parametar
    mov ax, rtp
    push ax
    mov ax, 1389
    push ax
    push ax
    // save PSW 
    pushf
    pop ax
    or ax, 1000000000b
    push ax
    // save CS of startfunction
    mov ax, tws
    push ax
    // save IP of startfunction
    mov ax, twp
    push ax
    //"unwind the stack"
    mov ax, 0
    push ax
    push ax
    push ax
    push ax
    push es
    push ds
    push si
    push di
    push bp

    mov sss, ss
    mov ssp, sp
    mov ss, oldSS
    mov sp, oldSP
    pop ax
  }
  newPCB->ss = sss;
  newPCB->sp = ssp;
  newPCB->stack = stack;
  unlock();
}

void Kernel::createSystemStack(PCB* newPCB, StackSize stackSize, void* functionpointer){
  lock();
  byte* stack = new byte[stackSize];
  sss = FP_SEG(stack+stackSize-1);
  ssp = FP_OFF(stack+stackSize-1);
  tws = FP_SEG(functionpointer);
  twp = FP_OFF(functionpointer);

  asm{
    push ax
    mov oldSS, ss
    mov oldSP, sp
    mov ss, sss
    mov sp, ssp
    mov ax, 1389
    push ax
    push ax
    // save PSW 
    pushf
    pop ax
    or ax, 1000000000b
    push ax
    // save CS of startfunction
    mov ax, tws
    push ax
    // save IP of startfunction
    mov ax, twp
    push ax
    //"unwind the stack"
    mov ax, 0
    push ax
    push ax
    push ax
    push ax
    push es
    push ds
    push si
    push di
    push bp

    mov sss, ss
    mov ssp, sp
    mov ss, oldSS
    mov sp, oldSP
    pop ax
  }
  newPCB->ss = sss;
  newPCB->sp = ssp;
  newPCB->stack = stack;
  unlock();
}

void Kernel::systemCall(ID id, unsigned int operation){
  lock();
  KernelObj* ko = KernelObj::get(id);
  switch(operation){
    case 42:
      ko->block();
      break;
    case 43:
      ko->signal();
      break;
    default:
      break;
  }
  dispatch();
  unlock();
}

void interrupt Kernel::dispatch(){
  lock();
  dispatchActive = 1;
  if(!runningPCB){
    asm{
      mov sp,kernelSP
      mov ss,kernelSS
    }
    unlock();
    return;
  }
  if(!systemEventid){
    asm{
      mov ssp, sp
      mov sss, ss
    }
    if(runningPCB->state == PCB::READY){
      kernelSP = ssp;
      kernelSS = sss;
    }else{
      runningPCB->sp = ssp;
      runningPCB->ss = sss;
    }
  }else{
    systemEventid = 0;
  }
  if(runningPCB->state == PCB::RUNNING)
  {
    runningPCB->state=PCB::READY;
    Scheduler::put((PCB*)runningPCB);
  }
  runningPCB = Scheduler::get();
  if(!runningPCB){
    asm{
      mov sp, kernelSP
      mov ss, kernelSS
    }
  }else{
    runningPCB->state = PCB::RUNNING;

    ssp = runningPCB->sp;
    sss = runningPCB->ss;
    asm{
      mov sp, ssp
      mov ss, sss
    }
  }
  dispatchActive = 0;
  unlock();
  //cout << "leaving on " << runningPCB->m_ID << runningPCB->ss << ":" << runningPCB->sp << endl;
}

PCB* Kernel::block(){
  runningPCB->state = PCB::BLOCKED;
  return (PCB*)runningPCB;
}

void Kernel::deblock(volatile PCB* pcb){
  if(pcb!=runningPCB){
    pcb->state = PCB::READY;
    Scheduler::put((PCB*)pcb);
  }
}

void Kernel::initialize(int argc, char* argv[]){
  originalTimerRoutine = swapHandlers(0x08, timerRoutine);
  originalSystemRoutine = swapHandlers(0x60, systemRoutine);
  UserThread* u = new UserThread(argc,argv); //just like that
  systemPCB = PCB::createSystemPCB(systemPCBStackSize, (void*)(&(systemCall)));
  runningPCB = s_UserThreadPCB;
  u->start();
  dispatch();
}

int Kernel::kill(){
  lock();
  UserThread* u = (UserThread*)(s_UserThreadPCB->m_Thread);
  int result = u->getInvocationResult();
  delete u;
  while(!AllThreadPCBs.isEmpty())
  {
    cout << "Still " << AllThreadPCBs.size() << " PCBs left" << endl;
    PCB* pcb = AllThreadPCBs.getFromFront();
    cout << pcb->m_ID << " => " << pcb->state << endl;
    //dispatch();
  }
  swapHandlers(0x08, originalTimerRoutine);
  swapHandlers(0x60, originalSystemRoutine);
  unlock();
  return result;
}

