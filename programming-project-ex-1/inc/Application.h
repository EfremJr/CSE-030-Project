#ifndef APPLICATION_H
#define APPLICATION_H

#include <bobcat_ui/all.h>
#include <Graph.h>
#include <bobcat_ui/dropdown.h>
#include <Canvas.h>
#include <iostream>

class Application : public bobcat::Application_ {
    bobcat::Window* window;

    bobcat::Dropdown* fromDropdown;
    bobcat::Dropdown* toDropdown;
    bobcat::Dropdown* searchDropdown;
    bobcat::Button* searchButton;
    bobcat::Button* clearButton;
    bobcat::Button* showAllButton;

    Canvas* canvas;

    Graph g;
    ArrayList<Vertex*> cities;

    Waypoint* path;

    void initData(std::istream* vertices, std::istream* edges);

    void initUI();
    
    void onClick(bobcat::Widget* sender);
    
public:
    Application();

    Application(std::istream* vertices, std::istream* edges);

    friend struct TestApplication;
};

#endif

