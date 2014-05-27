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
