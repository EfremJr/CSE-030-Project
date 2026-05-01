#include "Canvas.h"
#include <GL/freeglut.h>

Canvas::Canvas(int x, int y, int w, int h) : Canvas_ (x, y, w, h) {
    //
}

void Canvas::render() {
    glBegin(GL_POINTS);
        glVertex2f(0.0, 0.0);
    glEnd();
}