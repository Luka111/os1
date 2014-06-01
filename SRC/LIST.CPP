//==================================================
//   Projektni zadatak iz Operativnih sistema 1  
//   Student: Luka Petrovic 0069 / 12      
//   list.cpp
//===================================================


#include "List.h"

template <class T>
inline List<T>::List()
{
	lock();
	head = tail = 0;
	count = 0;
	unlock();
}

template <class T>
List<T>::~List()
{
	lock();
	while(head)
	{
		Node* old = head;
		head = old->next;
		delete old;
	}
	unlock();
}

template <class T>
inline void List<T>::addToBack(T* d)
{
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

template <class T>
inline T* List<T>::getFromFront()
{
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

template <class T>
int List<T>::remove(T *pointer)
{
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

template <class T>
inline int List<T>::size() const
{
	return count;
}

template <class T>
inline int List<T>::isEmpty() const
{
	return !count;
}
