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
  Kernel::triggerSystemCall(m_ID,42);
}

void KernelObj::block(){
  //cout << m_ID << " blocking" << endl;
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
