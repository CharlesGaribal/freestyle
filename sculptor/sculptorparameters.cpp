#include "sculptorparameters.h"

#include <math.h>

SculptorParameters::SculptorParameters(double minEdgeLength, double maxEdgeLength, double dmove, double dthickness) {
    if (validMinMaxEdge(minEdgeLength, maxEdgeLength) && validLemme(dmove, dthickness, maxEdgeLength)) {
        this->minEdgeLength = minEdgeLength;
        this->maxEdgeLength = maxEdgeLength;
        dMove = dmove;
        dThickness = dthickness;
    } else {
        // Invalid values
        this->minEdgeLength = 0;
        this->maxEdgeLength = 0;
        dMove = 0;
        dThickness = 0;
    }
}

double SculptorParameters::getMinEdgeLength() const {
    return minEdgeLength;
}

void SculptorParameters::setMinEdgeLength(double length) {
    if (validMinMaxEdge(length, maxEdgeLength))
        minEdgeLength = length;
}

double SculptorParameters::getMaxEdgeLength() const {
    return maxEdgeLength;
}

void SculptorParameters::setMaxEdgeLength(double length) {
    if (validMinMaxEdge(minEdgeLength, length) && validLemme(dMove, dThickness, length))
        maxEdgeLength = length;
}

double SculptorParameters::getDMove() const {
    return dMove;
}

void SculptorParameters::setDMove(double value) {
    if (validLemme(value, dThickness, maxEdgeLength))
        dMove = value;
}

double SculptorParameters::getDThickness() const {
    return dThickness;
}

void SculptorParameters::setDThickness(double value) {
    if (validLemme(dMove, value, maxEdgeLength))
        dThickness = value;
}

bool SculptorParameters::valid() {
    return validMinMaxEdge(minEdgeLength, maxEdgeLength) && validLemme(dMove, dThickness, maxEdgeLength);
}

bool SculptorParameters::validMinMaxEdge(double min, double max) {
    return min > 0 && min <= max/2.;
}

bool SculptorParameters::validLemme(double dmove, double dthickness, double ddetail) {
    //return dmove <= (dthickness - ddetail/sqrt(3)) / 2.;
    return 4.*dmove*dmove <= dthickness*dthickness - (ddetail*ddetail)/3.;
}

