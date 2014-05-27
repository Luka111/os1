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

volatile PCB* Kernel::runningPCB=NULL;

static volatile PCB* s_UserThreadPCB = NULL;

volatile PCB* Kernel::systemPCB = NULL;

List<PCB> Kernel::AllThreadPCBs;

static InterruptHandler originalTimerRoutine;

static volatile unsigned sss, ssp, tws, twp, rts, rtp, oldSP, oldSS, kernelSS, kernelSP;

static void interrupt systemRoutine(){
  ID eventid;
  unsigned operation;
  asm{
    mov eventid, ax
    mov operation, dx
    mov ssp, sp
    mov sss, ss
  }
  Kernel::runningPCB->sp = ssp;
  Kernel::runningPCB->ss = sss;
  Kernel::runningPCB->state = PCB::BLOCKED;
  ssp = FP_OFF(Kernel::systemPCB->stack+systemPCBStackSize-1);
  sss = FP_SEG(Kernel::systemPCB->stack+systemPCBStackSize-1);
	tws = FP_SEG(&(Kernel::systemCall));
	twp = FP_OFF(&(Kernel::systemCall));
  cout << "ir 60 " << eventid << ", " << operation << endl;
  asm{
    mov ax, eventid
    mov dx, operation
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
    mov bp, sp
  }
}

extern void tick();

volatile int nja = 0;

static void interrupt timerRoutine(){
  //if(!(++nja %100)){
  /*
  if(++nja<10){
    cout << nja << endl;
  }
  */
  if(Kernel::runningPCB && Kernel::runningPCB->shouldBeDispatched()){
    //cout << "dispatching" << endl;
    Kernel::dispatch();
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


void interrupt Kernel::dispatch(){
  if(!runningPCB){
    return;
  }
  lock();
  asm{
    mov ssp, sp
    mov sss, ss
  }
  if(runningPCB->state == PCB::READY){
    asm{
      mov kernelSP, sp
      mov kernelSS, ss
    }
  }else{
    runningPCB->sp = ssp;
    runningPCB->ss = sss;
  }
  if(runningPCB->state == PCB::RUNNING)
  {
    runningPCB->state=PCB::READY;
    Scheduler::put((PCB*)runningPCB);
  }

  runningPCB = Scheduler::get();
  if(!runningPCB){
    cout << "no running" << endl;
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
  cout << "KernelObj ID " << id << " starting operation " << operation << endl;
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
  cout << "KernelObj ID " << id << " finished operation " << operation << ", dispatching now" << endl;
  dispatch();
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

void Kernel::block(){
  asm{
    mov ssp, sp
    mov sss, ss
  }
  runningPCB->sp = ssp;
  runningPCB->ss = sss;
  runningPCB->state = PCB::BLOCKED;
  runningPCB = Scheduler::get();
  runningPCB->state = PCB::RUNNING;
  ssp = runningPCB->sp;
  sss = runningPCB->ss;
  asm{
    mov sp, ssp
    mov ss, sss
  }
}

void Kernel::deblock(volatile PCB* pcb){
  lock();
  pcb->state = PCB::READY;
  Scheduler::put((PCB*)pcb);
  unlock();
}

void Kernel::initialize(int argc, char* argv[]){
  originalTimerRoutine = swapHandlers(0x08, timerRoutine);
  swapHandlers(0x60, systemRoutine);
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
    dispatch();
  }
  unlock();
  return result;
}

