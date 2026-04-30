#include <Application.h>
#include <bobcat_ui/bobcat_ui.h>
#include <bobcat_ui/dropdown.h>
#include <fstream>
#include <sstream>

using namespace bobcat;
using namespace std;

void Application::initData() {
    fstream file;

    // read vertices from file
    file.open("./assets/vertices.csv", ios::in);
    if (file.is_open()) {
        string line;
        
        while(getline(file, line)) {
            cities.append(new Vertex(line));
        }

        file.close();
    }

    // add vertices to graph
    for (int i = 0; i < cities.size(); i++) {
        g.addVertex(cities[i]);
    }
    
    // read edges from file and add them as edges to the graph
    file.open("./assets/edges.csv", ios::in);
    if (file.is_open()) {
        string line;
        
        while(getline(file, line)) {
            stringstream ss(line);
            string from, to, time, cost;

            getline(ss, from, ',');
            getline(ss, to, ',');
            getline(ss, time, ',');
            getline(ss, cost, ',');

            g.addEdge(cities[stoi(from)], cities[stoi(to)], stoi(cost), stoi(time));
        }

        file.close();
    }
}

void Application::onClick(bobcat::Widget* sender){
    if (sender == searchButton){
        int fromIndex = fromDropdown->value();
        int toIndex = toDropdown->value();
        std::string searchOption = searchDropdown->text();

        if (searchOption == "Least Stops"){
            Waypoint* path = g.bfs(cities[fromIndex], cities[toIndex]);
        }
        else if (searchOption == "Lowest Cost"){
            Waypoint* path = g.ucsCost(cities[fromIndex], cities[toIndex]);
        }
        else if (searchOption == "Lowest Time"){
            Waypoint* path = g.ucsTime(cities[fromIndex], cities[toIndex]);
        }
    }
}

Application::Application() {
    // this is a change that I made - John Quinn
    initData();

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
    
    window->show();
}