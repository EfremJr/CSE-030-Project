#ifndef APPLICATION_H
#define APPLICATION_H

#include <bobcat_ui/all.h>
#include <Graph.h>

class Application : public bobcat::Application_ {
    bobcat::Window* window;

    Graph g;
    ArrayList<Vertex*> cities;

    void initData();
    
public:
    Application();
};

#endif

