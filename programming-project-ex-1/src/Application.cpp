#include "Graph.h"
#include "Visualizers.h"
#include <Application.h>
#include <bobcat_ui/bobcat_ui.h>
#include <bobcat_ui/dropdown.h>
#include <fstream>
#include <sstream>

using namespace bobcat;
using namespace std;

void Application::initData(istream* vertices, istream* edges) {
    string line;

    while(getline(*vertices, line)) {
        cities.append(new Vertex(line, cities.size()));
    }

    // add vertices to graph
    for (int i = 0; i < cities.size(); i++) {
        g.addVertex(cities[i]);
    }

    while(getline(*edges, line)) {
        stringstream ss(line);
        string from, to, time, cost;

        getline(ss, from, ',');
        getline(ss, to, ',');
        getline(ss, time, ',');
        getline(ss, cost, ',');

        g.addEdge(cities[stoi(from)], cities[stoi(to)], stoi(cost), stoi(time));
    }

    path = nullptr;
}

void Application::onClick(bobcat::Widget* sender){
    if (sender == searchButton){
        int fromIndex = fromDropdown->value();
        int toIndex = toDropdown->value();
        std::string searchOption = searchDropdown->text();

        if (searchOption == "Least Stops"){
            path = g.bfs(cities[fromIndex], cities[toIndex]);
        }
        else if (searchOption == "Lowest Cost"){
            path = g.ucsCost(cities[fromIndex], cities[toIndex]);
        }
        else if (searchOption == "Lowest Time"){
            path = g.ucsTime(cities[fromIndex], cities[toIndex]);
        }
        cout << path->vertex->data;
        Waypoint* temp = path;
        while (temp->parent != nullptr) {
            temp = temp->parent;
            cout << " <- " << temp->vertex->data;
        }
        cout << endl;
        graphVisualizer->visualizePath(path);
        graphVisualizer->renderMode = RenderMode::PATH;
        canvas->redraw();
    }
    else if (sender == clearButton) {
        path = nullptr;
        graphVisualizer->clearPath();
        graphVisualizer->renderMode = RenderMode::VERTICES;
        canvas->redraw();
    }
    else if (sender == showAllButton) {
        graphVisualizer->renderMode = RenderMode::EVERYTHING;
        canvas->redraw();
    }
}

void Application::initUI() {
    window = new Window(25, 75, 800, 400, "Simple Navigation Project");
    fromDropdown = new Dropdown(25, 25, 165, 25, "Origin");
    toDropdown = new Dropdown(210, 25, 165, 25, "Destination");

    for(int i = 0; i < cities.size(); i++){
        fromDropdown->add(cities[i]->data);
        toDropdown->add(cities[i]->data);
    }
    
    searchDropdown = new Dropdown(25, 75, 350, 25, "Search Type");

    searchDropdown->add("Least Stops");
    searchDropdown->add("Lowest Cost");
    searchDropdown->add("Lowest Time");
    
    searchButton = new Button(25, 125, 350, 25, "Search");

    clearButton = new Button(25, 350, 350, 25, "Clear");
    showAllButton = new Button(25, 300, 350, 25, "Show All Flights Connections");
    
    ON_CLICK(searchButton, Application::onClick);
    ON_CLICK(clearButton, Application::onClick);
    ON_CLICK(showAllButton, Application::onClick);

    graphVisualizer = new GraphVisualizer(&g);
    canvas = new Canvas(graphVisualizer, 425, 25, 350, 350);
    canvas->redraw();
    
    window->show();
}

Application::Application() {
    // this is a change that I made - John Quinn

    // By default, add data from the files in the assets folder
    fstream vertices, edges;
    vertices.open("./assets/vertices.csv", ios::in);
    edges.open("./assets/edges.csv", ios::in);
    if (vertices.is_open() && edges.is_open()) {
        initData(&vertices, &edges);
        vertices.close();
        edges.close();
    }

    initUI();
}

Application::Application(std::istream* vertices, std::istream* edges) {
    initData(vertices, edges);
    initUI();
}