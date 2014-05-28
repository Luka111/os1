#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "..\INCLUDE\TYPES.H"
#include "..\INCLUDE\LIST.H"

class PCB;

class Kernel {
  public:
  static volatile PCB* runningPCB;
  static volatile PCB* s_UserThreadPCB;
  static volatile PCB* systemPCB;
  static void triggerSystemCall(ID id);
  static interrupt void block();
  static void deblock(volatile PCB* pcb);
  static InterruptHandler swapHandlers(IVTNo ivtNo, InterruptHandler newhandler);
  static void interrupt dispatch();
  static void initialize(int argc, char* argv[]);
  static int kill();
  static void createStack(PCB* newPCB, StackSize stackSize, void* threadwrapper);
  static void createSystemStack(PCB* newPCB, StackSize stackSize, void* functionpointer);
  static void systemCall(ID id, unsigned int operation);
  static void saveStack();
  static void restoreStack();
  static List<PCB> Kernel::AllThreadPCBs;
};

#endif
