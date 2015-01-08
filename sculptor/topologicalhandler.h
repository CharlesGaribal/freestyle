#ifndef TOPOLOGICALHANDLER_H
#define TOPOLOGICALHANDLER_H

class Sculptor;

class TopologicalHandler
{
public:
    TopologicalHandler(Sculptor *sculptor);

    void setSculptor(Sculptor *sculptor) {
        this->sculptor = sculptor;
    }

private:
    Sculptor *sculptor;
};

#endif // TOPOLOGICALHANDLER_H
