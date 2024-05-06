#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 100

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
// A lista de adjac�ncias de cada v�rtice � as liga��es aos outros v�rtices que tem; por exemplo: lista do v�rtice 1 � 2 e 3.
// Fun��o para imprimir o grafo
void printGraph(Graph* graph, int size, int matrix[MAX_SIZE][MAX_SIZE]) {
    int v;
    for (v = 0; v < graph->numVertices; v++) {
        Node* temp = graph->adjLists[v];
        printf("\nLista de adjacencias do vertice %d (Valor na matriz: %d)\n", v, matrix[v / size][v % size]);
        while (temp) {
            printf("-> %d (Valor na matriz: %d)", temp->vertex, matrix[temp->vertex / size][temp->vertex % size]);
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

#pragma region Algorithm

// Fun��o de busca em profundidade (DFS)
void DFS(Graph* graph, int v, bool visited[], int path[], int pathIndex, int matrix[MAX_SIZE][MAX_SIZE]) {
    visited[v] = true;
    path[pathIndex] = v;

    // Se o v�rtice atual for o destino final do caminho, calcula e imprime a soma dos valores dos v�rtices no caminho
    if (pathIndex > 0 && path[pathIndex] == (graph->numVertices - 1)) {
        int sum = 0;
        printf("Caminho encontrado: ");
        for (int i = 0; i <= pathIndex; ++i) {
            int row = path[i] / MAX_SIZE;
            int col = path[i] % MAX_SIZE;
            sum += matrix[row][col];
            printf("%d ", path[i]);
            if (i < pathIndex)
                printf("-> ");
        }
        printf("\nSoma dos valores dos v�rtices no caminho: %d\n", sum);
    }

    Node* temp = graph->adjLists[v];
    while (temp) {
        int adjVertex = temp->vertex;
        if (!visited[adjVertex])
            DFS(graph, adjVertex, visited, path, pathIndex + 1, matrix);
        temp = temp->next;
    }

    visited[v] = false; // Marca o v�rtice como n�o visitado para outras travessias poss�veis
}


// Fun��o para encontrar todos os caminhos poss�veis
void findAllPaths(Graph* graph, int matrix[MAX_SIZE][MAX_SIZE]) {
    bool* visited = (bool*)malloc(graph->numVertices * sizeof(bool));
    int* path = (int*)malloc(graph->numVertices * sizeof(int));
    for (int i = 0; i < graph->numVertices; ++i)
        visited[i] = false;
    int pathIndex = 0;
    DFS(graph, 0, visited, path, pathIndex, matrix);
    free(visited);
    free(path);
}

#pragma endregion

// Fun��o para carregar uma matriz de inteiros de um arquivo de texto para o grafo
void loadMatrixFromFile(Graph* graph, const char* filename, int size, int matrix[MAX_SIZE][MAX_SIZE]) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s\n", filename);
        return;
    }

    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (fscanf(file, "%d;", &matrix[i][j]) != 1) {
                printf("Erro ao ler o arquivo %s\n", filename);
                fclose(file);
                return;
            }
        }
    }

    fclose(file);

    // Adiciona as arestas ao grafo com base na matriz
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            // Adiciona arestas de acordo com a regra espec�fica
            // Aqui, por exemplo, estamos considerando apenas elementos na mesma linha ou coluna
            if (i < size - 1) // Adiciona aresta para o elemento abaixo na mesma coluna
                addEdge(graph, i * size + j, (i + 1) * size + j);
            if (j < size - 1) // Adiciona aresta para o elemento � direita na mesma linha
                addEdge(graph, i * size + j, i * size + j + 1);
        }
    }
}

#pragma region Main

int main() {

    int i, j;
    int size = 5; // Defina o tamanho da matriz, por exemplo, 5
    int matrix[MAX_SIZE][MAX_SIZE];
    int V = 25; // N�mero de v�rtices (tamanho da matriz)
    Graph* graph = createGraph(V); // Cria��o do grafo com base no tamanho da matriz

    // Inicializa a matriz com alguns valores de teste
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            matrix[i][j] = i * size + j; // Por exemplo, preencha com o �ndice do elemento na matriz
        }
    }

    // Carrega a matriz de um arquivo de texto
    loadMatrixFromFile(graph, "C:\\Users\\H�lder Pinto\\Desktop\\Fase2_EDA-master\\Fase2_EDA-master\\Fase2_EDA\\matrix.txt", 5, matrix); // Assume que a matriz tem tamanho 5x5

    // Imprime o grafo (lista de adjac�ncias)
    printGraph(graph, 5, matrix);

    // Chama a fun��o para encontrar todos os caminhos poss�veis no grafo
    findAllPaths(graph, matrix);

    // Libera a mem�ria alocada para o grafo
    freeGraph(graph);

    /*int V = 5;
    Graph* graph = createGraph(V);
    addEdge(graph, 0, 1);
    addEdge(graph, 0, 2);
    addEdge(graph, 1, 3);
    addEdge(graph, 2, 4);
    addEdge(graph, 3, 4);
    addEdge(graph, 4, 0);

    printGraph(graph);

    removeEdge(graph, 0, 1);

    printf("\nGrafo ap�s remo��o de algumas arestas:\n");
    printGraph(graph);

    freeGraph(graph);*/

    return 0;
}


#pragma endregion

