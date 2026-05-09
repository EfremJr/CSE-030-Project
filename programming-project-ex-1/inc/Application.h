#ifndef APPLICATION_H
#define APPLICATION_H

#include "Visualizers.h"
#include <bobcat_ui/all.h>
#include <Graph.h>
#include <bobcat_ui/dropdown.h>
#include <Canvas.h>
#include <bobcat_ui/textbox.h>
#include <iostream>

class Application : public bobcat::Application_ {
    bobcat::Window* window;

    bobcat::Dropdown* fromDropdown;
    bobcat::Dropdown* toDropdown;
    bobcat::Dropdown* searchDropdown;
    bobcat::Button* searchButton;
    bobcat::Button* clearButton;
    bobcat::Button* showAllButton;
    bobcat::Button* regenGraphButton;

    bobcat::TextBox* visualizerBox;
    bobcat::TextBox* outputBox;

    Graph g;
    ArrayList<Vertex*> cities;

    Canvas* canvas;
    GraphVisualizer* graphVisualizer;

    Path* path;

    void initData(std::istream* vertices, std::istream* edges);

    void initUI();
    
    void onClick(bobcat::Widget* sender);

    void onCanvasMouseDown(bobcat::Widget* sender, float mx, float my);
    
public:
    Application();

    Application(std::istream* vertices, std::istream* edges);

    ~Application();

    friend struct TestApplication;
};

#endif

