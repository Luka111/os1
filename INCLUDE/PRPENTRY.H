//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   prpentry.h                  
//===================================================

#ifndef __PRPENTRY_H
#define __PRPENTRY_H

#define PREPAREENTRY(IVTN,useOldRoutine)            \
  static void interrupt interruptRoutine##IVTN () ;         \
  IVTEntry entry##IVTN = IVTEntry(0x##IVTN,interruptRoutine##IVTN); \
  void interrupt interruptRoutine##IVTN ()          \
  {                     \
    ID id = entry##IVTN.m_KernelEventID;  \
    if(id){ \
      Kernel::triggerSystemCall(id,43);  \
      if(useOldRoutine)               \
        entry##IVTN.execOriginalInterruptHandler();        \
    } else {\
      entry##IVTN.execOriginalInterruptHandler();        \
    } \
  };


#endif
