//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   systhrd.cpp
//===================================================

#include "..\INCLUDE\SYSTHRD.H"

SystemThread::SystemThread():
  Thread(4096,0),
  m_ShouldRun(1){
    (PCB*)(KernelObj::get(m_PCBId))->m_State = PCB::SYSTEM;
}

void SystemThread::run(){
  while(m_ShouldRun);
}
