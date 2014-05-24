#ifndef __KERNELSEMAPHORE_H
#define __KERNELSEMAPHORE_H

#include "..\INCLUDE\KRNELOBJ.H"
#include "..\INCLUDE\LIST.H"

class PCB;

class KernelSemaphore : public KernelObj {
private:
  int m_Val;
  List<PCB> m_BlockedPCBs;
public:
  KernelSemaphore(int val);
  void wait();
  void block();
  void signal();
  inline int val() const {return m_Val;}
};

#endif
