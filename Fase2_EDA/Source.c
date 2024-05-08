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

// Cria um novo nó para a lista de adjacências
Node* createNode(int vertex) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Erro ao alocar memória para o nó.\n");
        exit(EXIT_FAILURE);
    }
    newNode->vertex = vertex;
    newNode->next = NULL;
    return newNode;
}

// Cria um novo grafo com um número especificado de vértices
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
    if (!graph || !graph->adjLists) return;

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

// Imprime a lista de adjacências para cada vértice no grafo
void printGraph(const Graph* graph, const int matrix[MAX_SIZE][MAX_SIZE], int size) {
    if (!graph || !graph->adjLists) return;

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

// Liberta a memória alocada para o grafo e suas listas de adjacências
void freeGraph(Graph* graph) {
    if (!graph || !graph->adjLists) return;

    for (int i = 0; i < graph->numVertices; i++) {
        Node* current = graph->adjLists[i];
        while (current) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(graph->adjLists);
    free(graph);
}

// Busca em profundidade (DFS) para encontrar o maior caminho com maior soma
void DFS(Graph* graph, int v, bool* visited, int* currentPath, int currentPathIndex,
    const int matrix[MAX_SIZE][MAX_SIZE], int size,
    int* maxSum, int* bestPath, int* bestPathLength) {
    visited[v] = true;
    currentPath[currentPathIndex] = v;

    // Se atingir o último vértice, calcula a soma do caminho
    if (v == graph->numVertices - 1) {
        int currentSum = 0;
        printf("Caminho encontrado: ");
        for (int i = 0; i <= currentPathIndex; i++) {
            int row = currentPath[i] / size;
            int col = currentPath[i] % size;
            currentSum += matrix[row][col];
            printf("%d ", currentPath[i]);
            if (i < currentPathIndex) {
                printf("-> ");
            }
        }
        printf("\nSoma dos valores dos vértices no caminho: %d\n", currentSum);

        // Atualiza o caminho com maior soma, se necessário
        if (currentSum > *maxSum) {
            *maxSum = currentSum;
            *bestPathLength = currentPathIndex + 1;
            for (int i = 0; i <= currentPathIndex; i++) {
                bestPath[i] = currentPath[i];
            }
        }
    }

    // Explora os vértices adjacentes
    Node* temp = graph->adjLists[v];
    while (temp) {
        int adjVertex = temp->vertex;
        if (!visited[adjVertex]) {
            DFS(graph, adjVertex, visited, currentPath, currentPathIndex + 1,
                matrix, size, maxSum, bestPath, bestPathLength);
        }
        temp = temp->next;
    }

    // Desmarca o vértice para permitir outras buscas
    visited[v] = false;
}

// Função para imprimir o caminho com maior soma
void printBestPath(int* path, int length, int maxSum) {
    printf("\nMaior caminho encontrado: ");
    for (int i = 0; i < length; i++) {
        printf("%d", path[i]);
        if (i < length - 1) {
            printf(" -> ");
        }
    }
    printf("\nMaior soma: %d\n", maxSum);
}

// Carrega a matriz do arquivo e adiciona arestas ao grafo
void loadMatrixFromFile(Graph* graph, const char* filename, int size,
    int matrix[MAX_SIZE][MAX_SIZE]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", filename);
        return;
    }

    // Lê a matriz do arquivo
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

    // Adiciona arestas ao grafo com base na matriz
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
    // Tamanho da matriz e grafo
    int size = 5; // Pode ser ajustado conforme necessário
    int matrix[MAX_SIZE][MAX_SIZE];

    Graph* graph = createGraph(size * size); // Cria um grafo com vértices do tamanho da matriz

    // Carrega matriz do arquivo e adiciona arestas ao grafo
    loadMatrixFromFile(graph, "matrix.txt", size, matrix);

    // Imprime o grafo
    printGraph(graph, matrix, size);

    int currentPath[MAX_SIZE];
    int bestPath[MAX_SIZE];
    int bestPathLength = 0;
    int maxSum = 0;

    bool visited[MAX_SIZE] = { false }; // Inicializa array de visitados


    // Realiza DFS para encontrar o caminho com maior soma
    DFS(graph, 0, visited, currentPath, 0, matrix, size,
        &maxSum, bestPath, &bestPathLength);

    // Imprime o melhor caminho encontrado
    printBestPath(bestPath, bestPathLength, maxSum);

    // Liberta a memória alocada para o grafo
    freeGraph(graph);

    return 0;
}
