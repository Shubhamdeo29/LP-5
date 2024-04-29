#include <iostream>
#include <vector>
#include <queue>
#include <omp.h>

using namespace std;

// Define a Node structure for the graph
struct Node {
    int data;
    bool visited;
    vector<Node*> neighbors;
    Node(int val) : data(val), visited(false) {}
};

// Parallel Breadth First Search
void parallelBFS(Node* startNode) {
    queue<Node*> bfsQueue;
    bfsQueue.push(startNode);
    startNode->visited = true;

    while (!bfsQueue.empty()) {
        #pragma omp parallel
        {
            #pragma omp single nowait
            {
                Node* currentNode = bfsQueue.front();
                bfsQueue.pop();
                cout << "Visiting Node: " << currentNode->data << endl;

                // Enqueue unvisited neighbors
                for (Node* neighbor : currentNode->neighbors) {
                    if (!neighbor->visited) {
                        neighbor->visited = true;
                        bfsQueue.push(neighbor);
                    }
                }
            }
        }
    }
}

// Parallel Depth First Search
void parallelDFS(Node* currentNode) {
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            cout << "Visiting Node: " << currentNode->data << endl;
            currentNode->visited = true;

            for (Node* neighbor : currentNode->neighbors) {
                if (!neighbor->visited) {
                    #pragma omp task
                    {
                        parallelDFS(neighbor);
                    }
                }
            }
        }
    }
}

int main() {
    int numNodes, numEdges;
    cout << "Enter the number of nodes in the graph: ";
    cin >> numNodes;

    vector<Node*> nodes(numNodes + 1, nullptr); // Index 0 is not used
    for (int i = 1; i <= numNodes; ++i) {
        nodes[i] = new Node(i);
    }

    cout << "Enter the number of edges: ";
    cin >> numEdges;

    cout << "Enter the edges (node1 node2):" << endl;
    for (int i = 0; i < numEdges; ++i) {
        int node1, node2;
        cin >> node1 >> node2;
        nodes[node1]->neighbors.push_back(nodes[node2]);
        nodes[node2]->neighbors.push_back(nodes[node1]); // Undirected graph
    }

    int startNode;
    cout << "Enter the starting node for BFS: ";
    cin >> startNode;
    cout << "Parallel BFS:" << endl;
    parallelBFS(nodes[startNode]);

    // Resetting visited flags for DFS
    for (int i = 1; i <= numNodes; ++i) {
        nodes[i]->visited = false;
    }

    cout << "\nEnter the starting node for DFS: ";
    cin >> startNode;
    cout << "Parallel DFS:" << endl;
    parallelDFS(nodes[startNode]);

    // Free allocated memory
    for (int i = 1; i <= numNodes; ++i) {
        delete nodes[i];
    }

    return 0;
}
