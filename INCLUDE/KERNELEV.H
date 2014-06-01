//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   kernelev.h                  
//===================================================

#ifndef __KERNELEVENT_H
#define __KERNELEVENT_H

#include "..\INCLUDE\TYPES.H"
#include "..\INCLUDE\KRNELOBJ.H"

class PCB;

class KernelEvent : public KernelObj{
public:
  KernelEvent(IVTNo ivtNo);
  ~KernelEvent();
  void wait();
  void signal();
  static ID getByIvtNo(IVTNo ivtNo);
private:
  IVTNo m_ivtNo;
  volatile PCB* m_ParentPCB;
};

#endif
