#include "sculptorparameters.h"

#include <math.h>

SculptorParameters::SculptorParameters(float minEdgeLength, float maxEdgeLength, float dmove, float dthickness) {
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

float SculptorParameters::getMinEdgeLength() const {
    return minEdgeLength;
}

void SculptorParameters::setMinEdgeLength(float length) {
    //if (validMinMaxEdge(length, maxEdgeLength))
        minEdgeLength = length;
}

float SculptorParameters::getMaxEdgeLength() const {
    return maxEdgeLength;
}

void SculptorParameters::setMaxEdgeLength(float length) {
    //if (validMinMaxEdge(minEdgeLength, length) && validLemme(dMove, dThickness, length))
        maxEdgeLength = length;
}

float SculptorParameters::getDMove() const {
    return dMove;
}

void SculptorParameters::setDMove(float value) {
    //if (validLemme(value, dThickness, maxEdgeLength))
        dMove = value;
}

float SculptorParameters::getDThickness() const {
    return dThickness;
}

void SculptorParameters::setDThickness(float value) {
    //if (validLemme(dMove, value, maxEdgeLength))
        dThickness = value;
}

bool SculptorParameters::valid() {
    return validMinMaxEdge(minEdgeLength, maxEdgeLength) && validLemme(dMove, dThickness, maxEdgeLength);
}

bool SculptorParameters::validMinMaxEdge(float min, float max) {
    return min > 0 && min <= max/2.;
}

bool SculptorParameters::validLemme(float dmove, float dthickness, float ddetail) {
    return dmove <= (dthickness - ddetail/sqrt(3)) / 2.;
}

