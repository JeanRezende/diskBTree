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
    void overflowDecimal();
    string getString() const;
    void operator++();
    void operator--();
    void setValue(double value);
    double getValue() const;
    virtual string toString();
    virtual void fromString(string repr);
    virtual string toXML();
    virtual void fromXML(string repr);
    virtual string toCSV();
    virtual void fromCSV(string repr);
    virtual string toJSON();
    virtual void fromJSON(string repr);
    virtual unsigned long long int size() const;

protected:
    int inteiro;
    int decimal;
};
#endif // serialCurrency_H
