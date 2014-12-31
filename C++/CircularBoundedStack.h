#pragma once
#include<iostream>

using namespace std;

template<class T> 
class CircularBoundedStack{
	int start;
	int end;
	int size;
	int capacity;
	T* stack;
public:
	CircularBoundedStack(int c);
	~CircularBoundedStack();

	void push(T elem);
	T pop();
	int getSize(){ return size; }
	void clear();
	void printStack();
};

template<class T>
CircularBoundedStack<T>::CircularBoundedStack(int c) {
	stack = new T[c];
	size = 0;
	capacity = c;
	start = 0;
	end = -1;
}

template<class T>
CircularBoundedStack<T>::~CircularBoundedStack(){
	delete[] stack;
}

template<class T>
void CircularBoundedStack<T>::push(T elem) {
	end = (end + 1) % capacity;
	stack[end] = elem;

	if (size < capacity)
		size++;
	else
		start = (start + 1) % capacity;
}

template<class T>
T CircularBoundedStack<T>::pop() {
	if (size == 0)
		return T();

	T temp = stack[end];

	if (end == 0)
		end = capacity - 1;
	else
		end--;
	size--;

	return temp;
}

template<class T>
void CircularBoundedStack<T>::clear(){
	memset(stack, 0, capacity);
}

template<class T>
void CircularBoundedStack<T>::printStack() {
	for (int i = 0; i < capacity; i++) {
		if (stack[i] != T())
			cout << stack[i] << ", ";
	}
	cout << endl;
}