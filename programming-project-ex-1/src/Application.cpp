#include "Graph.h"
#include "Visualizers.h"
#include <Application.h>
#include <bobcat_ui/bobcat_ui.h>
#include <bobcat_ui/dropdown.h>
#include <bobcat_ui/textbox.h>
#include <fstream>
#include <sstream>
#include <string>

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
        
        Stack<string> orderedPath;
        orderedPath.push(path->vertex->data);
        Waypoint* temp = path;
        while (temp->parent != nullptr) {
            temp = temp->parent;
            orderedPath.push(temp->vertex->data);
        }
        
        string pathText;
        pathText = orderedPath.pop();
        int i = 1;
        int count = 1;
        while(!orderedPath.isEmpty()){
            if (i == 3){
                i = 0;
                pathText += "\n";
            }
            pathText += " -> " + orderedPath.pop();
            i++;
            count++;
        }
        
        if (fromIndex == toIndex){
            outputBox->label("You're already here! No flight needed!\nCost: FREE!\nTime: 0");
        } else{
            outputBox->label(pathText + "\nNumber of Flights: " + to_string(count) + "\nCost: $" + to_string(path->partialCost) + "\nTime: " + to_string(path->partialTime) + " Hours");
        }

        graphVisualizer->visualizePath(path);
        graphVisualizer->renderMode = RenderMode::PATH;
        canvas->redraw();
    }
    else if (sender == clearButton) {
        path = nullptr;
        outputBox->label("");
        graphVisualizer->clearPath();
        graphVisualizer->renderMode = RenderMode::VERTICES;
        canvas->redraw();
    }
    else if (sender == showAllButton) {
        outputBox->label("");
        graphVisualizer->renderMode = RenderMode::EVERYTHING;
        canvas->redraw();
    }
    else if (sender == regenGraphButton) {
        graphVisualizer->defaultSpreadVertices();
        canvas->redraw();
    }
}

void Application::onCanvasMouseDown(bobcat::Widget* sender, float mx, float my){
    for (int i = 0; i < graphVisualizer->vertices.size(); i++){
        VertexVisualizer* v = graphVisualizer->vertices[i];
        float distX = mx - v->x;
        float distY = my - v->y;
        float distanceSq = (distX * distX) + (distY * distY);
        if (distanceSq <= 0.0025){
            visualizerBox->label(v->name);
            return;
        }
    }
    if (graphVisualizer->renderMode == RenderMode::PATH){
        for (int i = 0; i < graphVisualizer->pathEdges.size(); i++){
            EdgeVisualizer* e = graphVisualizer->pathEdges[i];
            float x1 = e->vertex1->x;
            float y1 = e->vertex1->y;
            float x2 = e->vertex2->x;
            float y2 = e->vertex2->y;

            float lengthSq = ((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1));

            float t = ((mx-x1)*(x2-x1) + (my-y1)*(y2-y1)) / lengthSq;

            t = max(0.0f, min(1.0f, t));

            float closestX = x1 + t * (x2 - x1);
            float closestY = y1 + t * (y2 - y1);

            float distX = mx - closestX;
            float distY = my - closestY;

            float distanceSq = (distX * distX) + (distY * distY);
            if (distanceSq <= 0.0015){
                visualizerBox->label(e->vertex1->name + " -> " + e->vertex2->name + "  Cost: $" + to_string(e->cost) + "  Time: " + to_string(e->time) + "H");
                return;
            }
        }
    }
    if (graphVisualizer->renderMode == RenderMode::EVERYTHING){
        for (int i = 0; i < graphVisualizer->edges.size(); i++){
            EdgeVisualizer* e = graphVisualizer->edges[i];
            float x1 = e->vertex1->x;
            float y1 = e->vertex1->y;
            float x2 = e->vertex2->x;
            float y2 = e->vertex2->y;

            float lengthSq = ((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1));

            float t = ((mx-x1)*(x2-x1) + (my-y1)*(y2-y1)) / lengthSq;

            t = max(0.0f, min(1.0f, t));

            float closestX = x1 + t * (x2 - x1);
            float closestY = y1 + t * (y2 - y1);

            float distX = mx - closestX;
            float distY = my - closestY;

            float distanceSq = (distX * distX) + (distY * distY);
            if (distanceSq <= 0.001){
                visualizerBox->label(e->vertex1->name + " -> " + e->vertex2->name + "  Cost: $" + to_string(e->cost) + "  Time: " + to_string(e->time) + "H");
                return;
            }
        }
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

    outputBox = new TextBox(25, 160, 350, 130, "");
    
    ON_CLICK(searchButton, Application::onClick);
    ON_CLICK(clearButton, Application::onClick);
    ON_CLICK(showAllButton, Application::onClick);

    graphVisualizer = new GraphVisualizer(&g);
    canvas = new Canvas(graphVisualizer, 425, 25, 350, 350);
    canvas->redraw();

    
    regenGraphButton = new Button(500, 2, 200, 20, "Regenerate Graph");
    visualizerBox = new TextBox(425, 375, 350, 25, "Click on the graph for info!");
    visualizerBox->labelsize(12);
    ON_MOUSE_DOWN(canvas, Application::onCanvasMouseDown);
    ON_CLICK(regenGraphButton, Application::onClick);
    
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
    // Seeding upon starting the app
    srand(time(0));
    initData(vertices, edges);
    initUI();
}