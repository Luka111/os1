//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   main.cpp
//===================================================

#include "..\INCLUDE\KERNEL.H"

int main(int argc, char* argv[]){
  Kernel::initialize(argc,argv);
  return Kernel::kill();
}
