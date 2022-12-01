#ifndef RECORD_H
#define RECORD_H

#include <string>
#include "serializable.h"

using namespace std;

template <class T>
class record : public serializable {
   static_assert(is_base_of<serializable, T>::value, "T must be serializable");
   public:
      record();
      record(T d);
      record(const record<T> &other);
      virtual ~record();
      record<T> operator=(const record<T> &other);
      bool operator==(const record<T> &other);
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
      T data;
      bool deleted;
      unsigned long long int next;
};

template <class T>
record<T>::record() : serializable() {
    this->deleted = false;
    this->next = 0;
}

template <class T>
record<T>::record(T d) : serializable() {
    this->data = d;
    this->deleted = false;
    this->next = 0;
}

template <class T>
record<T>::record(const record<T> &other) {
    this->data = other.getData();
    this->deleted = other.isDeleted();
    this->next = other.getNext();
}

template <class T>
record<T>::~record() {}

template <class T>
record<T> record<T>::operator=(const record<T> &other) {

    if(this == &other) return *this;

    record<T> aux(other);

    this->data = other.getData();
    this->next = other.getNext();
    this->deleted = other.isDeleted();

    return aux;
}

template <class T>
bool record<T>::operator==(const record<T> &other) {
    //data e next devem ser iguais para comparacao ser verdadeira
    return this->data == other.getData() && this->next == other.getNext();
}

template <class T>
T record<T>::getData() const {
    return this->data;
}

template <class T>
void record<T>::setData(T d) {
    this->data = d;
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
    aux += string(reinterpret_cast<char*>(&data),sizeof(data));
    aux += string(reinterpret_cast<char*>(&deleted),sizeof(deleted));
    aux += string(reinterpret_cast<char*>(&next),sizeof(next));
    return aux;
}

template <class T>
void record<T>::fromString(string repr) {
    int pos = 0;
    repr.copy(reinterpret_cast<char*>(&data),sizeof(data),pos);
    pos += sizeof(data);
    repr.copy(reinterpret_cast<char*>(&deleted),sizeof(deleted),pos);
    pos += sizeof(deleted);
    repr.copy(reinterpret_cast<char*>(&next),sizeof(next),pos);
}

template <class T>
unsigned long long int record<T>::size() const {
    return sizeof(data) + sizeof(deleted) + sizeof(next);
}

#endif // RECORD_H
