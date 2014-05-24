#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "..\INCLUDE\TYPES.H"
#include "..\INCLUDE\LIST.H"

class PCB;

class Kernel {
  public:
  static PCB* runningPCB;
  static void block();
  static void deblock(PCB* pcb);
  static InterruptHandler swapHandlers(IVTNo ivtNo, InterruptHandler newhandler);
  static void dispatch();
  static void initialize(int argc, char* argv[]);
  static int kill();
  static void createStack(PCB*, StackSize stackSize, void* threadwrapper);
  static void saveStack();
  static void restoreStack();
  static List<PCB> Kernel::AllThreadPCBs;
};

#endif
