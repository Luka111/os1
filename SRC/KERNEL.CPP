#include "..\INCLUDE\KERNEL.H"
#include "..\INCLUDE\SEGMENT.H"
#include "..\INCLUDE\LOCK.H"
#include "..\INCLUDE\KERNELEV.H"
#include "..\INCLUDE\SCHEDULE.H"
#include "..\INCLUDE\PCB.H"
#include "..\INCLUDE\THREAD.H"
#include "..\INCLUDE\USRTHRD.H"


#include <iostream.h>

PCB* Kernel::runningPCB=NULL;

static PCB* s_UserThreadPCB = NULL;

List<PCB> Kernel::AllThreadPCBs;

static InterruptHandler originalTimerRoutine;

static volatile unsigned sss, ssp, tws, twp, rts, rtp, oldSP, oldSS;

static void interrupt systemRoutine(){
  ID eventid;
  asm {
    mov eventid, ax;
    pop ax;
  }
  KernelObj* kev = KernelObj::get(eventid);
  kev->block();
}

extern void tick();

int nja = 0;

static void interrupt timerRoutine(){
  if(Kernel::runningPCB->shouldBeDispatched()){
    Kernel::dispatch();
  }
  tick();
  originalTimerRoutine();
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
      mov bx, ivtno 

      push word ptr es:bx
      pop word ptr oldSEG
      push word ptr es:bx+2
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
  lock();

  asm{
    mov ssp, sp
    mov sss, ss
  }
  if(runningPCB!=s_UserThreadPCB){
    runningPCB->sp = ssp;
    runningPCB->ss = sss;
  }

  if(runningPCB->state == PCB::RUNNING)
  {
    runningPCB->state=PCB::READY;
    Scheduler::put(runningPCB);
  }

  runningPCB = Scheduler::get();
  runningPCB->state = PCB::RUNNING;

  ssp = runningPCB->sp;
  sss = runningPCB->ss;
  asm{
    mov sp, ssp
    mov ss, sss
  }

  unlock();
}

void Kernel::createStack(PCB* newPCB, StackSize stackSize, void* threadwrapper)
{
	lock();
  if(!runningPCB){
    s_UserThreadPCB = newPCB;
  }
	byte* stack = new byte[stackSize];

	sss = FP_SEG(stack+stackSize-1);
	ssp = FP_OFF(stack+stackSize-1);
	tws = FP_SEG(threadwrapper);
	twp = FP_OFF(threadwrapper);
  rts = FP_SEG(newPCB->m_Thread);
  rtp = FP_OFF(newPCB->m_Thread);

	asm {
    push ax
    mov oldSS, ss
    mov oldSP, sp
		mov ss, sss
		mov sp, ssp
		// save PSW 
    pushf
    pop ax
    or ax, 1000000000b
    push ax
		// save CS of the parametar
		mov ax, rts
		push ax
		// save IP of the parametar
		mov ax, rtp
		push ax
    mov ax, 42
    push ax
    push ax
    push ax
		// save CS of threadwrapper
		mov ax, tws
		push ax
		// save IP of threadwrapper
		mov ax, twp
		push ax
    //"unwind the stack"
    mov ax, 0
    push ax
    mov bx, rtp
    push bx
    push ax
    push ax
    mov es, rts
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

void Kernel::saveStack()
{
  unsigned tss, tsp;
  asm {
    mov tsp, sp
    mov tss, ss
		pushf
		push ax
		push bx
		push cx
		push dx
		push es
		push ds
		mov ssp, sp
		mov sss, ss
    mov sp, tsp
    mov ss, tss
	}
	runningPCB->sp = ssp;
	runningPCB->ss = sss;
}

void Kernel::restoreStack()
{
  unsigned bla=45;
	sss = runningPCB->ss;
	ssp = runningPCB->sp;

	asm {
		mov ss, sss
		mov sp, ssp
		pop ds
		pop es
		pop dx
    mov bla, dx
		pop cx
    mov bla, cx
		pop bx
    mov bla, bx
		pop ax
    mov bla, ax
		popf
	}
}

void Kernel::block(){
  lock();
  saveStack();
  runningPCB->state = PCB::BLOCKED;
  runningPCB = Scheduler::get();
  runningPCB->state = PCB::RUNNING;
  restoreStack();
	unlock();
}

void Kernel::deblock(PCB* pcb){
  pcb->state = PCB::READY;
  Scheduler::put(pcb);
}

void Kernel::initialize(int argc, char* argv[]){
  lock();
  UserThread* u = new UserThread(argc,argv); //just like that
  runningPCB = s_UserThreadPCB;
  originalTimerRoutine = swapHandlers(0x08, timerRoutine);
  //swapHandlers(0x60, systemRoutine);
  u->start();
  dispatch();
  unlock();
}

int Kernel::kill(){
  lock();
  while(!AllThreadPCBs.isEmpty())
  {
    dispatch();
  }
  UserThread* u = (UserThread*)(s_UserThreadPCB->m_Thread);
  int result = u->getInvocationResult();
  delete u;
  unlock();
  return result;
}

