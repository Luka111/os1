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
