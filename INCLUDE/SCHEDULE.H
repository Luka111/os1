//schedule.h

#ifndef _schedule_h_
#define _schedule_h_


class PCB;

class Scheduler{
public:
	static void put(PCB*);
	static PCB* get();

};


#endif