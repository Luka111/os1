#ifndef __USERTHREAD_H
#define __USERTHREAD_H

#include "..\INCLUDE\THREAD.H"

class UserThread : public Thread{
public:
  UserThread(int argc, char* argv[]);
  void run();
  inline int getInvocationResult() const {return m_Result;}
private:
  int m_Argc;
  char** m_Argv;
  int m_Result;
};

#endif
