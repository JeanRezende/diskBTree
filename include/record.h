#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <vector>
#include "serializable.h"

using namespace std;
const int MAX = 5;
const int MIN = 2;

template <class T>
class record : public serializable {
   static_assert(is_base_of<serializable, T>::value, "T must be serializable");
   public:
      record();
      record(T d);
      record(bool leaf);
      record(const record<T> &other);
      virtual ~record();
      record<T> operator=(const record<T> &other);
      T getData() const;
      void setData(T d);
      unsigned long long int getNext() const;
      void setNext(unsigned long long int n);
      bool isDeleted() const;
      void del();
      void undel();
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

    for(int i = 0; i < children.size(); i++){
        this->children[i] = -1;
    }
}

template <class T>
record<T>::record(bool leaf) : serializable() {

    this->deleted = false;
    this->leaf = leaf;
    this->l = 0;

    this->keys.resize(MAX);
    this->children.resize(MAX+1);

    for(int i = 0; i < children.size(); i++){
        this->children[i] = -1;
    }
}

template <class T>
record<T>::record(const record<T> &other){

    this->leaf = other.leaf;
    this->l= other.l;

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
    repr.copy(reinterpret_cast<char*>(&leaf),sizeof(leaf),pos);
    pos += sizeof(leaf);
    repr.copy(reinterpret_cast<char*>(&deleted),sizeof(deleted),pos);
    pos += sizeof(deleted);

    string  aux = "";
    for(int i = 0; i < keys.size(); i++){
        aux = repr.substr(pos, keys[i].size());
        keys[i].fromString(aux);
        pos += keys[i].size();
        aux = "";
    }
    for( int i = 0; i < children.size(); i++){
        repr.copy(reinterpret_cast<char*>(&this->children[i]), sizeof(this->children[i]), pos);
        pos += sizeof(this->children[i]);
    }
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


#endif // RECORD_H