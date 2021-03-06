#ifndef LA_MIN_ELEMENT_VALUE_H
#define LA_MIN_ELEMENT_VALUE_H

#include "Object.h"
#include "Handle.h"


// By Binhang, May 2017

// This class is designed for finding the maximal element by aggregation;
using namespace pdb;

class LAMinElementValueType : public Object {

private:
    double value = 100000;

    int rowIndex = -1;
    int colIndex = -1;

public:
    ENABLE_DEEP_COPY

    ~LAMinElementValueType() {}
    LAMinElementValueType() {}

    void setValue(double v) {
        value = v;
    }

    void setRowIndex(int rI) {
        rowIndex = rI;
    }

    void setColIndex(int cI) {
        colIndex = cI;
    }

    double getValue() {
        return value;
    }

    int getRowIndex() {
        return rowIndex;
    }

    int getColIndex() {
        return colIndex;
    }

    LAMinElementValueType& operator+(LAMinElementValueType& other) {
        if (value > other.value) {
            value = other.value;
            rowIndex = other.rowIndex;
            colIndex = other.colIndex;
        }
        return *this;
    }

    size_t hash() const override{
        return 10000 * rowIndex + colIndex;
    }

};

#endif
