#include "..\INCLUDE\KERNEL.H"
#include "..\INCLUDE\SEGMENT.H"
#include "..\INCLUDE\LOCK.H"
#include "..\INCLUDE\KERNELEV.H"
#include "..\INCLUDE\SCHEDULE.H"
#include "..\INCLUDE\PCB.H"
#include "..\INCLUDE\THREAD.H"
#include "..\INCLUDE\USRTHRD.H"


PCB* Kernel::runningPCB=NULL;

static PCB* s_UserThreadPCB = NULL;

List<PCB> Kernel::AllThreadPCBs;

static InterruptHandler originalTimerRoutine;

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

void Kernel::dispatch(){
  lock();
  if(!runningPCB){
    runningPCB = s_UserThreadPCB;
  }else{
    saveStack();
  }
  if(runningPCB->state == PCB::RUNNING)
  {
    runningPCB->state=PCB::READY;
    Scheduler::put(runningPCB);
  }
  runningPCB = Scheduler::get();
  runningPCB->state = PCB::RUNNING;
  restoreStack();
  unlock();
}

static unsigned sss, ssp, tws, twp, rts, rtp, oldSP, oldSS;

void Kernel::createStack(PCB* newPCB, StackSize stackSize, void* threadwrapper)
{
  if(!runningPCB){
    s_UserThreadPCB = newPCB;
  }
  AllThreadPCBs.addToBack(newPCB);
	lock();
	byte* stack = new byte[stackSize];

	sss = FP_SEG(stack+stackSize-1);
	ssp = FP_OFF(stack+stackSize-1);
	tws = FP_SEG(threadwrapper);
	twp = FP_OFF(threadwrapper);
  rts = FP_SEG(newPCB->m_Thread);
  rtp = FP_OFF(newPCB->m_Thread);

	newPCB->ss=sss;
	newPCB->sp=ssp;

	asm {
    push ax
		mov oldSS, ss
		mov oldSP, sp
		mov ss, sss
		mov sp, ssp
		// save CS of thread to run
		mov ax, rts
		push ax
		// save IP of thread to run
		mov ax, rtp
		push ax
		// save CS of threadwrapper
		mov ax, tws
		push ax
		// save IP of threadwrapper
		mov ax, twp
		push ax
    //"unwind the stack"
		push bp
		push si
		push di
		// save PSW
		pushf
		pop ax
		or ax, 1000000000b
		push ax
		// save rest of the registers
		mov ax,11
		push ax
		mov ax,12
		push ax //bx
		mov ax,13
		push ax //cx
		mov ax,14
		push ax //dx
		push es
		push ds
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
 // originalTimerRoutine = swapHandlers(0x08, timerRoutine);
 // swapHandlers(0x60, systemRoutine);
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

