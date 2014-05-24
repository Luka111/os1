//event.cpp


#include "..\INCLUDE\Event.h"
#include "..\INCLUDE\KRNELOBJ.H"

Event::Event(IVTNo ivtNo):
	m_KernelEvId(KernelObj::createKernelEvent(ivtNo)){
}

Event::~Event(){
	KernelObj::destroy(m_KernelEvId);
}

void Event::wait(){
  KernelObj::get(m_KernelEvId)->wait();
}
void Event::signal(){
  KernelObj::get(m_KernelEvId)->signal();
}
