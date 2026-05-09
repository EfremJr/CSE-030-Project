#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <GL/gl.h>

class Visualizer {
    public:
    virtual void draw() = 0;
    virtual ~Visualizer() {}
};

#endif