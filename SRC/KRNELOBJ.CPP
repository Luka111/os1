#include "..\INCLUDE\KRNELOBJ.H"
#include "..\INCLUDE\IDMAP.H"
#include "..\INCLUDE\LOCK.H"
#include "..\INCLUDE\KERNEL.H"



#include <iostream.h>


static IDMap<KernelObj> g_Map;

KernelObj::KernelObj():
m_ID(g_Map.put(this)){
}

KernelObj::~KernelObj(){
  g_Map.remove(m_ID);
}

void KernelObj::asmwait(){
  lock();
  unsigned id = m_ID;
  asm {
    mov ax, id
    mov dx, 42
    int 60h
  }
  unlock();
}

void KernelObj::block(){
  cout << "KernelObj blocking" << m_ID << endl;
  Kernel::block();
}

KernelObj* KernelObj::get(ID id){
  return g_Map.get(id);
}

void KernelObj::destroy(ID id){
  delete g_Map.get(id);
}

ID KernelObj::putObj(KernelObj* obj){
  //cout << "putting obj " << obj << " to map" << endl;
  return g_Map.put(obj);
}
