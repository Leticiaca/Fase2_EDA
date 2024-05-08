#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 100

// Estrutura para representar um nó (vértice) de um grafo
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

// Estrutura para representar um grafo
typedef struct Graph {
    int numVertices;
    Node** adjLists; // Lista de adjacências
} Graph;

// Cria um novo nó
Node* createNode(int v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}

//Estrutura para acompanhar a soma maior e o caminho correspondente 
typedef struct {
    int maxSum;
    int* bestPath;
    int bestPathSize;
};

// Cria um novo grafo
Graph* createGraph(int numVertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) {
        fprintf(stderr, "Erro ao alocar memória para o grafo.\n");
        exit(EXIT_FAILURE);
    }

    graph->numVertices = numVertices;
    graph->adjLists = (Node**)calloc(numVertices, sizeof(Node*)); // Usa calloc para inicializar com NULL
    if (!graph->adjLists) {
        fprintf(stderr, "Erro ao alocar memória para listas de adjacências.\n");
        free(graph);
        exit(EXIT_FAILURE);
    }

    return graph;
}

// Adiciona uma aresta ao grafo
void addEdge(Graph* graph, int src, int dest) {
    Node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
}

// Remove uma aresta do grafo
void removeEdge(Graph* graph, int src, int dest) {
    Node* current = graph->adjLists[src];
    Node* prev = NULL;

    while (current && current->vertex != dest) {
        prev = current;
        current = current->next;
    }

    if (current) {
        if (prev) {
            prev->next = current->next;
        }
        else {
            graph->adjLists[src] = current->next;
        }
        free(current);
    }
}

// Imprime o grafo
void printGraph(const Graph* graph, const int matrix[MAX_SIZE][MAX_SIZE], int size) {
    for (int v = 0; v < graph->numVertices; ++v) {
        printf("\nLista de adjacências do vértice %d (Valor na matriz: %d):\n", v, matrix[v / size][v % size]);
        Node* temp = graph->adjLists[v];
        while (temp) {
            printf("-> %d (Valor na matriz: %d)\n", temp->vertex, matrix[temp->vertex / size][temp->vertex % size]);
            temp = temp->next;
        }
    }
    printf("\n");
}

// Liberta a memória alocada para o grafo
void freeGraph(Graph* graph) {
    if (!graph) return;

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

// Busca em Profundidade
void DFS(Graph* graph, int v, bool* visited, int* path, int pathIndex, const int matrix[MAX_SIZE][MAX_SIZE], int size, PathInfo* pathInfo) {
    visited[v] = true;
    path[pathIndex] = v;

    int sum = 0;
    for (int i = 0; i <= pathIndex; i++) {
        int row = path[i] / size;
        int col = path[i] % size;
        sum += matrix[row][col];
    }

    if (sum > pathInfo->maxSum) {
        pathInfo->maxSum = sum;
        memcpy(pathInfo->bestPath, path, (pathIndex + 1) * sizeof(int));
        pathInfo->bestPathSize = pathIndex + 1;
    }


    Node* temp = graph->adjLists[v];
    while (temp) {
        int adjVertex = temp->vertex;
        if (!visited[adjVertex]) {
            DFS(graph, adjVertex, visited, path, pathIndex + 1, matrix, size);
        }
        temp = temp->next;
    }

    visited[v] = false;
}

// Função para encontrar todos os caminhos possíveis no grafo
void findAllPaths(Graph* graph, const int matrix[MAX_SIZE][MAX_SIZE], int size) {
    bool* visited = (bool*)calloc(graph->numVertices, sizeof(bool));
    int* path = (int*)malloc(graph->numVertices * sizeof(int));
    if (!visited || !path) {
        fprintf(stderr, "Erro ao alocar memória para busca.\n");
        exit(EXIT_FAILURE);
    }

    DFS(graph, 0, visited, path, 0, matrix, size);

    free(visited);
    free(path);
}

//Função para encontrar o caminho com a maior soma 
void findMaxPath(Graph* graph, const int matrix[MAX_SIZE][MAX_SIZE], int size) {
    bool* visited = (bool*)calloc(graph->numVertices, sizeof(bool));
    int* path = (int*)malloc(graph->numVertices * sizeof(int));
    PathInfo pathInfo = { 0, malloc(graph->numVertices * sizeof(int)), 0 };

    DFS(graph, 0, visited, path, 0, matrix, size, &pathInfo);

    printf("Caminho com a maior soma e: ");
    for (int i = 0; i < pathInfo.bestpathSize; i++) {
        printf("%d", pathInfo.bestPath[i]);
        if (i < pathInfo.bestpathSize - 1) printf("->");
    }

    printf("\n Maior soma dos valores: %d", pathInfo.maxSum);

    free(visited);
    free(path);
    free(pathInfo.bestPath);

}

// Carrega a matriz do arquivo
void loadMatrixFromFile(Graph* graph, const char* filename, int size, int matrix[MAX_SIZE][MAX_SIZE]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", filename);
        return;
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (fscanf(file, "%d;", &matrix[i][j]) != 1) {
                fprintf(stderr, "Erro ao ler o arquivo.\n");
                fclose(file);
                return;
            }
        }
    }

    fclose(file);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i < size - 1) { // Adiciona uma aresta para o elemento abaixo
                addEdge(graph, i * size + j, (i + 1) * size + j);
            }
            if (j < size - 1) { // Adiciona uma aresta para o elemento à direita
                addEdge(graph, i * size + j, i * size + j + 1);
            }
        }
    }
}

int main() {
    int size = 5; // Tamanho da matriz
    int matrix[MAX_SIZE][MAX_SIZE];

    // Preenche a matriz com valores de teste
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = i * size + j; // Exemplo de preenchimento
        }
    }

    // Cria um grafo baseado na matriz
    Graph* graph = createGraph(size * size);

    // Carrega a matriz do arquivo
    loadMatrixFromFile(graph, "matrix.txt", size, matrix);

    // Imprime o grafo
    printGraph(graph, matrix, size);

    // Encontra todos os caminhos possíveis
    findAllPaths(graph, matrix, size);

    // Liberta a memória
    freeGraph(graph);

    return 0;
}
