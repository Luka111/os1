//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   systhrd.h                  
//===================================================

#ifndef __SYSTEMTHREAD_H
#define __SYSTEMTHREAD_H

class SystemThread : public Thread{
public:
  enum Command{CREATEKERNELEVENT,CREATEKERNELSEMAPHORE,CREATEPCB,DESTROY};
  SystemThread();
  void run();
private:
  int m_ShouldRun; 
}

#endif
