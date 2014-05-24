#ifndef _IVTENTRY_H
#define _IVTENTRY_H

#include "..\INCLUDE\TYPES.H"

class IVTEntry {
public:
  IVTEntry(IVTNo ivtNo, InterruptHandler ih);
  void signal();
  void execOriginalInterruptHandler();
private:
  IVTNo m_ivtNo;
  InterruptHandler m_OriginalHandler;
};

#endif
