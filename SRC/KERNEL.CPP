#include "..\INCLUDE\KERNEL.H"
#include "..\INCLUDE\SEGMENT.H"
#include "..\INCLUDE\LOCK.H"
#include "..\INCLUDE\KERNELEV.H"
#include "..\INCLUDE\SCHEDULE.H"
#include "..\INCLUDE\PCB.H"
#include "..\INCLUDE\THREAD.H"
//#include "..\INCLUDE\USRTHRD.H"

#include <iostream.h>

extern int userMain(int argc, char* argv[]);

class UserThread : public Thread {
  int m_Result;
  int m_Argc;
  char** m_Argv;
public:
  UserThread(int argc, char* argv[]):
    Thread(),
    m_Result(-14),
    m_Argc(argc),
    m_Argv(argv){}
  void run(){
    m_Result = userMain(m_Argc,m_Argv);
  }
  inline int result() const {return m_Result;}
};

class KernelThread : public Thread {
  UserThread* m_UserThread;
  int m_Argc;
  char** m_Argv;
public:
  KernelThread(int argc, char* argv[]):
    Thread(),
    m_UserThread(NULL),
    m_Argc(argc),
    m_Argv(argv){
    }
  ~KernelThread(){
    delete m_UserThread;
  }
  void run(){
    m_UserThread = new UserThread(m_Argc,m_Argv);
    m_UserThread->start();
    m_UserThread->waitToComplete();
  }
  int result() const {return m_UserThread->result();}
} * s_KernelThread;

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
    runningPCB = s_UserThreadPCB;
    runningPCB->state = PCB::RUNNING;
    systemEventid = -1; //skip the initial setting of sp and ss for this PCB
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
    //cout << "No running" << endl;
    unlock();
    return;
  }
  if(!systemEventid){
    asm{
      mov ssp, sp
      mov sss, ss
    }
    runningPCB->sp = ssp;
    runningPCB->ss = sss;
  }else{
    if(systemEventid==-1){
      asm{
        mov kernelSP, sp
        mov kernelSS, ss
      }
    }
    systemEventid = 0;
  }
  if(runningPCB->state == PCB::RUNNING)
  {
    runningPCB->state=PCB::READY;
    Scheduler::put((PCB*)runningPCB);
  }else{
    //cout << runningPCB->m_ID << " is not in RUNNING state, but in " << runningPCB->state << endl;
  }
  runningPCB = Scheduler::get();
  if(!runningPCB){
    //cout << "No running after dispatch" << endl;
    asm{
      mov sp, kernelSP
      mov ss, kernelSS
    }
    unlock();
    return;
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
  lock();
  //cout << "Blocking " << runningPCB->m_ID << endl;
  runningPCB->state = PCB::BLOCKED;
  return (PCB*)runningPCB;
  unlock();
}

void Kernel::deblock(volatile PCB* pcb){
  lock();
  if(pcb!=runningPCB){
    //cout << "Deblocking " << pcb->m_ID << endl;
    pcb->state = PCB::READY;
    Scheduler::put((PCB*)pcb);
  }
  unlock();
}

void Kernel::initialize(int argc, char* argv[]){
  //UserThread* u = new UserThread(argc,argv); //just like that
  systemPCB = PCB::createSystemPCB(systemPCBStackSize, (void*)(&(systemCall)));
  (new KernelThread(argc,argv))->start();
  originalTimerRoutine = swapHandlers(0x08, timerRoutine);
  originalSystemRoutine = swapHandlers(0x60, systemRoutine);
  //u->start();
  dispatch();
}

int Kernel::kill(){
  lock();
  KernelThread* kt = (KernelThread*)(s_UserThreadPCB->m_Thread);
  int result = kt->result();
  delete kt;
  while(!AllThreadPCBs.isEmpty())
  {
    //cout << "Still " << AllThreadPCBs.size() << " PCBs left" << endl;
    PCB* pcb = AllThreadPCBs.getFromFront();
    //cout << pcb->m_ID << " => " << pcb->state << endl;
    dispatch();
  }
  swapHandlers(0x08, originalTimerRoutine);
  swapHandlers(0x60, originalSystemRoutine);
  unlock();
  return result;
}

