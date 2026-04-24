#ifndef APPLICATION_H
#define APPLICATION_H

#include <bobcat_ui/all.h>

class Application : public bobcat::Application_ {
    bobcat::Window* window;
    
public:
    Application();
};

#endif

