#include "Canvas.h"
#include <GL/freeglut.h>

Canvas::Canvas(Visualizer* visualizer, int x, int y, int w, int h) : Canvas_ (x, y, w, h) {
    this->visualizer = visualizer;
}

void Canvas::render() {
    visualizer->draw();
}