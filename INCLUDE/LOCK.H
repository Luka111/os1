//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   lock.h                  
//===================================================

#ifndef _lock_h_
#define _lock_h_

static void lock ()
{
    asm cli;
}

static void unlock ()
{
    asm sti;
}

#endif
