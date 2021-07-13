#include "Queue.hpp"

#ifndef QUEUE_CPP
template<class DataType>
LinkedList<DataType>::LinkedList(){
    head = nullptr;
    size = 0;
}

template<class DataType>
LinkedList<DataType>::LinkedList(DataType data){
    head = new node(data);
    size = 1;
}

template<class DataType>
LinkedList<DataType>::~LinkedList(){
    node* current = head;
    node* previous = nullptr;
    while (current != nullptr)
    {
        previous = current;
        current = current->next;
        delete previous;
    }
}

template<class DataType>
Node<DataType>* LinkedList<DataType>::getTail(){
    node* tail = head;
    while (tail->next != nullptr){
        tail = tail->next;
    }
    return tail;
}

template<class DataType>
void LinkedList<DataType>::pushBack(DataType data){   
    if(size){
        node* tail = getTail();
        tail->next = new node(data);
    }
    else
        head = new node(data);
    ++size;
}

template<class DataType>
void LinkedList<DataType>::insert(DataType data, int16_t index_raw){
    uint16_t index;
    index < 0 ? index = size + index_raw : index = index_raw;
    node* current = head;
    node* previous = nullptr;
    if(index){
        for (size_t i = 0; i <= index; i++)
        {
            if(index == i){
                previous->next = new node(data);
                previous->next->next = current;
                break;
            }
            else if(current->next == nullptr){
                current->next = new node(data);
                break;
            }
            previous = current;
            current = current->next;
        }
        ++size;
    }
    else{
        head = new node(data);
        head->next = current;
        ++size;
    }
}

template<class DataType>
void LinkedList<DataType>::remove(int16_t index_raw){
    uint16_t index;
    node* current = head;
    node* previous = nullptr;
    index < 0 ? index = size + index_raw : index = index_raw;
    if(index){
        for (size_t i = 0; i <= index; i++)
        {
            if(i == index){
                previous->next = current->next;
                delete current;
                break;
            }
            else if(!current->next){
                previous->next = nullptr;
                delete current;
                break;
            }
        }
        --size;
    }
    else{
        head = current->next;
        delete current;
        --size;
    }
}

template<class DataType>
DataType LinkedList<DataType>::operator[](int16_t index_raw){
    uint16_t index;
    node* current = head;
    index_raw < 0 ? index = size + index_raw : index = index_raw;
    if(index){
        for (size_t i = 0; i <= index; i++)
        {
            if(index == i)
                return current->data;
            else if(!current->next)
                return current->data;
            current = current->next;
        }
        
    }
    else{
        return head->data;
    }
}

template<class DataType>
uint16_t LinkedList<DataType>::getSize(){
    return size;
}



template<class DataEntry>
DataEntry Queue<DataEntry>::peek(){
    if(content.getSize() > 0)
        return content[0];
    else{
        return NULL;
    }
}

template<class DataEntry>
DataEntry Queue<DataEntry>::pop(){
    if(content.getSize() > 0){
        DataEntry result = content[0];
        content.remove(0);
        return result;
    }
    else{
        return NULL;
    }
}

template<class DataEntry>
void Queue<DataEntry>::add(DataEntry data){
    content.pushBack(data);
}

template<class DataEntry>
uint16_t Queue<DataEntry>::getSize(){
    return content.getSize();    
} 

#endif