#include <stdio.h>
#include <stdlib.h>


// Estrutura para representar um nó (vértice) em um grafo
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

// Estrutura para representar um grafo
typedef struct Graph {
    int numVertices;
    Node** adjLists; // Lista de adjacências
} Graph;

// Função para criar um novo nó (vértice)
Node* createNode(int v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}


#pragma region Graph

// Função para criar um grafo com 'V' vértices
Graph* createGraph(int V) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numVertices = V;

    // Cria uma matriz de listas de adjacências com 'V' elementos
    graph->adjLists = (Node**)malloc(V * sizeof(Node*));

    // Inicializa cada lista de adjacências como vazia
    int i;
    for (i = 0; i < V; i++) {
        graph->adjLists[i] = NULL;
    }

    return graph;
}


// Função para imprimir o grafo
void printGraph(Graph* graph) {
    int v;
    for (v = 0; v < graph->numVertices; v++) {
        Node* temp = graph->adjLists[v];
        printf("\n Lista de adjacências do vértice %d\n ", v);
        while (temp) {
            printf("-> %d", temp->vertex);
            temp = temp->next;
        }
        printf("\n");
    }
}

// Função para remover o grafo da memória
void freeGraph(Graph* graph) {
    if (graph) {
        if (graph->adjLists) {
            for (int i = 0; i < graph->numVertices; ++i) {
                Node* current = graph->adjLists[i];
                while (current) {
                    Node* temp = current;
                    current = current->next;
                    free(temp);
                }
            }
            free(graph->adjLists);
        }
        free(graph);
    }
}

#pragma endregion

#pragma region Edge

// Função para adicionar uma aresta de 'src' para 'dest' no grafo
void addEdge(Graph* graph, int src, int dest) {
    // Adiciona uma aresta de src para dest
    Node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
}

// Função para remover uma aresta de 'src' para 'dest' do grafo
void removeEdge(Graph* graph, int src, int dest) {
    Node* current = graph->adjLists[src];
    Node* prev = NULL;

    while (current != NULL && current->vertex != dest) {
        prev = current;
        current = current->next;
    }

    if (current != NULL) {
        if (prev != NULL) {
            prev->next = current->next;
        }
        else {
            graph->adjLists[src] = current->next;
        }
        free(current);
    }
}

#pragma endregion

#pragma region Vertice

#pragma endregion

#pragma region Algorithm

#pragma endregion

#pragma region Main

int main() {
    int V = 5;
    Graph* graph = createGraph(V);
    addEdge(graph, 0, 1);
    addEdge(graph, 0, 2);
    addEdge(graph, 1, 3);
    addEdge(graph, 2, 4);
    addEdge(graph, 3, 4);
    addEdge(graph, 4, 0);

    printGraph(graph);

    removeEdge(graph, 0, 1);
    removeEdge(graph, 3, 4);

    printf("\nGrafo após remoção de algumas arestas:\n");
    printGraph(graph);

    freeGraph(graph);

    return 0;
}


#pragma endregion



