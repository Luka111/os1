#ifndef __PRPENTRY_H
#define __PRPENTRY_H

#define PREPAREENTRY(IVTN,useOldRoutine)            \
  static void interrupt interruptRoutine##IVTN () ;         \
  IVTEntry entry##IVTN = IVTEntry(0x##IVTN,interruptRoutine##IVTN); \
  void interrupt interruptRoutine##IVTN ()          \
  {                     \
    entry##IVTN.signal();            \
    if(useOldRoutine)               \
      entry##IVTN.execOriginalInterruptHandler();        \
    Kernel::dispatch();             \
  };


#endif
