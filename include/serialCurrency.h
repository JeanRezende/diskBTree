//==================================================================//
/// Arvore B em disco para a disciplina de Tecnicas de Programacao ///
/// IFMG Bambui - Engenharia de Computacao 2023                    ///
/// Alunos:                                                        ///
/// 0034077 - Felipe de Freitas Silva                              ///
/// 0036703 - Jean Gustavo Ferreira Rezende                        ///
//==================================================================//

#ifndef serialCurrency_H
#define serialCurrency_H

#include "serializable.h"
#include <math.h>

class serialCurrency : serializable
{
public:
    serialCurrency();
    serialCurrency(double value);
    serialCurrency(const serialCurrency& other);
    virtual ~serialCurrency();
    serialCurrency operator=(const serialCurrency& other);
    bool operator==(const serialCurrency &other) const;
    bool operator<(const serialCurrency &other) const;
    bool operator<=(const serialCurrency &other) const;
    bool operator>(const serialCurrency &other) const;
    bool operator>=(const serialCurrency &other) const;
    bool operator!=(const serialCurrency &other) const;
    serialCurrency operator+(const serialCurrency& other) const;
    serialCurrency operator-(const serialCurrency& other) const;
    serialCurrency operator*(const serialCurrency& other) const;
    serialCurrency operator/(const serialCurrency& other) const;
    string getString() const;
    void operator++();
    void operator--();
    void setValue(double value);
    double getValue() const;
    virtual string toString();
    virtual void fromString(string repr);
    virtual string toXML() {};
    virtual void fromXML(string repr) {};
    virtual string toCSV() {};
    virtual void fromCSV(string repr) {};
    virtual string toJSON() {};
    virtual void fromJSON(string repr) {};
    virtual unsigned long long int size() const;

protected:
    double value;
};
#endif // serialCurrency_H
