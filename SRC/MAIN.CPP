#include "..\INCLUDE\KERNEL.H"

int main(int argc, char* argv[]){
  Kernel::initialize(argc,argv);
  return Kernel::kill();
}
