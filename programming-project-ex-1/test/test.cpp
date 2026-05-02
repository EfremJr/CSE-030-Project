#include "Graph.h"
#include <igloo/igloo.h>
#include <sstream>
#include <stdexcept>
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

        vertices    << "SFO" << endl << "NYC" << endl << "GIG" << endl
                    << "CDG" << endl << "JNB" << endl << "SVO" << endl
                    << "SYD" << endl << "HND" << endl << "PEK" << endl;

        return vertices;
    }

    stringstream testEdges() {
        stringstream edges;

        edges   << "0,1,6,450" << endl
                << "1,2,13,900" << endl
                << "1,3,7,550" << endl
                << "1,4,14,1000" << endl
                << "1,5,15,1100" << endl
                << "1,6,40,3100" << endl
                << "2,3,11,750" << endl
                << "2,8,18,1300" << endl
                << "3,6,17,1200" << endl
                << "4,7,16,1180" << endl
                << "4,6,11,720" << endl
                << "6,7,10,700" << endl
                << "6,8,3,250" << endl
                << "8,5,8,580" << endl;

        return edges;
    }

    Waypoint* makePath(ArrayList<Vertex*> vertices) {
        if (vertices.size() == 0) {
            throw std::logic_error("Can't make path with no vertices");
        }

        Waypoint* cur;
        Waypoint* next = new Waypoint(vertices[0]);
        
        for(int i = 0; i < vertices.size() - 1; i++) {
            cur = next;
            cur->expand();
            
            for (int j = 0; j < cur->children.size(); j++) {
                if (cur->children[j]->vertex->data == vertices[i+1]->data) {
                    next = cur->children[j];
                    break;
                }
            }
        }

        return next;
    }

    bool pathsEqual(Waypoint* path1, Waypoint* path2) {
        if (path1->vertex->data != path2->vertex->data) {
            return false;
        }
        if (path1->parent == nullptr && path2->parent == nullptr) {
            return true;
        }
        if (path1->parent == nullptr || path2->parent == nullptr) {
            return false;
        }
        return pathsEqual(path1->parent, path2->parent);
    }

    Spec(TestDefaultInitialization) {
        Application* app = new Application();

        Assert::That(app->cities.size(), Is().Not().EqualTo(0));

        delete app;
    }

    Spec(TestTestInitialization) {
        stringstream vertices = testVertices();
        stringstream edges = testEdges();

        Application* app = new Application(&vertices, &edges);

        Assert::That(app->cities.size(), Is().EqualTo(9));

        delete app;
    }

    Spec(TestInitUI) {
        stringstream vertices, edges;

        vertices << "A" << endl << "B" << endl << "C" << endl;
        edges   << "0,1,1,500" << endl
                << "1,2,1,500" << endl
                << "2,0,2,1000" << endl;

        Application* app = new Application(&vertices, &edges);
        
        // From Dropdown checks
        app->fromDropdown->value(0);
        Assert::That(app->fromDropdown->text(), Equals("A"));
        app->fromDropdown->value(1);
        Assert::That(app->fromDropdown->text(), Equals("B"));
        app->fromDropdown->value(2);
        Assert::That(app->fromDropdown->text(), Equals("C"));

        // To Dropdown checks
        app->toDropdown->value(0);
        Assert::That(app->toDropdown->text(), Equals("A"));
        app->toDropdown->value(1);
        Assert::That(app->toDropdown->text(), Equals("B"));
        app->toDropdown->value(2);
        Assert::That(app->toDropdown->text(), Equals("C"));

        delete app;
    }
    
    Spec(TestBFS) {
        stringstream vertices = testVertices();
        stringstream edges = testEdges();
        
        Application* app = new Application(&vertices, &edges);

        ArrayList<Vertex*> expectedVertices;
        expectedVertices.append(app->cities[0]);
        expectedVertices.append(app->cities[1]);
        expectedVertices.append(app->cities[2]);
        expectedVertices.append(app->cities[8]);
        
        Waypoint* expectedPath = makePath(expectedVertices);
        Waypoint* resultPath = app->g.bfs(app->cities[0], app->cities[8]);

        Assert::That(pathsEqual(expectedPath, resultPath), IsTrue());
        
        delete app;
    }

    Spec(TestUCSCost) {
        stringstream vertices = testVertices();
        stringstream edges = testEdges();

        Application* app = new Application(&vertices, &edges);

        ArrayList<Vertex*> expectedVertices;
        expectedVertices.append(app->cities[3]);
        expectedVertices.append(app->cities[6]);
        expectedVertices.append(app->cities[7]);

        Waypoint* expectedPath = makePath(expectedVertices);
        Waypoint* resultPath = app->g.ucsCost(app->cities[3], app->cities[7]);

        Assert::That(pathsEqual(expectedPath, resultPath), IsTrue());

        delete app;
    }

    Spec(TestUCSTime) {
        stringstream vertices = testVertices();
        stringstream edges = testEdges();

        Application* app = new Application(&vertices, &edges);

        ArrayList<Vertex*> expectedVertices;
        expectedVertices.append(app->cities[7]);
        expectedVertices.append(app->cities[6]);
        expectedVertices.append(app->cities[8]);
        expectedVertices.append(app->cities[2]);

        Waypoint* expectedPath = makePath(expectedVertices);
        Waypoint* resultPath = app->g.ucsTime(app->cities[7], app->cities[2]);

        Assert::That(pathsEqual(expectedPath, resultPath), IsTrue());

        delete app;
    }
};

int main(int argc, const char* argv[]){
    TestRunner::RunAllTests(argc, argv);
}