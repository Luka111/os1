//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   semaphor.cpp
//===================================================

#include "..\INCLUDE\SEMAPHOR.H"
#include "..\INCLUDE\KRNELOBJ.H"
#include "..\INCLUDE\KRNELSEM.H"

Semaphore::Semaphore(int val):
	m_KernelSemID(KernelObj::createKernelSemaphore(val)){
}


Semaphore::~Semaphore(){
  KernelObj::destroy(m_KernelSemID);
}


void Semaphore::wait(){
	KernelObj::get(m_KernelSemID)->wait();
}


void Semaphore::signal(){
	KernelObj::get(m_KernelSemID)->signal();
}


int Semaphore::val()const{
	return ((KernelSemaphore*)KernelObj::get(m_KernelSemID))->val();
}

