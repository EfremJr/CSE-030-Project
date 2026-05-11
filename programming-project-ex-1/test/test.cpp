#include "Graph.h"
#include "Visualizers.h"
#include <bobcat_ui/dropdown.h>
#include <bobcat_ui/float_input.h>
#include <bobcat_ui/int_input.h>
#include <bobcat_ui/textbox.h>
#include <cmath>
#include <igloo/igloo.h>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <unistd.h>
#include <Application.h>

using namespace igloo;
using namespace bobcat;
using namespace std;

Context(TestApplication) {
    int oldSTDOUT;
    int oldSTDERR;

    
    // Suppress outputs so that program being
    // tested does not pollute the console
    void SetUp() {
        oldSTDOUT = dup(1);
        oldSTDERR = dup(2);
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
    }

    // Restore output ability after tests are complete
    void TearDown() {
        fflush(stdout);
        fflush(stderr);
        dup2(oldSTDOUT, 1);
        dup2(oldSTDERR, 2);
    }
    

    stringstream testVertices() {
        stringstream vertices;

        vertices    << "A" << endl << "B" << endl << "C" << endl
                    << "D" << endl << "E" << endl << "F" << endl
                    << "G" << endl;

        return vertices;
    }

    stringstream testEdges() {
        stringstream edges;

        edges   << "0,1,4,1000" << endl
                << "1,6,3,500" << endl
                << "6,2,2,400" << endl
                << "2,3,5,1100" << endl
                << "0,6,6,2000" << endl
                << "6,3,6,4000" << endl
                << "0,5,3,1000" << endl
                << "5,4,4,1100" << endl
                << "4,3,3,1100" << endl
                << "5,6,4,800" << endl;

        return edges;
    }

    Path* makePath(ArrayList<Vertex*> vertices) {
        if (vertices.size() == 0) {
            throw std::logic_error("Can't make path with no vertices");
        }

        Waypoint* cur = new Waypoint(vertices[0]);
        Waypoint* temp;

        for (int i = 1; i < vertices.size(); i++) {
            temp = new Waypoint(vertices[i]);
            cur->children.append(temp);
            temp->parent = cur;
            cur = temp;
        }

        return new Path(cur);
    }

    bool pathsEqual(Path* path1, Path* path2) {
        if (path1->list.size() != path2->list.size()) {
            return false;
        }
        for (int i = 0; i < path1->list.size(); i++) {
            if (path1->list[i]->vertex->index != path2->list[i]->vertex->index) {
                return false;
            }
        }
        return true;
    }

    // Initialization Tests

    Spec(TestDefaultInitialization) {
        Application* app = new Application();

        Assert::That(app != nullptr, Is().True());

        delete app;
    }

    Spec(TestTestInitialization) {
        stringstream vertices = testVertices();
        stringstream edges = testEdges();

        Application* app = new Application(&vertices, &edges);

        Assert::That(app->g.vertices.size(), Is().EqualTo(7));

        delete app;
    }

    Spec(TestInitUI) {
        stringstream vertices, edges;

        // Data
        vertices << "A" << endl << "B" << endl << "C" << endl << "D" << endl;
        edges   << "0,3,10,2000" << endl                            // Shortest
                << "0,1,2,1500" << endl << "1,3,3,1500" << endl     // Fastest
                << "0,2,4,500" << endl << "2,3,3,500" << endl;      // Cheapest

        Application* app = new Application(&vertices, &edges);

        Dropdown* from = app->fromDropdown;
        Dropdown* to = app->toDropdown;
        Dropdown* searchDrop = app->searchDropdown;

        FloatInput* min = app->minDistInput;
        FloatInput* max = app->maxDistInput;
        FloatInput* step = app->stepInput;
        IntInput* iterations = app->numIterationsInput;
        
        // From Dropdown checks
        from->value(0);
        Assert::That(from->text(), Equals("A"));
        from->value(1);
        Assert::That(from->text(), Equals("B"));
        from->value(2);
        Assert::That(from->text(), Equals("C"));
        from->value(3);
        Assert::That(from->text(), Equals("D"));

        // To Dropdown checks
        to->value(0);
        Assert::That(to->text(), Equals("A"));
        to->value(1);
        Assert::That(to->text(), Equals("B"));
        to->value(2);
        Assert::That(to->text(), Equals("C"));
        to->value(3);
        Assert::That(to->text(), Equals("D"));

        // Search Dropdown checks
        searchDrop->value(0);
        Assert::That(searchDrop->text(), Equals("Least Stops"));
        searchDrop->value(1);
        Assert::That(searchDrop->text(), Equals("Lowest Cost"));
        searchDrop->value(2);
        Assert::That(searchDrop->text(), Equals("Lowest Time"));

        // Graph Input Checks
        Assert::That(isnan(min->value()), Is().False());
        Assert::That(isnan(max->value()), Is().False());
        Assert::That(isnan(step->value()), Is().False());
        Assert::That(isnan(iterations->value()), Is().False());
        
        // Graph Visualizer checks
        Assert::That(app->graphVisualizer->vertices.size(), Is().EqualTo(4));
        Assert::That(app->graphVisualizer->edges.size(), Is().EqualTo(5));

        delete app;
    }

    Spec(TestIdenticalIndexes) {
        stringstream vertices, edges;
        
        vertices << "A" << endl << "B" << endl << "C" << endl << "D" << endl;

        Application* app = new Application(&vertices, &edges);

        Graph* graph = &app->g;
        GraphVisualizer* gVisual = app->graphVisualizer;

        Assert::That(graph->vertices.size(), Is().EqualTo(app->cities.size()));
        Assert::That(graph->vertices.size(), Is().EqualTo(gVisual->vertices.size()));

        for (int i = 0; i < app->cities.size(); i++) {
            Assert::That(graph->vertices[i] == app->cities[i], Is().True());

            Assert::That(graph->vertices[i]->index, Is().EqualTo(i));
            Assert::That(gVisual->vertices[i]->cityIndex, Is().EqualTo(i));

            Assert::That(graph->vertices[i]->data, Equals(gVisual->vertices[i]->name));
        }

        delete app;
    }
    
    // Algorithm Tests

    Spec(TestBFS) {
        stringstream vertices = testVertices();
        stringstream edges = testEdges();
        
        Application* app = new Application(&vertices, &edges);

        ArrayList<Vertex*> expectedVertices;
        expectedVertices.append(app->cities[0]);
        expectedVertices.append(app->cities[6]);
        expectedVertices.append(app->cities[3]);
        
        Path* expectedPath = makePath(expectedVertices);
        Path* resultPath = app->g.bfs(app->cities[0], app->cities[3]);

        Assert::That(pathsEqual(expectedPath, resultPath), IsTrue());
        
        delete app;
    }

    Spec(TestUCSCost) {
        stringstream vertices = testVertices();
        stringstream edges = testEdges();

        Application* app = new Application(&vertices, &edges);

        ArrayList<Vertex*> expectedVertices;
        expectedVertices.append(app->cities[0]);
        expectedVertices.append(app->cities[1]);
        expectedVertices.append(app->cities[6]);
        expectedVertices.append(app->cities[2]);
        expectedVertices.append(app->cities[3]);

        Path* expectedPath = makePath(expectedVertices);
        Path* resultPath = app->g.ucsCost(app->cities[0], app->cities[3]);

        Assert::That(pathsEqual(expectedPath, resultPath), IsTrue());

        delete app;
    }

    Spec(TestUCSTime) {
        stringstream vertices = testVertices();
        stringstream edges = testEdges();

        Application* app = new Application(&vertices, &edges);

        ArrayList<Vertex*> expectedVertices;
        expectedVertices.append(app->cities[0]);
        expectedVertices.append(app->cities[5]);
        expectedVertices.append(app->cities[4]);
        expectedVertices.append(app->cities[3]);

        Path* expectedPath = makePath(expectedVertices);
        Path* resultPath = app->g.ucsTime(app->cities[0], app->cities[3]);

        Assert::That(pathsEqual(expectedPath, resultPath), IsTrue());

        delete app;
    }

    Spec(TestPath) {
        stringstream vertices = testVertices();
        stringstream edges = testEdges();
        
        Application* app = new Application(&vertices, &edges);
        
        Path* path = app->g.ucsCost(app->cities[0], app->cities[3]);

        for (int i = 0; i < path->list.size(); i++) {
            Waypoint* curPoint = path->list[i];

            // If not last
            if (i < path->list.size()-1) {
                Assert::That(curPoint->children.size(), Is().EqualTo(1));
                
                Waypoint* next = path->list[i+1];
                Waypoint* child = curPoint->children[0];
                Assert::That(child->vertex->data, Equals(next->vertex->data));
            }

            // If not first
            if (i > 0) {
                Assert::That(curPoint->parent != nullptr, Is().True());

                Waypoint* previous = path->list[i-1];
                Assert::That(curPoint->parent->vertex->data, Equals(previous->vertex->data));
            }
        }

        delete app;
    }

    // Graph Visualizer Tests

    Spec(TestOverlappingVertices) {
        stringstream vertices, edges;

        vertices << "A" << endl << "B" << endl;

        Application* app = new Application(&vertices, &edges);

        GraphVisualizer* gVisual = app->graphVisualizer;

        VertexVisualizer* A = gVisual->vertices[0];
        VertexVisualizer* B = gVisual->vertices[1];

        A->x = 0.0;
        A->y = 0.0;
        B->x = 0.0;
        B->y = 0.0;

        gVisual->moveFurther(A, B, 0.05);

        Assert::That(isnan(A->x) || isnan(A->y), Is().False());
        Assert::That(isnan(B->x) || isnan(B->y), Is().False());
        
        A->x = 0.0;
        A->y = 0.0;
        B->x = 0.0;
        B->y = 0.0;

        gVisual->moveCloser(A, B, 0.05);

        Assert::That(isnan(A->x) || isnan(A->y), Is().False());
        Assert::That(isnan(B->x) || isnan(B->y), Is().False());

        delete app;
    }

    Spec(TestOverlappingVertexAndEdge) {
        stringstream vertices, edges;

        vertices << "A" << endl << "B" << endl << "C" << endl;
        edges << "0,1,5,500" << endl;

        Application* app = new Application(&vertices, &edges);

        VertexVisualizer* A = app->graphVisualizer->vertices[0];
        VertexVisualizer* B = app->graphVisualizer->vertices[1];
        VertexVisualizer* C = app->graphVisualizer->vertices[2];
        EdgeVisualizer* edge = app->graphVisualizer->edges[0];

        Assert::That(C->name, Equals("C"));

        A->x = 0.0;
        A->y = 0.0;
        B->x = 0.0;
        B->y = 0.0;
        C->x = 0.0;
        C->y = 0.0;

        app->graphVisualizer->spreadHelperEdge(C, edge, 0.2, 0.05);

        Assert::That(isnan(C->x) || isnan(C->y), Is().False());

        A->x = 0.0;
        A->y = 0.0;
        B->x = 0.0;
        B->y = 0.0;
        C->x = 0.0;
        C->y = 0.0;
        
        app->graphVisualizer->oneDirMoveCloser(C, 0.0, 0.0, 0.05);

        Assert::That(isnan(C->x) || isnan(C->y), Is().False());

        delete app;
    }

    Spec(TestSpreadArgumentsZero) {
        stringstream vertices, edges;

        vertices << "A" << endl << "B" << endl << "C" << endl;
        edges << "0,1,5,500" << endl;

        Application* app = new Application(&vertices, &edges);

        app->graphVisualizer->spreadVertices(0.0, 0.0, 0.0, 10);
        
        VertexVisualizer* A = app->graphVisualizer->vertices[0];
        VertexVisualizer* B = app->graphVisualizer->vertices[1];
        VertexVisualizer* C = app->graphVisualizer->vertices[2];
        EdgeVisualizer* edge = app->graphVisualizer->edges[0];
        
        A->x = 0.0;
        A->y = 0.0;
        B->x = 0.0;
        B->y = 0.0;
        C->x = 0.0;
        C->y = 0.0;

        app->graphVisualizer->moveFurther(A, B, 0.0);
        app->graphVisualizer->moveCloser(A, B, 0.0);
        app->graphVisualizer->spreadHelperEdge(C, edge, 0.0, 0.0);
        app->graphVisualizer->oneDirMoveCloser(C, 0.0, 0.0, 0.0);
        
        Assert::That(isnan(A->x) || isnan(A->y), Is().False());
        Assert::That(isnan(B->x) || isnan(B->y), Is().False());
        Assert::That(isnan(C->x) || isnan(C->y), Is().False());

        delete app;
    }

    Spec(TestDifferentGraphInputValues) {
        stringstream vertices, edges;

        vertices << "A" << endl << "B" << endl << "C" << endl;
        
        Application* app = new Application(&vertices, &edges);

        srand(1);

        app->onClick(app->regenGraphButton);

        ArrayList<float> xVals;
        ArrayList<float> yVals;

        for (int i = 0; i < app->graphVisualizer->vertices.size(); i++) {
            xVals.append(app->graphVisualizer->vertices[i]->x);
            yVals.append(app->graphVisualizer->vertices[i]->y);
        }

        app->minDistInput->value(0.5);
        app->maxDistInput->value(1.0);
        app->stepInput->value(0.01);
        app->numIterationsInput->value(100);
        srand(1);
        app->onClick(app->regenGraphButton);

        for (int i = 0; i < app->graphVisualizer->vertices.size(); i++) {
            Assert::That(app->graphVisualizer->vertices[i]->x, Is().Not().EqualTo(xVals[i]));
            Assert::That(app->graphVisualizer->vertices[i]->y, Is().Not().EqualTo(yVals[i]));
        }
    }

    // Random Test

    Spec(TestReseedingGraph) {
        stringstream vertices, edges;

        vertices << "A" << endl << "B" << endl << "C" << endl;
        
        Application* app = new Application(&vertices, &edges);

        srand(1);

        app->onClick(app->regenGraphButton);

        ArrayList<float> xVals;
        ArrayList<float> yVals;

        for (int i = 0; i < app->graphVisualizer->vertices.size(); i++) {
            xVals.append(app->graphVisualizer->vertices[i]->x);
            yVals.append(app->graphVisualizer->vertices[i]->y);
        }

        srand(1);
        app->onClick(app->regenGraphButton);

        for (int i = 0; i < app->graphVisualizer->vertices.size(); i++) {
            Assert::That(app->graphVisualizer->vertices[i]->x, Is().EqualTo(xVals[i]));
            Assert::That(app->graphVisualizer->vertices[i]->y, Is().EqualTo(yVals[i]));
        }
    }

    Spec(TestRandom) {
        stringstream vertices1, edges1;

        vertices1 << "A" << endl << "B" << endl << "C" << endl;
        edges1 << "0,1,5,500" << endl;

        Application* app1 = new Application(&vertices1, &edges1);

        std::this_thread::sleep_for(std::chrono::seconds(1));

        stringstream vertices2, edges2;

        vertices2 << "A" << endl << "B" << endl << "C" << endl;
        edges2 << "0,1,5,500" << endl;

        Application* app2 = new Application(&vertices2, &edges2);

        GraphVisualizer* graph1 = app1->graphVisualizer;
        GraphVisualizer* graph2 = app2->graphVisualizer;

        bool different = false;

        for (int i = 0; i < graph1->vertices.size(); i++) {
            if ((graph1->vertices[i]->x != graph2->vertices[i]->x) || (graph1->vertices[i]->y != graph2->vertices[i]->y)) {
                different = true;
                break;
            }
        }

        Assert::That(different, Is().True());

        delete app1;
        delete app2;
    }
};

int main(int argc, const char* argv[]){
    TestRunner::RunAllTests(argc, argv);
}