//semaphor.h

#ifndef _semaphor_h_
#define _semaphor_h_

#include "../INCLUDE/THREAD.H" //just for ID

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
