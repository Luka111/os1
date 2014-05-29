//PCB.H

#ifndef _pcb_h_
#define _pcb_h_

#include "..\INCLUDE\TYPES.H"
#include "..\INCLUDE\LIST.H"
#include "..\INCLUDE\KRNELOBJ.H"

class Thread;
typedef unsigned long StackSize;

struct PCB : public KernelObj{
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
  static PCB* createSystemPCB(StackSize stackSize, void* systemCallFunctionPointer);

private:
	Time m_timeQuantsToRun;
	Time m_timeQuantsRan;

protected:
  //constructor for system PCB
  PCB(StackSize sz, void* functionpointer);
	List<PCB> m_Waiting ;
    
};

#endif
