//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   thread.h                  
//===================================================

#ifndef _thread_h_
#define _thread_h_

typedef unsigned long StackSize;
const StackSize defaultStackSize=4096;
typedef unsigned int Time; // time, x 55ms
const Time defaultTimeSlice=2; // default = 2*55ms
typedef int ID;

class Thread{

public:
	void start();
	void waitToComplete();

	virtual ~Thread();

protected:

	Thread(StackSize stackSize = defaultStackSize, Time timeSlice= 
    defaultTimeSlice);
	virtual void run(){};

private:
	ID m_PCBId;
  static void wrapper(Thread* running);

};


void dispatch();

#endif
