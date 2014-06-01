//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   kernel.h                  
//===================================================

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "..\INCLUDE\TYPES.H"
#include "..\INCLUDE\LIST.H"

class PCB;

struct Kernel {
  static volatile PCB* runningPCB;
  static void triggerSystemCall(ID id, unsigned operation);
  static PCB* block();
  static void deblock(volatile PCB* pcb);
  static InterruptHandler swapHandlers(IVTNo ivtNo, InterruptHandler newhandler);
  static void interrupt dispatch();
  static void initialize(int argc, char* argv[]);
  static int kill();
  static void createStack(PCB* newPCB, StackSize stackSize, void* threadwrapper);
  static void createSystemStack(PCB* newPCB);
  static List<PCB> Kernel::AllThreadPCBs;
};

#endif
