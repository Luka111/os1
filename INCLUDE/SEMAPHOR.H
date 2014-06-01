//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   semaphor.h                  
//===================================================

#ifndef _semaphor_h_
#define _semaphor_h_

#include "../INCLUDE/TYPES.H"

class Semaphore{
public:
	Semaphore(int init=1);
	virtual ~Semaphore();

	virtual void wait();
	virtual void signal();

	int val() const;  // Returns the current value of the semaphore
private:
  ID m_KernelSemID;
};
#endif
