#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 100

//Ponto 1: Definição de uma estrutura de dados para representar um grafo

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

// Ponto 1.1 adicionar aresta ao grafo
void addEdge(Graph* graph, int src, int dest) {
    Node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
}

// Ponto 1.2 remover aresta do grafo
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

// Ponto 2: Listagem de adjacentes
void printGraph(const Graph* graph, const int matrix[MAX_SIZE][MAX_SIZE], int size) {
    if (!graph || !graph->adjLists) return;

    for (int v = 0; v < graph->numVertices; ++v) {
        printf("\nLista de adjacências do vertice %d (Valor na matriz: %d):\n", v, matrix[v / size][v % size]);
        Node* temp = graph->adjLists[v];
        while (temp) {
            printf("-> %d (Valor na matriz: %d)\n", temp->vertex, matrix[temp->vertex / size][temp->vertex % size]);
            temp = temp->next;
        }
    }
    printf("\n");
}


// Ponto 3: Carregamento de um grafo a partir de um ficheiro
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


// Ponto 4: Implementação de manipulação: procura em profundidade
void DFS(Graph* graph, int v, bool* visited, int* currentPath, int currentPathIndex,
    const int matrix[MAX_SIZE][MAX_SIZE], int size,
    int* maxSum, int* bestPath, int* bestPathLength) {
    if (currentPathIndex >= MAX_SIZE) {
        fprintf(stderr, "Erro: caminho excedeu o tamanho máximo.\n");
        return;
    }

    currentPath[currentPathIndex] = v;

    visited[v] = true;
    //Ponto 4.1: Encontrar todos os caminhos possíveis
    if (v == graph->numVertices - 1) {
        int currentSum = 0;
        printf("\nCaminho encontrado: ");
        for (int i = 0; i <= currentPathIndex; i++) {
            int row = currentPath[i] / size;
            int col = currentPath[i] % size;
            currentSum += matrix[row][col];
            printf("%d ", currentPath[i]);
            if (i < currentPathIndex) {
                printf("-> ");
            }
        }
        printf("\nSoma dos valores dos vertices do caminho: %d\n", currentSum);

        // Atualiza o caminho com maior soma, se necessário
        if (maxSum != NULL && currentSum > *maxSum) {
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

// Ponto 5: Encontrar o melhor caminho com maior soma
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

// Liberta a memória alocada para um grafo
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


int main() {
    int size = 5; // Pode ser ajustado conforme necessário
    int matrix[MAX_SIZE][MAX_SIZE];
    Graph* graph = createGraph(size * size); // Cria um grafo com vértices do tamanho da matriz

    loadMatrixFromFile(graph, "matrix.txt", size, matrix);

    int option;

    while (true) {
        printf("\nMenu de Opcoes:\n");
        printf("1. Lista de adjacentes\n");
        printf("2. Adicionar uma aresta ao grafo\n");
        printf("3. Remover uma aresta do grafo\n");
        printf("4. Todos os caminhos possiveis\n");
        printf("5. Encontrar o melhor caminho com maior soma\n");
        printf("6. Sair\n");
        printf("Escolha uma opcao: ");


        if (scanf("%d", &option) != 1) {
            fprintf(stderr, "Entrada inválida.\n");
            exit(EXIT_FAILURE);
        }

        switch (option) {
        case 1:
            printGraph(graph, matrix, size);
            break;

        case 2:
            printf("Digite a origem da aresta: ");
            int src;
            scanf("%d", &src);
            printf("Digite o destino da aresta: ");
            int dest;
            scanf("%d", &dest);
            addEdge(graph, src, dest);
            printf("Aresta adicionada de %d para %d.\n", src, dest);
            break;

        case 3:
            printf("Digite a origem da aresta para remover: ");
            scanf("%d", &src);
            printf("Digite o destino da aresta para remover: ");
            scanf("%d", &dest);
            removeEdge(graph, src, dest);
            printf("Aresta removida de %d para %d.\n", src, dest);
            break;

        case 4:
            printf("\nCaminhos possiveis a partir do primeiro vertice:\n");
            bool visited[MAX_SIZE] = { false }; // Marca de vértices visitados
            int currentPath[MAX_SIZE];
            int bestPath[MAX_SIZE];
            int currentPathIndex = 0;

            DFS(graph, 0, visited, currentPath, currentPathIndex, matrix, size, NULL, NULL, NULL);
            break;

        case 5:
        {
            int maxSum = 0;
            int bestPathLength = 0;
            int bestPath[MAX_SIZE];

            DFS(graph, 0, visited, currentPath, currentPathIndex, matrix, size, &maxSum, bestPath, &bestPathLength);

            printBestPath(bestPath, bestPathLength, maxSum);
            break;
        }

        case 6:
            freeGraph(graph);
            printf("A sair...\n");
            exit(EXIT_SUCCESS);
            break;

        default:
            printf("Opção inválida. Escolha novamente.\n");
            break;
        }
    }

    return 0;
}