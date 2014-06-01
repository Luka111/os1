//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   event.h                  
//===================================================

#ifndef _event_h_
#define _event_h_

#include "../INCLUDE/TYPES.H"

class Event{
public:
	Event(IVTNo ivtNo);
	~Event();

	void wait();

protected:
	void signal();

private:
	ID m_KernelEvId;

};

//to make user code happy
#include "../INCLUDE/IVTENTRY.H"
#include "../INCLUDE/KERNEL.H"
#include "../INCLUDE/PRPENTRY.H"

#endif
