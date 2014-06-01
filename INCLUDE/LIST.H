//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   list.h                  
//===================================================

#ifndef _list_h_
#define _list_h_

#include "../INCLUDE/LOCK.H"

template <class T>
class List
{
  public:

    struct Node {
      T* data; 
      Node* next;

      Node (T* d, Node* n=0) : data(d), next(n) {};
    };

    Node *head, *tail;

    List (){
      head = tail = 0;
      count = 0;
    }
    ~List (){
      while(head)
      {
        Node* old = head;
        head = old->next;
        delete old;
      }
    }

    void addToBack (T* d){
      Node* newNode = new Node(d);
      if (head)
        tail->next = newNode;
      else
        head = newNode;
      tail = newNode;
      count++;
    }
    T* getFromFront (){
      if (isEmpty())
      {
        return 0;
      }
      T* d = head->data;
      Node* old = head;
      head = old->next;
      delete old;
      count--;
      return d;
    }
    int remove(T* pointer){
      Node *current = head, *previous = 0;
      while((current->data != pointer) && (current != 0))
      {
        previous = current;
        current = current->next;
      }
      if (!current)
      {
        return -1;
      }
      if(previous){
        previous->next = current->next;
      }
      delete current;
      count--;
      return 0;
    }

    int size () const {
      return count;
    }
    int isEmpty() const{
      return !count;
    }

  private:

    int count;

};

#endif
