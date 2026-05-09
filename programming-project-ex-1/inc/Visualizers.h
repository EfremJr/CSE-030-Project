#ifndef VISUALIZERS_H
#define VISUALIZERS_H

#include "ArrayList.h"
#include "HashTable.h"
#include <GL/gl.h>
#include <cmath>
#include <iostream>
#include <Graph.h>
#include <Visualizer.h>
#include <string>
#include <ctime>

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

    
    
    void moveCloser(VertexVisualizer* vertex1, VertexVisualizer* vertex2, float step) {
        float dist = std::sqrt(std::pow(vertex2->x - vertex1->x, 2) + std::pow(vertex2->y - vertex1->y, 2));
        
        float unitX = (vertex2->x - vertex1->x) / dist;
        float unitY = (vertex2->y - vertex1->y) / dist;

        float dx = unitX * step;
        float dy = unitY * step;

        vertex1->x += dx;
        vertex2->x -= dx;
        vertex1->y += dy;
        vertex2->y -= dy;
    }

    void moveFurther(VertexVisualizer* vertex1, VertexVisualizer* vertex2, float step) {
        float dist = std::sqrt(std::pow(vertex2->x - vertex1->x, 2) + std::pow(vertex2->y - vertex1->y, 2));
        
        float unitX = (vertex2->x - vertex1->x) / dist;
        float unitY = (vertex2->y - vertex1->y) / dist;

        float dx = -unitX * step;
        float dy = -unitY * step;

        vertex1->x += dx;
        vertex2->x -= dx;
        vertex1->y += dy;
        vertex2->y -= dy;
    }

    void oneDirMoveCloser(VertexVisualizer* vertex, float x, float y, float step) {
        float dist = std::sqrt(std::pow(x - vertex->x, 2) + std::pow(y - vertex->y, 2));
        
        float unitX = (x - vertex->x) / dist;
        float unitY = (y - vertex->y) / dist;

        float dx = unitX * step;
        float dy = unitY * step;

        vertex->x += dx;
        vertex->y += dy;
    }

    void oneDirMoveFurther(VertexVisualizer* vertex, float x, float y, float step) {
        float dist = std::sqrt(std::pow(x - vertex->x, 2) + std::pow(y - vertex->y, 2));
        
        float unitX = (x - vertex->x) / dist;
        float unitY = (y - vertex->y) / dist;

        float dx = -unitX * step;
        float dy = -unitY * step;

        vertex->x += dx;
        vertex->y += dy;
    }

public:
    ArrayList<VertexVisualizer*> vertices;
    ArrayList<EdgeVisualizer*> edges;
    
    ArrayList<VertexVisualizer*> pathVertices;
    ArrayList<EdgeVisualizer*> pathEdges;
    bool pathExists;
    
    RenderMode renderMode;

    EdgeVisualizer* visualizedEdge(int index1, int index2) {
        for (int i = 0; i < edges.size(); i++) {
            if (    (edges[i]->vertex1->cityIndex == index1 && edges[i]->vertex2->cityIndex == index2)
                ||  (edges[i]->vertex1->cityIndex == index2 && edges[i]->vertex2->cityIndex == index1) ){
                return edges[i];
            }
        }
        return nullptr;
    }

    void spreadVertices(float squareMin, float squareMax, float step, unsigned int iterations) {
        // Randomize positions
        for (int i = 0; i < vertices.size(); i++) {
            VertexVisualizer* currentVertex = vertices[i];
            currentVertex->x = ((float)rand() / RAND_MAX) * 2.0 - 1.0;
            currentVertex->y = ((float)rand() / RAND_MAX) * 2.0 - 1.0;
        }

        // adjust positions over a number of iterations
        for (unsigned int iter = 0; iter < iterations; iter++) {
            for (int i = 0; i < vertices.size() - 1; i++) {
                VertexVisualizer* vertex1 = vertices[i];
                
                for (int j = i + 1; j < vertices.size(); j++) {
                    VertexVisualizer* vertex2 = vertices[j];
                    
                    // Calculate the squared distance between two vertices
                    float dx = vertices[j]->x - vertices[i]->x;
                    float dy = vertices[j]->y - vertices[i]->y;
                    float squareDist = dx * dx + dy * dy;

                    bool edgeExists = visualizedEdge(vertex1->cityIndex, vertex2->cityIndex) != nullptr;

                    // If the edge exists and too far apart, move them closer
                    if (edgeExists && squareDist > squareMax) {
                        moveCloser(vertex1, vertex2, step);
                    // If too close, move them apart, not needed if they were moved closer
                    } else if (squareDist < squareMin) {
                        moveFurther(vertex1, vertex2, step);
                    }
                    
                }

                for (int j = 0; j < edges.size(); j++) {
                    EdgeVisualizer* edge = edges[j];
                    if (edge->vertex1->cityIndex == vertex1->cityIndex || edge->vertex2->cityIndex == vertex1->cityIndex) {
                        continue;
                    }
                    
                    VertexVisualizer* v1 = edge->vertex1;
                    VertexVisualizer* v2 = edge->vertex2;

                    float edgedx = v2->x - v1->x;
                    float edgedy = v2->y - v1->y;

                    float vertexdx = vertex1->x - v1->x;
                    float vertexdy = vertex1->y - v1->y;

                    float dotProduct = vertexdx * edgedx + vertexdy * edgedy;
                    float edgeLength = std::sqrt(std::pow(edgedx, 2) + std::pow(edgedy, 2));

                    if (edgeLength == 0) {
                        continue;
                    }

                    float projection = dotProduct / edgeLength;

                    if (projection < 0 || projection > edgeLength) {
                        continue;
                    }

                    float edgeUnitX = edgedx / edgeLength;
                    float edgeUnitY = edgedy / edgeLength;
                    
                    float projx = v1->x + edgeUnitX * projection;
                    float projy = v1->y + edgeUnitY * projection;

                    float squareDist = std::pow(vertex1->x - projx, 2) + std::pow(vertex1->y - projy, 2);

                    if (squareDist < squareMin) {
                        oneDirMoveFurther(vertex1, projx, projy, step);
                    }
                }


                // Keep the dots from going off of the canvas
                float dx = 0;
                float dy = 0;
                
                if (vertex1->x < -1) {
                    dx = -(vertex1->x + 1);
                }
                else if (vertex1->x > 1) {
                    dx = -(vertex1->x - 1);
                }
                if (vertex1->y < -1) {
                    dy = -(vertex1->y + 1);
                }
                else if (vertex1->y > 1) {
                    dy = -(vertex1->y - 1);
                }

                for (int j = 0; j < vertices.size(); j++) {
                    vertices[i]->x += dx;
                    vertices[i]->y += dy;
                }
            }
        }
    }

    void defaultSpreadVertices() {
        spreadVertices(0.1, 0.4, 0.05, 1000);
    }

    GraphVisualizer(Graph* graph){
        for (int i = 0; i < graph->vertices.size(); i++) {
            Vertex* currentVertex = graph->vertices[i];
            VertexVisualizer* visualVertex = new VertexVisualizer(0.0, 0.0, currentVertex->data, currentVertex->index);
            vertices.append(visualVertex);
        }

        defaultSpreadVertices();
        
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