#ifndef __PRPENTRY_H
#define __PRPENTRY_H

#define PREPAREENTRY(IVTN,useOldRoutine)            \
  static void interrupt interruptRoutine##IVTN () ;         \
  IVTEntry entry##IVTN = IVTEntry(0x##IVTN,interruptRoutine##IVTN); \
  void interrupt interruptRoutine##IVTN ()          \
  {                     \
    ID id = entry##IVTN.m_KernelEventID;  \
    if(id){ \
      if(useOldRoutine)               \
        entry##IVTN.execOriginalInterruptHandler();        \
      asm {mov ax, id}\
      asm {mov dx, 43}\
      asm {int 60h}\
    } else {\
      entry##IVTN.execOriginalInterruptHandler();        \
    } \
  };


#endif