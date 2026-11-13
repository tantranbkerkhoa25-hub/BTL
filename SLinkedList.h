#ifndef SLINKEDLIST_H
#define SLINKEDLIST_H

#include "IList.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace std;

template<class T>
class SLinkedList : public IList<T> {
public:
    class Iterator;
    class Node;

protected:
    Node* head;
    Node* tail;
    int count;
    bool (*itemEqual)(T& lhs, T& rhs);
    void (*deleteUserData)(SLinkedList<T>*);

public:
    SLinkedList(
        void (*deleteUserData)(SLinkedList<T>*) = 0,
        bool (*itemEqual)(T&, T&) = 0
    ) {
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->next = head;
        count = 0;
        this->itemEqual = itemEqual;
        this->deleteUserData = deleteUserData;
    }//Default constructor

    SLinkedList(const SLinkedList<T>& list) {
        head = new Node();
        tail = new Node();
        head->next = tail;
        tail->next = head;
        count = 0;
        itemEqual = list.itemEqual;
        deleteUserData = list.deleteUserData;
        copyFrom(list);
    }//Normal constructor

    SLinkedList<T>& operator=(const SLinkedList<T>& list) {
        if (this == &list) return *this;
        removeInternalData();
        head->next = tail;
        tail->next = head;
        count = 0;
        itemEqual = list.itemEqual;
        deleteUserData = list.deleteUserData;
        copyFrom(list);
        return *this;
    }

    ~SLinkedList() {
        removeInternalData();
        delete head;
        delete tail;
    }

    void add(T e) override {
        Node* newTail=new Node();
        tail->data=e;
        tail->next=newTail;
        newTail->next=head;
        tail=newTail;
        count++;
    }

    void add(int index, T e) override {
        if (index<0 || index>count) throw std::out_of_range("Index out of range");
        Node** pp=&(head->next);
        for (int i=0; i<index; i++) {
            pp=&((*pp)->next);
        }
        Node* temp=new Node(e);
        temp->next=(*pp);
        (*pp)=temp;
        count++;
    }

    T removeAt(int index) override {
        if (index<0 || index>=count) throw std::out_of_range("Index out of range");
        Node** pp=&(head->next);
        for (int i=0; i<index; i++) {
            pp=&((*pp)->next);
        }
        Node* temp=(*pp);
        (*pp)=temp->next;
        T value=temp->data;
        delete temp;
        count--;
        return value;
    }

    bool removeItem(T item, void (*removeItemData)(T) = 0) override {
        Node**pp=&(head->next);bool check=false;int idx=0;
        if (removeItemData) check=true;
        for (int i=0; i<count; i++) {
            if (itemEqual!=nullptr) {
                if (itemEqual((*pp)->data,item)) {
                    if (check) {
                        removeItemData((*pp)->data);
                    }
                    removeAt(idx);
                    return true;
                }
            }
            else {
                if (item==(*pp)->data) {
                    if (check) removeItemData((*pp)->data);
                    removeAt(idx);
                    return true;
                }
            }
            idx++;
            pp=&((*pp)->next);
        }
        return false;
    }

    void clear() override {
        removeInternalData();

        head->next=tail;
        count=0;
    }

    T& get(int index) override {
        if (index<0 || index >=count) throw std::out_of_range("Index out of range");
        Node* temp=head->next;
        for (int i=1; i<=index; i++) {
            temp=temp->next;
        }
        return temp->data;
    }

    int indexOf(T item) override {
        Node* temp=head->next;
        int idx=-1;
        for (int i=0; i<count; i++) {
            if (itemEqual!=nullptr) {
                idx=(itemEqual(temp->data, item))?i:idx;
            }
            else {
                idx=(item==temp->data)?i:idx;
            }
            if (idx!=-1) break;
            temp=temp->next;
        }
        return idx;
    }

    bool empty() override { return count == 0; }
    int size() override { return count; }

    bool contains(T item) override {
        return indexOf(item) >= 0;
    }

    string toString(string (*item2str)(T&) = 0) override {
        stringstream ss;
        ss << "[";
        Node* cur = head->next;
        bool first = true;
        while (cur != tail) {
            if (!first) ss << ", ";
            if (item2str) ss << item2str(cur->data);
            else ss << cur->data;
            first = false;
            cur = cur->next;
        }
        ss << "]";
        return ss.str();
    }

    void println(string (*item2str)(T&) = 0) {
        cout << toString(item2str) << endl;
    }

    void setDeleteUserDataPtr(void (*deleteUserData)(SLinkedList<T>*) = 0) {
        this->deleteUserData = deleteUserData;
    }

    Iterator begin() { return Iterator(this, true); }
    Iterator end() { return Iterator(this, false); }

    static void free(SLinkedList<T>* list) {
        Iterator it = list->begin();
        while (it != list->end()) {
            delete *it;
            it++;
        }
    }

protected:
    static bool equals(T& lhs, T& rhs, bool (*itemEqual)(T&, T&)) {
        return itemEqual ? itemEqual(lhs, rhs) : (lhs == rhs);
    }

    void copyFrom(const SLinkedList<T>& list) {
        Node* src = list.head->next;
        while (src != list.tail) {
            add(src->data);
            src = src->next;
        }
    }

    void removeInternalData() {
        if (deleteUserData != 0 && count > 0) deleteUserData(this);
        Node* cur = head->next;
        while (cur != tail) {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
    }

public:
    class Node {
    public:
        T data;
        Node* next;

        Node(Node* next = 0) : next(next) {}
        Node(T data, Node* next = 0) : data(data), next(next) {}
    };

    class Iterator {
    private:
        SLinkedList<T>* pList;
        Node* pNode;

    public:
        Iterator(SLinkedList<T>* pList = 0, bool begin = true) {
            this->pList = pList;
            if (pList == 0) pNode = 0;
            else pNode = begin ? pList->head->next : pList->tail;
        }

        Iterator& operator=(const Iterator& iterator) {
            pNode = iterator.pNode;
            pList = iterator.pList;
            return *this;
        }

        void remove(void (*removeItemData)(T) = 0) {
            if (pList == 0 || pNode == pList->tail) return;

            Node* prev = pList->head;
            int index = 0;
            while (prev->next != pList->tail && prev->next != pNode) {
                prev = prev->next;
                ++index;
            }
            if (prev->next != pNode) return;

            T removed = pList->removeAt(index);
            if (removeItemData != 0) removeItemData(removed);
            pNode = prev;
        }

        T& operator*() { return pNode->data; }
        bool operator!=(const Iterator& iterator) { return pNode != iterator.pNode; }
        Iterator& operator++() { pNode = pNode->next; return *this; }
        Iterator operator++(int) { Iterator old = *this; ++(*this); return old; }
    };
};

#endif
