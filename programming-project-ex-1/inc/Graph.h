#ifndef GRAPH_H
#define GRAPH_H

#include <LinkedList.h>
#include <ArrayList.h>
#include <HashTable.h>
#include <Queue.h>
#include <Stack.h>
#include <ostream>
#include <string>

struct Edge;

struct Vertex {
    std::string data;
    ArrayList<Edge *> edgeList;

    Vertex(std::string data) { this->data = data; }
};

inline std::ostream &operator<<(std::ostream &os, Vertex *v) {
    os << v->data;

    return os;
}

struct Edge {
    Vertex *from;
    Vertex *to;
    int weight;

    Edge(Vertex *from, Vertex *to, int weight) {
        this->from = from;
        this->to = to;
        this->weight = weight;
    }
};

inline std::ostream &operator<<(std::ostream &os, Edge *e) {
    os << "(" << e->from << ", " << e->to << ") - " << e->weight;

    return os;
}

struct Waypoint {
    Waypoint *parent;
    Vertex *vertex;
    ArrayList<Waypoint *> children;
    int partialCost;
    int weight;

    Waypoint(Vertex *v) {
        parent = nullptr;
        vertex = v;
        weight = 0;
        partialCost = 0;
    }

    void expand() {
        for (int i = 0; i < vertex->edgeList.size(); i++) {
            Waypoint *temp = new Waypoint(vertex->edgeList[i]->to);
            temp->parent = this;
            temp->weight = vertex->edgeList[i]->weight;
            temp->partialCost = partialCost + vertex->edgeList[i]->weight;
            children.append(temp);
        }
    }
};

inline std::ostream &operator<<(std::ostream &os, Waypoint *wp) {
    std::string p = "null";
    if (wp->parent != nullptr) {
        p = wp->parent->vertex->data;
    }

    os << p << " -> " << wp->vertex->data;

    return os;
}

struct Graph {
    ArrayList<Vertex *> vertices;

    void addVertex(Vertex *v) { vertices.append(v); }

    void addEdge(Vertex *x, Vertex *y, int w) {
        x->edgeList.append(new Edge(x, y, w));
        y->edgeList.append(new Edge(y, x, w));
    }

    void addDirectedEdge(Vertex *x, Vertex *y, int w) {
        x->edgeList.append(new Edge(x, y, w));
    }

