#ifndef GRAPH_H
#define GRAPH_H

#include <LinkedList.h>
#include <ArrayList.h>
#include <HashTable.h>
#include <Queue.h>
#include <Stack.h>
#include <ostream>
#include <string>

struct Vertex;

struct Edge {
    Vertex *from;
    Vertex *to;
    int cost;
    int time;

    Edge(Vertex *from, Vertex *to, int cost, int time) {
        this->from = from;
        this->to = to;
        this->cost = cost;
        this->time = time;
    }
};

inline std::ostream &operator<<(std::ostream &os, Edge *e) {
    os << "(" << e->from << ", " << e->to << ") - " << "($" << e->cost << ", " << e->time << ")";

    return os;
}

struct Vertex {
    std::string data;
    int index;
    ArrayList<Edge *> edgeList;

    Vertex(std::string data, int index) { this->data = data; this->index = index;}

    ~Vertex() {
        for (int i = 0; i < edgeList.size(); i++) {
            delete edgeList[i];
        }
    }
};

inline std::ostream &operator<<(std::ostream &os, Vertex *v) {
    os << v->data;

    return os;
}

struct Waypoint {
    Waypoint *parent;
    Vertex *vertex;
    ArrayList<Waypoint *> children;
    int cost;
    int time;
    int partialCost;
    int partialTime;

    Waypoint(Vertex *v) {
        parent = nullptr;
        vertex = v;
        cost = 0;
        time = 0;
        partialCost = 0;
        partialTime = 0;
    }

    void expand() {
        for (int i = 0; i < vertex->edgeList.size(); i++) {
            bool exists = false;
            for (int j = 0; j < children.size(); j++) {
                if (vertex->edgeList[i]->to->index == children[j]->vertex->index) {
                    exists = true;
                    break;
                }
            }
            if (exists) { continue; }
            
            Waypoint *temp = new Waypoint(vertex->edgeList[i]->to);
            temp->parent = this;
            temp->cost = vertex->edgeList[i]->cost;
            temp->time = vertex->edgeList[i]->time;
            temp->partialCost = partialCost + temp->cost;
            temp->partialTime = partialTime + temp->time;
            children.append(temp);
        }
    }

    void reconstructChildren() {
        ArrayList<Waypoint*> reconstructedChildren;
        for (int i = 0; i < children.size(); i++) {
            if (children[i] != nullptr) {
                reconstructedChildren.append(children[i]);
            }
        }
        children = reconstructedChildren;
    }

    void setParent(Waypoint* parent) {
        this->parent = parent;
        this->partialCost = parent->partialCost + cost;
        this->partialTime = parent->partialTime + time;
    }

