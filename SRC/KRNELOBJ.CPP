//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   krnelobj.cpp
//===================================================

#include "..\INCLUDE\KRNELOBJ.H"
#include "..\INCLUDE\IDMAP.H"
#include "..\INCLUDE\LOCK.H"
#include "..\INCLUDE\KERNEL.H"

static IDMap<KernelObj> g_Map;

KernelObj::KernelObj():
m_ID(g_Map.put(this)){
}

KernelObj::~KernelObj(){
  g_Map.remove(m_ID);
}

void KernelObj::asmwait(){
  Kernel::triggerSystemCall(m_ID,42);
}

void KernelObj::block(){
  Kernel::block();
}

KernelObj* KernelObj::get(ID id){
  return g_Map.get(id);
}

void KernelObj::destroy(ID id){
  delete g_Map.remove(id);
}

ID KernelObj::putObj(KernelObj* obj){
  return g_Map.put(obj);
}