    Waypoint *bfs(Vertex *start, Vertex *destination) {
        std::cout << "Running Breadth-First Search" << std::endl;
        Queue<Waypoint *> frontier;
        HashTable<std::string> seen;

        Waypoint *first = new Waypoint(start);

        frontier.enqueue(first);
        seen.insert(first->vertex->data);

        Waypoint *result = nullptr;

        while (!frontier.isEmpty()) {
            result = frontier.dequeue();

            if (result->vertex == destination) {
                return result;
            }

            result->expand();
            // Get the neighbors of the current vertex
            // that we are on...

            // The neighbors are stored in the result->children array
            std::cout << std::endl
                      << "Expanding " << result->vertex->data << std::endl;

            for (int i = 0; i < result->children.size(); i++) {
                // For every child of the result node
                // If we have not seen it
                // We add it to the frontier (as a queue)
                // We mark it as seen
                if (!seen.search(result->children[i]->vertex->data)) {
                    std::cout << "Adding " << result->children[i]->vertex->data
                              << std::endl;
                    frontier.enqueue(result->children[i]);
                    seen.insert(result->children[i]->vertex->data);
                }
            }

            std::cout << std::endl << "Frontier" << std::endl;

            Link<Waypoint *> *temp = frontier.list.front;
            while (temp != nullptr) {
                std::cout << "(" << temp->data->vertex->data << ","
                          << temp->data->partialCost << ")";
                temp = temp->next;

                if (temp != nullptr) {
                    std::cout << ", ";
                } else {
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
        }

        return nullptr;
    }

    Waypoint *dfs(Vertex *start, Vertex *destination) {
        std::cout << "Running Depth-First Search" << std::endl;

        Stack<Waypoint *> frontier;
        HashTable<std::string> seen;

        Waypoint *first = new Waypoint(start);

        frontier.push(first);
        seen.insert(first->vertex->data);

        Waypoint *result = nullptr;

        while (!frontier.isEmpty()) {
            result = frontier.pop();

            if (result->vertex == destination) {
                return result;
            }

            result->expand();

            std::cout << std::endl
                      << "Expanding " << result->vertex->data << std::endl;

            for (int i = 0; i < result->children.size(); i++) {
                if (!seen.search(result->children[i]->vertex->data)) {
                    std::cout << "Adding " << result->children[i]->vertex->data
                              << std::endl;
                    frontier.push(result->children[i]);
                    seen.insert(result->children[i]->vertex->data);
                }
            }

            std::cout << std::endl << "Frontier" << std::endl;

            Link<Waypoint *> *temp = frontier.front;
            while (temp != nullptr) {
                std::cout << "(" << temp->data->vertex->data << ","
                          << temp->data->partialCost << ")";
                temp = temp->next;

                if (temp != nullptr) {
                    std::cout << ", ";
                } else {
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
        }

        return nullptr;
    }

    Waypoint *ucs(Vertex *start, Vertex *destination) {
        std::cout << "Running Uniform Cost Search" << std::endl;

        // Should be a priority queue
        ArrayList<Waypoint *> frontier;
        HashTable<std::string> seen;

        Waypoint *first = new Waypoint(start);

        frontier.append(first);
        seen.insert(first->vertex->data);

        Waypoint *result = nullptr;

        while (frontier.size() != 0) {
            result = frontier.removeLast();

            if (result->vertex == destination) {
                return result;
            }

            result->expand();

            std::cout << "Expanding " << result->vertex->data << std::endl;

            for (int i = 0; i < result->children.size(); i++) {
                // Look at each child
                if (!seen.search(result->children[i]->vertex->data)) {
                    // If not in the seen list, let's add it
                    std::cout << "Adding " << result->children[i]->vertex->data
                              << std::endl;
                    frontier.append(result->children[i]);


                    // Sort the frontier....
                    int j = frontier.size() - 1;
                    while (j > 0 && frontier.data[j]->partialCost >
                                        frontier.data[j - 1]->partialCost) {

                        Waypoint *temp = frontier.data[j];
                        frontier.data[j] = frontier.data[j - 1];
                        frontier.data[j - 1] = temp;
                        j--;
                    }

                    seen.insert(result->children[i]->vertex->data);
                } else {
                    // If it is in the seen list, we may have to do some work

                    // First we will check if it is still in the frontier but
                    // with a higher partial cost
                    Waypoint *worsePath = nullptr;

                    for (int k = 0; k < frontier.size(); k++) {
                        if (frontier[k]->vertex->data ==
                            result->children[i]->vertex->data) {
                            if (frontier[k]->partialCost >
                                result->children[i]->partialCost) {
                                worsePath = frontier[k];
                                // The same node was visited before,
                                // but with a higher partial cost
                                break;
                            }
                        }
                    }

                    // If we had a worse node before, we need to change it.
                    if (worsePath) {
                        std::cout
                            << "Found another way to get to "
                            << result->children[i]->vertex->data << ". Was "
                            << worsePath->partialCost << ", but now it is "
                            << result->children[i]->partialCost << std::endl;

                        // Make it so that the children of the worse waypoint
                        // become our children
                        for (int k = 0; k < frontier.size(); k++) {
                            if (frontier[k]->parent->vertex->data ==
                                result->children[i]->vertex->data) {
                                frontier[k]->parent = result->children[i];
                            }
                        }

                        // Replace the worse one with the better one
                        for (int k = 0; k < frontier.size(); k++) {
                            if (frontier[k]->vertex->data ==
                                result->children[i]->vertex->data) {
                                delete frontier[k];
                                frontier[k] = result->children[i];
                                break;
                            }
                        }

                        // Sort the frontier because the replacement above
                        // may have caused things to fall out of order
                        for (int a = 1; a < frontier.size(); a++) {
                            int b = a;
                            while (b > 0 && frontier[b]->partialCost >
                                                frontier[b - 1]->partialCost) {
                                Waypoint *x = frontier[b];
                                frontier[b] = frontier[b - 1];
                                frontier[b - 1] = x;
                                b--;
                            }
                        }
                    }
                }
            }

            std::cout << std::endl << "Frontier" << std::endl;

            for (int k = frontier.size() - 1; k >= 0; k--) {
                std::cout << "(" << frontier[k]->vertex->data << ", "
                          << frontier[k]->partialCost << ") ";
                if (k > 0) {
                    std::cout << ", ";
                } else {
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
        }

        return nullptr;
    }
};

inline std::ostream &operator<<(std::ostream &os, const Graph &g) {
    for (int i = 0; i < g.vertices.size(); i++) {
        os << g.vertices[i]->edgeList << std::endl;
    }

    return os;
}

#endif