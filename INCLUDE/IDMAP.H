#include "..\INCLUDE\LOCK.H"

extern ID g_IDMapCounter;
template <class T> class IDMap {
  struct IDMapEntry {
    T* m_Pt;
    ID m_Id;
    IDMapEntry* next;
    //IDMapEntry():m_Pt(0),m_Id(0){};
    IDMapEntry(T* pt, ID id,IDMapEntry* nxt=0):m_Pt(pt),m_Id(id),next(nxt){};
    //int operator==(const IDMapEntry& other) const {return m_Id==other.m_Id;}
  };
  IDMapEntry *prvi, *posl;
  void isprazni(){
    while (prvi){
      IDMapEntry* stari = prvi;
      prvi = prvi->next;
      delete stari;
    }
    posl = 0;
  }
public:
  IDMap():prvi(0),posl(0){};
  ID put(T* pt){
    lock();
    g_IDMapCounter++;
    posl = (!prvi ? prvi : posl->next) = new IDMapEntry(pt,g_IDMapCounter);
    unlock();
    return g_IDMapCounter;
  }
  T* get(ID id){
    IDMapEntry *tek = prvi;
    while ((tek->m_Id != id) && (tek!=0)){
      tek = tek->next;
    }
    if (!tek){
      return 0;
    }
    return tek->m_Pt;
  }
  T* remove(ID id){
    IDMapEntry *tek = prvi, *pret = 0;
    while ((tek->m_Id != id) && (tek!=0)){
      pret = tek;
      tek = tek->next;
    }
    if (!tek){
      return 0;
    }
    if (pret!=0){
      pret->next = tek->next;
    }
    tek->next = 0;
    T* vrati = tek->m_Pt;
    delete tek;
    return vrati;
  }
  ~IDMap(){ isprazni(); }
};


