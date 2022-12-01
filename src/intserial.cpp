#include "intserial.h"

intSerial::intSerial() : serializable() {
    this->value = 0;
}

intSerial::intSerial(int i) : serializable() {
    this->value = i;
}

intSerial::intSerial(const intSerial& other) {
    this->value = other.getValue();
}

intSerial::~intSerial() {}

intSerial intSerial::operator=(const intSerial& other) {
    //funciona
    if(this != &other)
    {
        this->value = other.value;
        return *this;
    }
    return *this;
}

bool intSerial::operator==(const intSerial &other) const {
    return this->value == other.getValue();
}

bool intSerial::operator<(const intSerial &other) const {
    return this->value < other.getValue();
}

bool intSerial::operator<=(const intSerial &other) const {
    return this->value <= other.getValue();
}

bool intSerial::operator>(const intSerial &other) const {
    return this->value > other.getValue();
}

bool intSerial::operator>=(const intSerial &other) const {
    return this->value >= other.getValue();
}

bool intSerial::operator!=(const intSerial &other) const {
    return this->value != other.getValue();
}

intSerial intSerial::operator+(const intSerial& other) const {
    intSerial aux; //cria variavel auxiliar que recebe a soma
    aux.setValue(this->value + other.getValue()); //acesso ao valor da instancia + get value do other
    return aux; //retorna a soma
}

intSerial intSerial::operator-(const intSerial& other) const {
    intSerial aux; //cria variavel auxiliar que recebe a subtracao
    aux.setValue(this->value - other.getValue()); //acesso ao valor da instancia - get value do other
    return aux; //retorna a subtracao
}

intSerial intSerial::operator*(const intSerial& other) const {
    intSerial aux; //cria variavel auxiliar que recebe a multiplicacao
    aux.setValue(this->value * other.getValue()); //acesso ao valor da instancia * get value do other
    return aux; //retorna a multiplicacao
}

intSerial intSerial::operator/(const intSerial& other) const {
    intSerial aux; //cria variavel auxiliar que recebe a divisao
    aux.setValue(this->value / other.getValue()); //acesso ao valor da instancia / get value do other
    return aux; //retorna a divisao
}

void intSerial::operator++() {
    this->value++;
}

void intSerial::operator--() {
    this->value--;
}

void intSerial::setValue(int v) {
    this->value = v;
}

int intSerial::getValue() const {
    return this->value;
}

string intSerial::toString() {
    //utiliza o reinterpret cast para temporariamente ver a posicao de memoria
    //como char para uma conversao segura para string
    return string(reinterpret_cast<char*>(&value), sizeof(value));
}

void intSerial::fromString(string repr) {
    //utiliza o reinterpret cast para temporariamente ver a posicao de memoria
    //como char para uma conversao segura para int
    repr.copy(reinterpret_cast<char*>(&value), sizeof(value));
}

string intSerial::toXML() {

}

void intSerial::fromXML(string repr) {

}

string intSerial::toCSV() {

}

void intSerial::fromCSV(string repr) {

}

string intSerial::toJSON() {

}

void intSerial::fromJSON(string repr) {

}

unsigned long long int intSerial::size() const {
    return sizeof(this->value);
}

