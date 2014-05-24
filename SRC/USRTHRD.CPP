#include "..\INCLUDE\USRTHRD.H"

extern int userMain(int argc, char* argv[]);

UserThread::UserThread(int argc, char* argv[]):
  Thread(),
  m_Result(-1){
}

void UserThread::run(){
  int m_Result = userMain(m_Argc,m_Argv);
}

