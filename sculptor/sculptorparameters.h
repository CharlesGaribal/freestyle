#ifndef SCULPTORPARAMETERS_H
#define SCULPTORPARAMETERS_H

class SculptorParameters
{
private:
    double minEdgeLength, maxEdgeLength;
    double dMove, dThickness;

public:
    SculptorParameters() {}
    SculptorParameters(double minEdgeLength, double maxEdgeLength, double dmove, double dthickness);

    double getMinEdgeLength() const;
    void setMinEdgeLength(double length);

    double getMaxEdgeLength() const;
    void setMaxEdgeLength(double length);

    double getDMove() const;
    void setDMove(double value);

    double getDThickness() const;
    void setDThickness(double value);

    bool valid();

    static bool validMinMaxEdge(double min, double max);
    static bool validLemme(double dmove, double dthickness, double ddetail);
};

#endif // SCULPTORPARAMETERS_H
