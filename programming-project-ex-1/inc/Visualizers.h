#ifndef VISUALIZERS_H
#define VISUALIZERS_H

#include "ArrayList.h"
#include "HashTable.h"
#include <GL/gl.h>
#include <iostream>
#include <Graph.h>
#include <Visualizer.h>
#include <string>

class VertexVisualizer : public Visualizer {

public:
    float x;
    float y;
    std::string name;
    int cityIndex;

    VertexVisualizer(float x, float y, std::string name, int cityIndex){
        this->x = x;
        this->y = y;
        this->name = name;
        this->cityIndex = cityIndex;
    }

    void drawColor(float r, float g, float b) {
        std::cout << "    Rending Vertex (" << x << ", " << y << ")" << std::endl;

        glColor3f(r, g, b);
        glPointSize(10.0);

        glBegin(GL_POINTS);
            glVertex2f(x, y);
        glEnd();
    }

    void draw() {
        drawColor(0.0, 0.0, 0.0);
    }
};

class EdgeVisualizer : public Visualizer {
    
public:
    VertexVisualizer* vertex1;
    VertexVisualizer* vertex2;
    int cost;
    int time;

    EdgeVisualizer(VertexVisualizer* vertex1, VertexVisualizer* vertex2, int cost, int time){
        this->vertex1 = vertex1;
        this->vertex2 = vertex2;
        this->cost = cost;
        this->time = time;
    }

    void drawColor(float r, float g, float b) {
        glColor3f(r, g, b);
        glLineWidth(2.0);

        glBegin(GL_LINES);
            glVertex2f(vertex1->x, vertex1->y);
            glVertex2f(vertex2->x, vertex2->y);
        glEnd();
    }

    void draw() {
        drawColor(0.0, 0.0, 0.0);
    }
};

enum RenderMode {
    VERTICES,
    PATH,
    EVERYTHING
};

class GraphVisualizer : public Visualizer {

public:
    ArrayList<VertexVisualizer*> vertices;
    ArrayList<EdgeVisualizer*> edges;
    
    ArrayList<VertexVisualizer*> pathVertices;
    ArrayList<EdgeVisualizer*> pathEdges;
    bool pathExists;
    
    RenderMode renderMode;

    GraphVisualizer(Graph* graph){
        // Adding VertexVisualizers
        float dif = 2.0 / ( (float) graph->vertices.size() + 1);
        for (int i = 0; i < graph->vertices.size(); i++) {
            Vertex* currentVertex = graph->vertices[i];

            VertexVisualizer* visualVertex = new VertexVisualizer(-1.0 + dif * (i+1), -1.0 + dif * (i+1), currentVertex->data, currentVertex->index);
            vertices.append(visualVertex);
        }
        
        // Adding EdgeVisualizers
        for (int i = 0; i < graph->vertices.size(); i++) {
            Vertex* vertex = graph->vertices[i];

            for (int j = 0; j < vertex->edgeList.size(); j++) {
                Edge* edge = vertex->edgeList[j];
                
                // Doesn't add edge if it's already in list
                bool seen = false;
                for (int k = 0; k < edges.size(); k++) {
                    EdgeVisualizer* compEdge = edges[k];
                    if (    (compEdge->vertex1->cityIndex == edge->from->index && compEdge->vertex2->cityIndex == edge->to->index)
                        ||  (compEdge->vertex1->cityIndex == edge->to->index && compEdge->vertex2->cityIndex == edge->from->index) ) {
                        seen = true;
                        break;
                    }
                }
                if (seen) {
                    continue;
                }

                EdgeVisualizer* visualEdge = new EdgeVisualizer(vertices[edge->from->index], vertices[edge->to->index], edge->cost, edge->time);
                edges.append(visualEdge);
            }
        }
        renderMode = RenderMode::VERTICES;
        pathExists = false;
    }
    
    EdgeVisualizer* visualizedEdge(int index1, int index2) {
        for (int i = 0; i < edges.size(); i++) {
            if (    (edges[i]->vertex1->cityIndex == index1 && edges[i]->vertex2->cityIndex == index2)
                ||  (edges[i]->vertex1->cityIndex == index2 && edges[i]->vertex2->cityIndex == index1) ){
                return edges[i];
            }
        }
        throw std::logic_error("There doesn't exist an edge between these vertexes");
    }

    void visualizePath(Waypoint* endpoint) {
        clearPath();

        Waypoint* position = endpoint;
        while (position->parent != nullptr) {
            pathVertices.append(vertices[position->vertex->index]);

            EdgeVisualizer* edge = visualizedEdge(position->vertex->index, position->parent->vertex->index);
            pathEdges.append(edge);
            
            position = position->parent;
        }
        pathVertices.append(vertices[position->vertex->index]);

        pathExists = true;
    }

    void clearPath() {
        while (pathVertices.size() > 0) {
            pathVertices.removeLast();
        }
        while (pathEdges.size() > 0) {
            pathEdges.removeLast();
        }

        pathExists = false;
    }

    void draw() {
        if (renderMode == RenderMode::VERTICES) {
            std::cout << "Rendering Vertices" << std::endl;
            for (int i = 0; i < vertices.size(); i++) {
                vertices[i]->draw();
            }
        }
        else if (renderMode == RenderMode::PATH) {
            std::cout << "Rendering Path" << std::endl;
            HashTable<std::string> seen;
            if (pathExists) {
                for (int i = 0; i < pathVertices.size(); i++) {
                    pathVertices[i]->drawColor(1.0, 0.0, 0.0);
                    seen.insert(pathVertices[i]->name);
                }
                for (int i = 0; i < pathEdges.size(); i++) {
                    pathEdges[i]->drawColor(1.0, 0.0, 0.0);

                }
            }
            for (int i = 0; i < vertices.size(); i++) {
                if (!seen.search(vertices[i]->name)) {
                    vertices[i]->draw();
                }
            }
        }
        else if (renderMode == RenderMode::EVERYTHING) {
            std::cout << "Rendering Everything" << std::endl;
            for (int i = 0; i < vertices.size(); i++) {
                vertices[i]->draw();
            }
            for (int i = 0; i < edges.size(); i++) {
                edges[i]->draw();
            }
        }
    }
};

#endif