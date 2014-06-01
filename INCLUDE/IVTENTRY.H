//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   ivtentry.h                  
//===================================================

#ifndef _IVTENTRY_H
#define _IVTENTRY_H

#include "..\INCLUDE\TYPES.H"

class IVTEntry {
public:
  IVTEntry(IVTNo ivtNo, InterruptHandler ih);
  ~IVTEntry();
  void signal();
  void execOriginalInterruptHandler();
  ID m_KernelEventID;
  static IVTEntry* getByIvtNo(IVTNo ivtNo);
private:
  IVTNo m_ivtNo;
  InterruptHandler m_OriginalHandler;
};

#endif
