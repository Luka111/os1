//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   pcb.h                  
//===================================================

#ifndef _pcb_h_
#define _pcb_h_

#include "..\INCLUDE\TYPES.H"
#include "..\INCLUDE\LIST.H"
#include "..\INCLUDE\KRNELOBJ.H"

class Thread;
typedef unsigned long StackSize;

struct PCB : public KernelObj{
  PCB(); //this one creates the system PCB
	PCB(Thread *thr, StackSize sz , Time t, void* threadwrapper);
  ~PCB();
	static enum {NEW,RUNNING,READY,BLOCKED,TERMINATED,SYSTEM};
	volatile int state;
	
	Thread *m_Thread;

	byte *stack;
	unsigned sp;
	unsigned ss;

  void wait();
  void block();
  void signal();
  void addToWaitingQueue(PCB* blockingPCB);

  int shouldBeDispatched();

private:
	Time m_timeQuantsToRun;
	Time m_timeQuantsRan;

protected:
  //constructor for system PCB
	List<PCB> m_Waiting ;
    
};

#endif
