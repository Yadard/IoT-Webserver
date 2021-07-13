#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <Arduino.h>

#ifdef TRACK_MEMORY
size_t memory_usage = 0;

void* operator new(size_t size){
    Serial.printf("[LinkedList] allocanting %llu bytes\n", size);
    memory_usage += size;
    printf("[Dynamic Memory] current usage = %llu bytes\n", memory_usage);
    return malloc(size);
}

void operator delete(void* memory, size_t size){
    Serial.printf("[LinkedList] freeing %llu bytes\n", size);
    Serial.printf("[Dynamic Memory] current usage = %llu bytes\n", memory_usage);
    free(memory);
}
#endif

template<class DataType>
struct Node{
    DataType data;
    Node* next;

    Node(DataType d) : data(d), next(nullptr) {}
};

template<class DataType>
class LinkedList
{
private:
    typedef Node<DataType> node;

    node* head;
    uint16_t size;
public:
    LinkedList();
    LinkedList(DataType);
    ~LinkedList();

    node* getTail();

    void pushBack(DataType);

    void insert(DataType, int16_t);

    void remove(int16_t);

    DataType operator[](int16_t);

    uint16_t getSize();
};


template<class DataEntry>
class Queue{
private:
    LinkedList<DataEntry> content;
public:
    DataEntry peek();

    DataEntry pop();

    void add(DataEntry);

    uint16_t getSize(); 
};

//! If you now a prittier way to separate template classes/functions in .hpp and .cpp. Please let me know
#include "Queue.cpp"
#define QUEUE_CPP
#endif
