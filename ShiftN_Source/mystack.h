#ifndef mystack_h
#define mystack_h
#include <stddef.h>

template<class T>
class Stack {
	struct stackEl {
		T el_daten;
		stackEl* Naechster;
	};
	stackEl* first;
	public:
	Stack();
	int push(T);
	int pop(T&);
};

template<class T>
Stack<T>::Stack() {
	first=NULL;
}

template<class T>
int Stack<T>::push(T el) {
	stackEl *temp=new stackEl;
	if (!temp) return 0;
	temp->el_daten=el;
	temp->Naechster=first;
	first=temp;
	return 1;
}

template<class T>
int Stack<T>::pop(T& el) {
	if (!first) return 0;
	el=first->el_daten;
	stackEl *temp=first;
	first=temp->Naechster;
	delete temp;
	return 1;
}
#endif

