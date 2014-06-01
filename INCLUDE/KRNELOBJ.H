//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   krnelobj.h                  
//===================================================

#ifndef __KERNELOBJ_H
#define __KERNELOBJ_H

#include "..\INCLUDE\TYPES.H"

class Thread;

class KernelObj {
protected:
  KernelObj();
  virtual ~KernelObj();
  ID m_ID;
public:
  void asmwait(); //switcher to system call
  virtual void wait() = 0; //called by the user code
  virtual void block(); //called by the kernel
  virtual void signal() = 0;
  static ID createKernelEvent(IVTNo ivtNo);
  static ID createPCB(Thread* parentThread, StackSize stackSize, Time timeSlice, void* threadwrapper);
  static ID createKernelSemaphore(int val);
  static KernelObj* get(ID id);
  static void destroy(ID id);
protected:
  static ID putObj(KernelObj* obj);
};

#endif
