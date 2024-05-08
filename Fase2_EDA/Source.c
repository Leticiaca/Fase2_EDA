#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 100

// Estrutura para representar um n� (v�rtice) de um grafo
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

// Estrutura para representar um grafo
typedef struct Graph {
    int numVertices;
    Node** adjLists; // Lista de adjac�ncias
} Graph;

//Estrutura para acompanhar a soma maior e o caminho correspondente 
typedef struct {
    int maxSum;
    int* bestPath;
    int bestPathSize;
} PathInfo;

// Cria um novo n�
Node* createNode(int v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = v;
    newNode->next = NULL;
    return newNode;
}



// Cria um novo grafo
Graph* createGraph(int numVertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) {
        fprintf(stderr, "Erro ao alocar mem�ria para o grafo.\n");
        exit(EXIT_FAILURE);
    }

    graph->numVertices = numVertices;
    graph->adjLists = (Node**)calloc(numVertices, sizeof(Node*)); // Usa calloc para inicializar com NULL
    if (!graph->adjLists) {
        fprintf(stderr, "Erro ao alocar mem�ria para listas de adjac�ncias.\n");
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
        printf("\nLista de adjac�ncias do v�rtice %d (Valor na matriz: %d):\n", v, matrix[v / size][v % size]);
        Node* temp = graph->adjLists[v];
        while (temp) {
            printf("-> %d (Valor na matriz: %d)\n", temp->vertex, matrix[temp->vertex / size][temp->vertex % size]);
            temp = temp->next;
        }
    }
    printf("\n");
}

// Liberta a mem�ria alocada para o grafo
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
void DFS(Graph* graph, int v, bool* visited, int* path, int pathIndex, int currentSum, const int matrix[MAX_SIZE][MAX_SIZE], int size, PathInfo* pathInfo) {
    visited[v] = true;
    path[pathIndex] = v;
    currentSum += matrix[v / size][v % size];

    if (currentSum > pathInfo->maxSum) {
        pathInfo->maxSum = currentSum;
        memcpy(pathInfo->bestPath, path, (pathIndex + 1) * sizeof(int));
        pathInfo->bestPathSize = pathIndex + 1;
    }

    Node* temp = graph->adjLists[v];
    while (temp) {
        if (!visited[temp->vertex]) {
            DFS(graph, temp->vertex, visited, path, pathIndex + 1, currentSum, matrix, size, pathInfo);
        }
        temp = temp->next;
    }
    visited[v] = false;
}



void findMaxPath(Graph* graph, const int matrix[MAX_SIZE][MAX_SIZE], int size) {
    // Inicializa o array de visitados e o caminho
    bool* visited = calloc(graph->numVertices, sizeof(bool));
    int* path = malloc(graph->numVertices * sizeof(int));

    // Inicializa a estrutura PathInfo
    PathInfo pathInfo;
    pathInfo.maxSum = 0;
    pathInfo.bestPath = malloc(graph->numVertices * sizeof(int));  // Alocar espa�o para o melhor caminho
    pathInfo.bestPathSize = 0;

    // Iterar sobre todos os v�rtices, usando DFS para encontrar o caminho com a maior soma
    for (int i = 0; i < graph->numVertices; i++) {
        DFS(graph, i, visited, path, 0, 0, matrix, size, &pathInfo);
    }

    // Imprimir o resultado
    printf("Caminho com a maior soma �: ");
    for (int i = 0; i < pathInfo.bestPathSize; i++) {
        printf("%d ", pathInfo.bestPath[i]);
        if (i < pathInfo.bestPathSize - 1) printf("-> ");
    }
    printf("\nMaior soma dos valores: %d\n", pathInfo.maxSum);

    // Limpeza: liberar mem�ria alocada
    free(visited);
    free(path);
    free(pathInfo.bestPath);
}



// Fun��o para encontrar todos os caminhos poss�veis no grafo
void findAllPaths(Graph* graph, const int matrix[MAX_SIZE][MAX_SIZE], int size) {
    bool* visited = calloc(graph->numVertices, sizeof(bool));
    int* path = malloc(graph->numVertices * sizeof(int));
    PathInfo pathInfo = { 0, malloc(graph->numVertices * sizeof(int)), 0 };

    for (int i = 0; i < graph->numVertices; i++) {
        DFS(graph, i, visited, path, 0, 0, matrix, size, &pathInfo);  // Inclui argumento para soma atual e PathInfo
    }

    // Opcional: Mostrar resultados ou limpeza adicional
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
            if (j < size - 1) { // Adiciona uma aresta para o elemento � direita
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
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i < size - 1) { addEdge(graph, i * size + j, (i + 1) * size + j); }
            if (j < size - 1) { addEdge(graph, i * size + j, i * size + j + 1); }
        }
    }

    // Carrega a matriz do arquivo
    loadMatrixFromFile(graph, "matrix.txt", size, matrix);

    // Imprime o grafo
    printGraph(graph, matrix, size);

    // Encontra todos os caminhos poss�veis
    findAllPaths(graph, matrix, size);

    // Liberta a mem�ria
    freeGraph(graph);

    return 0;
}
