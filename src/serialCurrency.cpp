//==================================================================//
/// Arvore B em disco para a disciplina de Tecnicas de Programacao ///
/// IFMG Bambui - Engenharia de Computacao 2023                    ///
/// Alunos:                                                        ///
/// 0034077 - Felipe de Freitas Silva                              ///
/// 0036703 - Jean Gustavo Ferreira Rezende                        ///
//==================================================================//

#include "serialCurrency.h"

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
}

double serialCurrency::getValue() const
{
    return this->value;
}
//converte o numero em string mantendo 2 casas decimais pos virgula
string serialCurrency::getString() const
{ //utilizado no print para exibir os numeros de forma correta
    string str = to_string(this->getValue());
    str = str.substr(0, str.find(".") + 3);
    return str;
}

string serialCurrency::toString() //retorna o valor em string
{
    return string(reinterpret_cast<char*>(&value), sizeof(value));
}

void serialCurrency::fromString(string repr) //copia o valor de um string e converte para double
{
    int pos = 0;
    repr.copy(reinterpret_cast<char*>(&this->value), sizeof(this->value), pos);
}

unsigned long long int serialCurrency::size() const //retorna o tamanho do value
{
    return sizeof(this->value);
}
