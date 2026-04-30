#ifndef APPLICATION_H
#define APPLICATION_H

#include <bobcat_ui/all.h>
#include <Graph.h>
#include <bobcat_ui/dropdown.h>

class Application : public bobcat::Application_ {
    bobcat::Window* window;

    bobcat::Dropdown* fromDropdown;
    bobcat::Dropdown* toDropdown;
    bobcat::Dropdown* searchDropdown;
    bobcat::Button* searchButton;
    bobcat::Button* clearButton;
    bobcat::Button* showAllButton;

    Graph g;
    ArrayList<Vertex*> cities;

    void initData();
    
    void onClick(bobcat::Widget* sender);
    
public:
    Application();
};

#endif

