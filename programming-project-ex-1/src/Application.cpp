#include <Application.h>
#include <bobcat_ui/bobcat_ui.h>
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

Application::Application() {
    // this is a change that I made - John Quinn

    window = new Window(25, 75, 400, 400, "Simple Navigation Project");


    window->show();
}