#ifndef __TYPES_H
#define __TYPES_H

#include "_Null.h"
#include "Lock.h"
#include "Bool.h"

typedef int ID;
typedef unsigned char byte;
typedef unsigned char IVTNo;
typedef unsigned long StackSize;
typedef unsigned int Time; // time, x 55ms

typedef void interrupt (*InterruptHandler)();

#endif
