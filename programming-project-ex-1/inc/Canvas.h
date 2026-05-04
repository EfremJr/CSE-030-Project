#ifndef CANVAS_H
#define CANVAS_H

#include <bobcat_ui/all.h>
#include <Visualizer.h>

class Canvas : public bobcat::Canvas_ {
    Visualizer* visualizer;
    
public:
    Canvas(Visualizer* visualizer, int x, int y, int w, int h);

    void render();
};

#endif