    void removeChild(Waypoint* child) {
        for (int i = 0; i < children.size(); i++) {
            if (children[i]->vertex->index == child->vertex->index) {
                children[i] = nullptr;
                break;
            }
        }
        reconstructChildren();
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

struct Path {
    ArrayList<Waypoint*> list;
    int cost;
    int time;

    Path() {}

    Path(Waypoint* end) {
        cost = end->partialCost;
        time = end->partialTime;

        Stack<Waypoint*> stack;
        while (end != nullptr) {
            stack.push(end);
            end = end->parent;
        }

        while (!stack.isEmpty()) {
            list.append(stack.pop());
        }
    }

    ~Path() {
        for (int i = 0; i < list.size(); i++) {
            delete list[i];
        }
    }

    bool contains(Waypoint* point) {
        for (int i = 0; i < list.size(); i++) {
            if (list[i]->vertex->index == point->vertex->index) {
                return true;
            }
        }
        return false;
    }
};

inline std::ostream &operator<<(std::ostream &os, Path *pa) {
    os << pa->list[0]->vertex->data;
    for (int i = 1; i < pa->list.size(); i++) {
        os << " -> " << pa->list[i]->vertex->data;
    }

    return os;
}

struct Graph {
    ArrayList<Vertex *> vertices;
    
    //Destructor for Graph
    ~Graph() {
        for (int i = 0; i < vertices.size(); i++) {
            delete vertices[i];
        }
    }

    void addVertex(Vertex *v) { vertices.append(v); }

    void addEdge(Vertex *x, Vertex *y, int cost, int time) {
        x->edgeList.append(new Edge(x, y, cost, time));
        y->edgeList.append(new Edge(y, x, cost, time));
    }

    void addDirectedEdge(Vertex *x, Vertex *y, int cost, int time) {
        x->edgeList.append(new Edge(x, y, cost, time));
    }

    void cleanupWaypoints(Waypoint* start, Path* exceptions) {
        for (int i = 0; i < start->children.size(); i++) {
            cleanupWaypoints(start->children[i], exceptions);
        }
        if (!exceptions->contains(start)) {
            delete start;
        }
        else {
            start->reconstructChildren();
        }
    }

    Path *bfs(Vertex *start, Vertex *destination) {
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
                Path* path = new Path(result);

                cleanupWaypoints(first, path);

                return path;
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

    Path *dfs(Vertex *start, Vertex *destination) {
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
                Path* path = new Path(result);
                cleanupWaypoints(first, path);
                return path;
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

    Path *ucsCost(Vertex *start, Vertex *destination) {
        std::cout << "Running Uniform Cost Search on edge costs" << std::endl;

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
                Path* path = new Path(result);
                cleanupWaypoints(first, path);
                return path;
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


                    // Sort into the frontier....
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

                    /*
                        Since we've already seen this vertex, we'll check if it's still in the frontier,
                        and whether the one in the frontier is worse than the one we just found.
                    */

                    int worsePathIndex = -1;

                    for (int k = 0; k < frontier.size(); k++) {
                        if (frontier[k]->vertex->index == result->children[i]->vertex->index) {
                            if (frontier[k]->partialCost > result->children[i]->partialCost) {
                                worsePathIndex = k;
                                break;
                            }
                        }
                    }

                    // If the waypoint in the frontier is worse, we replace it with the better one.
                    if (worsePathIndex != -1) {
                        Waypoint* worse = frontier[worsePathIndex];
                        Waypoint* better = result->children[i];
                        frontier[worsePathIndex] = better;

                        std::cout
                            << "Found another way to get to "
                            << better->vertex->data << ". Was "
                            << worse->partialCost << ", but now it is "
                            << better->partialCost << std::endl;

                        // Replacing the parent of the worse path's children
                        for (int k = 0; k < frontier.size(); k++) {
                            if (frontier[k]->parent->vertex->index == worse->vertex->index) {
                                worse->removeChild(frontier[k]);
                                
                                frontier[k]->setParent(better);
                                better->children.append(frontier[k]);
                            }
                        }

                        worse->parent->removeChild(worse);

                        Path* emptyPath = new Path();
                        cleanupWaypoints(worse, emptyPath);
                        delete emptyPath;

                        // Sort the frontier
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
    
    Path *ucsTime(Vertex *start, Vertex *destination) {
        std::cout << "Running Uniform Cost Search on edge times" << std::endl;

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
                Path* path = new Path(result);
                cleanupWaypoints(first, path);
                return path;
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


                    // Sort into the frontier....
                    int j = frontier.size() - 1;
                    while (j > 0 && frontier.data[j]->partialTime >
                                        frontier.data[j - 1]->partialTime) {

                        Waypoint *temp = frontier.data[j];
                        frontier.data[j] = frontier.data[j - 1];
                        frontier.data[j - 1] = temp;
                        j--;
                    }

                    seen.insert(result->children[i]->vertex->data);
                } else {

                    /*
                        Since we've already seen this vertex, we'll check if it's still in the frontier,
                        and whether the one in the frontier is worse than the one we just found.
                    */

                    int worsePathIndex = -1;

                    for (int k = 0; k < frontier.size(); k++) {
                        if (frontier[k]->vertex->index == result->children[i]->vertex->index) {
                            if (frontier[k]->partialTime > result->children[i]->partialTime) {
                                worsePathIndex = k;
                                break;
                            }
                        }
                    }

                    // If the waypoint in the frontier is worse, we replace it with the better one.
                    if (worsePathIndex != -1) {
                        Waypoint* worse = frontier[worsePathIndex];
                        Waypoint* better = result->children[i];
                        frontier[worsePathIndex] = better;

                        std::cout
                            << "Found another way to get to "
                            << better->vertex->data << ". Was "
                            << worse->partialTime << ", but now it is "
                            << better->partialTime << std::endl;

                        // Replacing the parent of the worse path's children
                        for (int k = 0; k < frontier.size(); k++) {
                            if (frontier[k]->parent->vertex->index == worse->vertex->index) {
                                worse->removeChild(frontier[k]);
                                
                                frontier[k]->setParent(better);
                                better->children.append(frontier[k]);
                            }
                        }

                        worse->parent->removeChild(worse);
                        
                        Path* emptyPath = new Path();
                        cleanupWaypoints(worse, emptyPath);
                        delete emptyPath;

                        // Sort the frontier
                        for (int a = 1; a < frontier.size(); a++) {
                            int b = a;
                            while (b > 0 && frontier[b]->partialTime >
                                                frontier[b - 1]->partialTime) {
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
                          << frontier[k]->partialTime << ") ";
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