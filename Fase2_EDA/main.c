#include <stdio.h>
#include <stdlib.h>


// Estrutura para representar um n� (v�rtice) em um grafo
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

// Estrutura para representar um grafo
typedef struct Graph {
    int numVertices;
    Node** adjLists; // Lista de adjac�ncias
} Graph;

// Fun��o para criar um novo n� (v�rtice)
Node* createNode(int v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}


#pragma region Graph

// Fun��o para criar um grafo com 'V' v�rtices
Graph* createGraph(int V) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numVertices = V;

    // Cria uma matriz de listas de adjac�ncias com 'V' elementos
    graph->adjLists = (Node**)malloc(V * sizeof(Node*));

    // Inicializa cada lista de adjac�ncias como vazia
    int i;
    for (i = 0; i < V; i++) {
        graph->adjLists[i] = NULL;
    }

    return graph;
}


// Fun��o para imprimir o grafo
void printGraph(Graph* graph) {
    int v;
    for (v = 0; v < graph->numVertices; v++) {
        Node* temp = graph->adjLists[v];
        printf("\n Lista de adjac�ncias do v�rtice %d\n ", v);
        while (temp) {
            printf("-> %d", temp->vertex);
            temp = temp->next;
        }
        printf("\n");
    }
}

// Fun��o para remover o grafo da mem�ria
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

// Fun��o para adicionar uma aresta de 'src' para 'dest' no grafo
void addEdge(Graph* graph, int src, int dest) {
    // Adiciona uma aresta de src para dest
    Node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
}

// Fun��o para remover uma aresta de 'src' para 'dest' do grafo
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

    printf("\nGrafo ap�s remo��o de algumas arestas:\n");
    printGraph(graph);

    freeGraph(graph);

    return 0;
}


#pragma endregion



