#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <vector>
#include "serializable.h"

using namespace std;


template <class T>
class record : public serializable {
   static_assert(is_base_of<serializable, T>::value, "T must be serializable");
   public:

    const unsigned int MIN = 2;
    const unsigned int MAX = 5;

      record();
      record(T d);
      record(const record<T> &other);
      virtual ~record();
      record<T> operator=(const record<T> &other);
      vector<T> getData() const;
      void setData(T d);
      unsigned long long int getNext() const;
      void setNext(unsigned long long int n);
      bool isDeleted() const;
      void del();
      void undel();

      unsigned int getLenght();
      void setLenght(unsigned int l);

      T getKey(unsigned int i);
      void setKey(unsigned int i, T key);

      unsigned int getChildren(unsigned int i);
      void setChildren(unsigned int i, unsigned int diskAdress);

      bool isLeaf();
      void setLeaf(bool leaf);


      virtual string toString();
      virtual void fromString(string repr);
      virtual string toCSV() { return ""; }
      virtual void fromCSV(string repr) {}
      virtual string toJSON() { return ""; }
      virtual void fromJSON(string repr) {}
      virtual string toXML() { return ""; }
      virtual void fromXML(string repr) {}
      virtual unsigned long long int size() const;
   protected:
        vector<T> keys;
        vector<unsigned int> children;
        bool leaf;
        unsigned int l;
        bool deleted;
        unsigned long long int next;
};

template <class T>
record<T>::record() : serializable() {

    this->deleted = false;
    this->leaf = true;
    this->l = 0;

    this->keys.resize(MAX);
    this->children.resize(MAX+1);

    for(unsigned int i = 0; i < children.size(); i++){
        this->children[i] = -1;
    }
}


template <class T>
record<T>::record(const record<T> &other){

    this->leaf = other.leaf;
    this->l= other.l;
    this->deleted = other.deleted;

    keys.resize(MAX);
    children.resize(MAX + 1);

    this->children = other.children;
    this->keys = other.keys;

}

template <class T>
record<T>::~record() {}

template <class T>
record<T> record<T>::operator=(const record<T> &other) {

    if(this == &other) return *this;

    record<T> aux(other);

    this->children = other.children;
    this->keys = other.keys;
    this->leaf = other.leaf;
    this->l = other.l;
    this->deleted = other.deleted;

    return aux;
}

template <class T>
bool record<T>::isDeleted() const {
    return this->deleted;
}

template <class T>
void record<T>::del() {
    this->deleted = true;
}

template <class T>
void record<T>::undel() {
    this->deleted = false;
}

template <class T>
string record<T>::toString() {
    string aux = "";
    aux += string(reinterpret_cast<char*>(&leaf),sizeof(leaf));
    aux += string(reinterpret_cast<char*>(&deleted),sizeof(deleted));
    aux += string(reinterpret_cast<char*>(&l),sizeof(l));

    for(int i=0; i< keys.size(); i++){
        aux += keys[i].toString();
    }

    for(int i=0; i< children.size(); i++){
        aux += string(reinterpret_cast<char*>(&this->children[i]), sizeof(this->children[i]));
    }
    return aux;
}

template <class T>
void record<T>::fromString(string repr) {
    int pos = 0;
    repr.copy(reinterpret_cast<char*>(&this->leaf), sizeof(this->leaf), pos);
    pos += sizeof(this->leaf);
    repr.copy(reinterpret_cast<char*>(&this->deleted), sizeof(this->deleted), pos);
    pos += sizeof(this->deleted);
    repr.copy(reinterpret_cast<char*>(&this->l),sizeof(this->l), pos);
    pos += sizeof(this->l);

    string  aux = "";
    for(unsigned int i = 0; i < keys.size(); i++){
        aux = repr.substr(pos, keys[i].size());
        keys[i].fromString(aux);
        pos += keys[i].size();
        aux = "";
    }
    for( unsigned int i = 0; i < children.size(); i++){
        repr.copy(reinterpret_cast<char*>(&this->children[i]), sizeof(this->children[i]), pos);
        pos += sizeof(this->children[i]);
    }
}

template <class T>
vector<T> record<T>::getData() const {
    return this->keys;
}

template <class T>
unsigned long long int record<T>::size() const {

    return keys.size()*keys[0].size() + children.size()*sizeof(children[0]) + sizeof(leaf) + sizeof(l) + sizeof(deleted);
}

template <class T>
unsigned long long int record<T>::getNext() const {
    return this->next;
}

template <class T>
void record<T>::setNext(unsigned long long int n) {
    this->next = n;
}

template <class T>
unsigned int record<T>::getLenght() {
    return this->l;
}

template <class T>
void record<T>::setLenght(unsigned int l) {
    this->l = l;
}

template <class T>
T record<T>::getKey(unsigned int i) {
    return this->keys[i];
}

template <class T>
void record<T>::setKey(unsigned int i , T key) {
    this->keys[i] = key;
}

template <class T>
unsigned int record<T>::getChildren(unsigned int i) {
    return this->children[i];
}

template <class T>
void record<T>::setChildren(unsigned int i , unsigned int diskAdress) {
    this->children[i] = diskAdress;
}

template <class T>
bool record<T>::isLeaf() {
    return this->leaf;
}

template <class T>
void record<T>::setLeaf(bool leaf) {
    this->leaf = leaf;
}
#endif // RECORD_H
