#ifndef _list_h_
#define _list_h_

#include "Bool.h"
#include "Lock.h"

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
      lock();
      head = tail = 0;
      count = 0;
      unlock();
    }
    ~List (){
      lock();
      while(head)
      {
        Node* old = head;
        head = old->next;
        delete old;
      }
      unlock();
    }

    void addToBack (T* d){
      lock();
      Node* newNode = new Node(d);
      if (head)
        tail->next = newNode;
      else
        head = newNode;
      tail = newNode;
      count++;
      unlock();
    }
    T* getFromFront (){
      lock();
      if (isEmpty())
      {
        unlock();
        return 0;
      }
      T* d = head->data;
      Node* old = head;
      head = old->next;
      delete old;
      count--;
      unlock();
      return d;
    }
    int remove(T* pointer){
      lock();
      Node *current = head, *previous = 0;
      while((current->data != pointer) && (current != 0))
      {
        previous = current;
        current = current->next;
      }
      if (!current)
      {
        unlock();
        return -1;
      }
      previous->next = current->next;
      delete current;
      count--;
      unlock();
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
