#ifndef CIRCULARLINKEDLIST_H
#define CIRCULARLINKEDLIST_H

#include "IList.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace std;

template<class T>
class CircularLinkedList : public IList<T> {
public:
    class Node;

protected:
    Node* head;
    Node* tail;
    int count;
    bool (*itemEqual)(T& lhs, T& rhs);
    void (*deleteUserData)(CircularLinkedList<T>*);

public:
    CircularLinkedList(
        void (*deleteUserData)(CircularLinkedList<T>*) = 0,
        bool (*itemEqual)(T&, T&) = 0
    )
        : head(nullptr), tail(nullptr), count(0),
          itemEqual(itemEqual), deleteUserData(deleteUserData) {}

    CircularLinkedList(const CircularLinkedList<T>& list)
        : head(nullptr), tail(nullptr), count(0),
          itemEqual(list.itemEqual), deleteUserData(list.deleteUserData) {
        copyFrom(list);
    }

    CircularLinkedList<T>& operator=(const CircularLinkedList<T>& list) {
        if (this == &list) return *this;
        removeInternalData();
        itemEqual = list.itemEqual;
        deleteUserData = list.deleteUserData;
        copyFrom(list);
        return *this;
    }

    ~CircularLinkedList() {
        removeInternalData();
    }

    void add(T e) override {
        Node* newTail=new Node(e);
        if (count==0) {
            head=tail=newTail;
            newTail->next=newTail;
        }
        else {
            newTail->next=head;
            tail->next=newTail;
            tail=newTail;
        }
        count++;
    }

    void add(int index, T e) override {
        if (index<0 || index>count) throw std::out_of_range("Index out of range");
        Node** pp=&(head);
        if (index==count && count>0) {
        pp=&(tail->next);
        }
        else {
            for (int i=0; i<index; i++) {
                pp=&((*pp)->next);
            }
        }
        Node* temp=new Node(e);
        temp->next=(*pp);
        (*pp)=temp;
        if (count==0) {head=temp;tail=head;}
        if (index==count) {tail=temp;}
        tail->next=head;
        count++;
    }

    T removeAt(int index) override {
        if (index<0 || index>=count) throw std::out_of_range("Index out of range");
        Node* prev=tail;
        Node** pp=&head;
        for (int i=0; i<index; i++) {
            prev = *pp;
            pp=&((*pp)->next);
        }
        Node* temp=*pp;
        T value=temp->data;
        if (count==1) {delete temp; head=tail=nullptr; count--; return value;}
        if (temp==tail) {
            tail=prev;
        }
        (*pp)=temp->next;
        delete temp;
        count--;
        tail->next=head;
        return value;
    }

    bool removeItem(T item, void (*removeItemData)(T) = 0) override {
        if (count==0) return false;
        bool check=false;
        if (count==1) check=true;
        Node* temp=head;
        for (int i=0; i<count; i++) {
            if (itemEqual && itemEqual(item, temp->data)) {
                if (removeItemData) {
                    removeItemData(temp->data);
                }
                removeAt(i);
                if (check) tail=head;
                return true;
            }
            else if (!itemEqual && temp->data==item) {
                if (removeItemData) {
                    removeItemData(temp->data);
                }
                removeAt(i);
                if (check) tail=head;
                return true;
            }
            temp=temp->next;
        }
        return false;
    }

    void clear() override {
        removeInternalData();

    }

    T& get(int index) override {
        Node* temp=head;
        if (index<0 || index>=count) throw std::out_of_range("Index out of range");
        for (int i=0; i<index; i++) {
            temp=temp->next;
        }
        return temp->data;
    }

    int indexOf(T item) override {
        Node* temp=head;
        for (int i=0; i<count; i++) {
            if (itemEqual && itemEqual(temp->data, item)) return i;
            else if (!itemEqual && temp->data==item) return i;
            temp=temp->next;
        }
        return -1;
    }

    bool empty() override { return count == 0; }
    int size() override { return count; }
    bool contains(T item) override { return indexOf(item) >= 0; }

    string toString(string (*item2str)(T&) = 0) override {
        stringstream ss;
        ss << "[";
        Node* cur = head;
        for (int i = 0; i < count; ++i) {
            if (i > 0) ss << ", ";
            if (item2str) ss << item2str(cur->data);
            else ss << cur->data;
            cur = cur->next;
        }
        ss << "]";
        return ss.str();
    }

    void println(string (*item2str)(T&) = 0) {
        cout << toString(item2str) << endl;
    }

protected:
    static bool equals(T& lhs, T& rhs, bool (*itemEqual)(T&, T&)) {
        return itemEqual ? itemEqual(lhs, rhs) : (lhs == rhs);
    }

    void copyFrom(const CircularLinkedList<T>& list) {
        Node* cur = list.head;
        for (int i = 0; i < list.count; ++i) {
            add(cur->data);
            cur = cur->next;
        }
    }

    void removeInternalData() {
        if (deleteUserData != 0 && count > 0) deleteUserData(this);
        Node* cur = head;
        for (int i = 0; i < count; ++i) {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        head = tail = nullptr;
        count = 0;
    }

public:
    class Node {
    public:
        T data;
        Node* next;
        Node(T data, Node* next = nullptr) : data(data), next(next) {}
    };
};

#endif
