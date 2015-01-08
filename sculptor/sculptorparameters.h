#ifndef SCULPTORPARAMETERS_H
#define SCULPTORPARAMETERS_H

class SculptorParameters
{
private:
    float minEdgeLength, maxEdgeLength;
    float dMove, dThickness;

public:
    SculptorParameters(float minEdgeLength, float maxEdgeLength, float dmove, float dthickness);

    float getMinEdgeLength() const;
    void setMinEdgeLength(float length);

    float getMaxEdgeLength() const;
    void setMaxEdgeLength(float length);

    float getDMove() const;
    void setDMove(float value);

    float getDThickness() const;
    void setDThickness(float value);

    bool valid();

    static bool validMinMaxEdge(float min, float max);
    static bool validLemme(float dmove, float dthickness, float ddetail);
};

#endif // SCULPTORPARAMETERS_H
