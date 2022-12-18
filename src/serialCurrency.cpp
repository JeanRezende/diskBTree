#include "serialCurrency.h"
#include "math.h"
//remover
#include <iostream>
using namespace std;

serialCurrency::serialCurrency() : serializable() {
    this->inteiro = 0;
    this->decimal = 0;
}

serialCurrency::serialCurrency(double value) : serializable() {
    this->setValue(value);
}

serialCurrency::serialCurrency(const serialCurrency& other) {
    double value = other.getValue();
    this->inteiro = (int) value;
    this->decimal = (int)((value - this->inteiro)*100);
    //overflowDecimal();
}
//função que adiciona um inteiro caso o decimal for maior que 100
void serialCurrency::overflowDecimal(){
    if(this->decimal % 100 != 0){
        this->inteiro += this->decimal / 100;
        this->decimal = this->decimal % 100;
        //cout << "inteiro " << inteiro << " decimal " << decimal << endl;
    }
}

serialCurrency::~serialCurrency() {}

serialCurrency serialCurrency::operator=(const serialCurrency& other) {
    //funciona
    if(this != &other)
    {
        double value = other.getValue();
        this->inteiro = (int) value;
        this->decimal = (int)((value - this->inteiro)*100);
        return *this;
    }
    return *this;
}

bool serialCurrency::operator==(const serialCurrency &other) const {
    return this->getValue() == other.getValue();
}

bool serialCurrency::operator<(const serialCurrency &other) const {
    return this->getValue()< other.getValue();
}

bool serialCurrency::operator<=(const serialCurrency &other) const {
    return this->getValue() <= other.getValue();
}

bool serialCurrency::operator>(const serialCurrency &other) const {
    return this->getValue() > other.getValue();
}

bool serialCurrency::operator>=(const serialCurrency &other) const {
    return this->getValue() >= other.getValue();
}

bool serialCurrency::operator!=(const serialCurrency &other) const {
    return this->getValue() != other.getValue();
}

serialCurrency serialCurrency::operator+(const serialCurrency& other) const {
    serialCurrency aux; //cria variavel auxiliar que recebe a soma
    aux.setValue(this->getValue() + other.getValue()); //acesso ao valor da instancia + get value do other
    return aux; //retorna a soma
}

serialCurrency serialCurrency::operator-(const serialCurrency& other) const {
    serialCurrency aux; //cria variavel auxiliar que recebe a subtracao
    aux.setValue(this->getValue() - other.getValue()); //acesso ao valor da instancia - get value do other
    return aux; //retorna a subtracao
}

serialCurrency serialCurrency::operator*(const serialCurrency& other) const {
    serialCurrency aux; //cria variavel auxiliar que recebe a multiplicacao
    aux.setValue(this->getValue() * other.getValue()); //acesso ao valor da instancia * get value do other
    return aux; //retorna a multiplicacao
}

serialCurrency serialCurrency::operator/(const serialCurrency& other) const {
    serialCurrency aux; //cria variavel auxiliar que recebe a divisao
    aux.setValue(this->getValue() / other.getValue()); //acesso ao valor da instancia / get value do other
    return aux; //retorna a divisao
}

void serialCurrency::operator++() {
    this->inteiro++;
}

void serialCurrency::operator--() {
    this->inteiro--;
}

void serialCurrency::setValue(double value) {
    //parte inteira recebe a conversao de value
    this->inteiro = (int) (value);
    //parte decimal vira inteiro
    this->decimal = (int)((value - this->inteiro)*100);
    //overflowDecimal();
    cout << "inteiro " << inteiro << " decimal " << decimal << endl;
}

double serialCurrency::getValue() const {
    double sum = this->inteiro + (double(this->decimal)/100.00);
    return sum;
}


string serialCurrency::toString() {
    string aux = "";
    aux += string(reinterpret_cast<char*>(&inteiro), sizeof(inteiro));
//    aux += ".";
    aux += string(reinterpret_cast<char*>(&decimal), sizeof(decimal));
    return aux;
}

void serialCurrency::fromString(string repr) {
    int pos = 0;
    repr.copy(reinterpret_cast<char*>(&this->inteiro), sizeof(this->inteiro), pos);
    pos += sizeof(this->inteiro);
    repr.copy(reinterpret_cast<char*>(&this->decimal), sizeof(this->decimal), pos);
    pos += sizeof(this->decimal);
}

string serialCurrency::toXML() {

}

void serialCurrency::fromXML(string repr) {

}

string serialCurrency::toCSV() {

}

void serialCurrency::fromCSV(string repr) {

}

string serialCurrency::toJSON() {

}

void serialCurrency::fromJSON(string repr) {

}

unsigned long long int serialCurrency::size() const {
    return sizeof(this->inteiro) + sizeof(this->decimal);
}

