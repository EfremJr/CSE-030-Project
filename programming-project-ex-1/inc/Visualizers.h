#ifndef VISUALIZERS_H
#define VISUALIZERS_H

#include <iostream>
#include <Graph.h>

class VertexVisualizer {

public:
    int x;
    int y;
    std::string name;
    int cityIndex;

    VertexVisualizer(int x, int y, std::string name, int cityIndex){
        this->x = x;
        this->y = y;
        this->name = name;
        this->cityIndex = cityIndex;
    }
};

class EdgeVisualizer {
    
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
};

struct VisualPath {
    ArrayList<VertexVisualizer*> vertices;
    ArrayList<EdgeVisualizer*> edges;
};

class GraphVisualizer {

public:
    ArrayList<VertexVisualizer*> vertices;
    ArrayList<EdgeVisualizer*> edges;

    GraphVisualizer(Graph* graph){
        for (int i = 0; i < graph->vertices.size(); i++){
            Vertex* currentVertex = graph->vertices[i];
            //TODO: Calculate x and y for drawing vertex based on amount of vertices

            VertexVisualizer* visualVertex = new VertexVisualizer(0, 0, currentVertex->data, currentVertex->index);
            vertices.append(visualVertex);

            for (int j = 0; j < currentVertex->edgeList.size(); j++) {
                Edge* currentEdge = currentVertex->edgeList[j];
                Vertex* destination = currentEdge->to;

                // Condition only works if all edges are two-way
                if (currentVertex->index > destination->index){
                    VertexVisualizer* v1 = vertices[currentVertex->index];
                    VertexVisualizer* v2 = vertices[destination->index];
                    
                    EdgeVisualizer* visualEdge = new EdgeVisualizer(v1, v2, currentEdge->cost, currentEdge->time);
                    edges.append(visualEdge);
                }
            }
        }
    }

    VertexVisualizer* visualizedVertex(Vertex* vertex) { return vertices[vertex->index]; }
    
    EdgeVisualizer* visualizedEdge(Vertex* vertex1, Vertex* vertex2) {
        for (int i = 0; i < edges.size(); i++) {
            if ( (edges[i]->vertex1->cityIndex == vertex1->index && edges[i]->vertex2->cityIndex == vertex2->index)
                || (edges[i]->vertex1->cityIndex == vertex2->index && edges[i]->vertex2->cityIndex == vertex1->index)) {
                return edges[i];
            }
        }
        return nullptr;
    }

    VisualPath visualizePath(Waypoint* endpoint) {
        VisualPath path;

        Waypoint* temp = endpoint;
        while (temp->parent != nullptr) {
            VertexVisualizer* tempVertex = visualizedVertex(temp->vertex);
            EdgeVisualizer* edge = visualizedEdge(temp->vertex, temp->parent->vertex);

            path.vertices.append(tempVertex);
            path.edges.append(edge);
        }
        path.vertices.append(visualizedVertex(temp->parent->vertex));

        return path;
    }
};

#endif