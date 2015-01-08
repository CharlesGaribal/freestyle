#ifndef OPERATOR_H
#define OPERATOR_H

#include "quasiuniformmesh.h"
class Sculptor;

class Operator
{
public:
    class AdditionnalParameters {

    };

    enum ETopologicalChange {NONE, GENUS};

    Operator(Sculptor *sculptor);

    virtual void operator()(AdditionnalParameters params) = 0;
    virtual ETopologicalChange getTopologicalChange() = 0;

    void setSculptor(Sculptor *sculptor) {
        this->sculptor = sculptor;
    }

private:
    Sculptor *sculptor;
};

#endif // OPERATOR_H
