#include "serialCurrency.h"
#include "math.h"

//remover
#include <iostream>
using namespace std;

serialCurrency::serialCurrency() : serializable()
{
    this->value = 0.0;
}

serialCurrency::serialCurrency(double value) : serializable()
{
    this->setValue(value);
}

serialCurrency::serialCurrency(const serialCurrency& other)
{
    this->value = other.getValue();
    //overflowDecimal();
}
//função que adiciona um inteiro caso o decimal for maior que 100
void serialCurrency::overflowDecimal()
{

}

serialCurrency::~serialCurrency() {}

serialCurrency serialCurrency::operator=(const serialCurrency& other)
{
    //funciona
    if(this != &other)
    {
        this->value = other.getValue();
        return *this;
    }
    return *this;
}

bool serialCurrency::operator==(const serialCurrency &other) const
{
    return this->getValue() == other.getValue();
}

bool serialCurrency::operator<(const serialCurrency &other) const
{
    return this->getValue()< other.getValue();
}

bool serialCurrency::operator<=(const serialCurrency &other) const
{
    return this->getValue() <= other.getValue();
}

bool serialCurrency::operator>(const serialCurrency &other) const
{
    return this->getValue() > other.getValue();
}

bool serialCurrency::operator>=(const serialCurrency &other) const
{
    return this->getValue() >= other.getValue();
}

bool serialCurrency::operator!=(const serialCurrency &other) const
{
    return this->getValue() != other.getValue();
}

serialCurrency serialCurrency::operator+(const serialCurrency& other) const
{
    serialCurrency aux; //cria variavel auxiliar que recebe a soma
    aux.setValue(this->getValue() + other.getValue()); //acesso ao valor da instancia + get value do other
    return aux; //retorna a soma
}

serialCurrency serialCurrency::operator-(const serialCurrency& other) const
{
    serialCurrency aux; //cria variavel auxiliar que recebe a subtracao
    aux.setValue(this->getValue() - other.getValue()); //acesso ao valor da instancia - get value do other
    return aux; //retorna a subtracao
}

serialCurrency serialCurrency::operator*(const serialCurrency& other) const
{
    serialCurrency aux; //cria variavel auxiliar que recebe a multiplicacao
    aux.setValue(this->getValue() * other.getValue()); //acesso ao valor da instancia * get value do other
    return aux; //retorna a multiplicacao
}

serialCurrency serialCurrency::operator/(const serialCurrency& other) const
{
    serialCurrency aux; //cria variavel auxiliar que recebe a divisao
    aux.setValue(this->getValue() / other.getValue()); //acesso ao valor da instancia / get value do other
    return aux; //retorna a divisao
}

void serialCurrency::operator++()
{
    this->value++;
}

void serialCurrency::operator--()
{
    this->value--;
}

void serialCurrency::setValue(double value)
{
    this->value = value;
    //overflowDecimal();
    cout << "SET VALUE  : " << this->value << endl;
}

double serialCurrency::getValue() const
{
    return this->value;
}

string serialCurrency::getString() const
{
    string str = to_string(this->getValue());
    str = str.substr(0, str.find(".") + 3);
    return str;
}


string serialCurrency::toString()
{
    return string(reinterpret_cast<char*>(&value), sizeof(value));
}

void serialCurrency::fromString(string repr)
{
    int pos = 0;
    repr.copy(reinterpret_cast<char*>(&this->value), sizeof(this->value), pos);
}

string serialCurrency::toXML()
{

}

void serialCurrency::fromXML(string repr)
{

}

string serialCurrency::toCSV()
{

}

void serialCurrency::fromCSV(string repr)
{

}

string serialCurrency::toJSON()
{

}

void serialCurrency::fromJSON(string repr)
{

}

unsigned long long int serialCurrency::size() const
{
    return sizeof(this->value);
}
