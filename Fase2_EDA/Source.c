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
/*void addEdge(Graph* graph, int src, int dest) {
    Node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
}*/

void addEdgeAndSaveToDot(Graph* graph, int src, int dest, const char* filename) {
    Node* newNode = createNode(dest);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;

    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo %s", filename);
        return;
    }

    fprintf(file, "digraph G {\n");
    for (int v = 0; v < graph->numVertices; v++) {
        Node* temp = graph->adjLists[v];
        while (temp) {
            fprintf(file, " %d -> %d;\n", v, temp->vertex);
            temp = temp->next;
        }
    }
    fprintf(file, "}\n");
    fclose(file);
}

void loadAdjacenciesFromDotFile(Graph* graph, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", filename);
        return;
    }

    // Ignorar as linhas até encontrar a linha "digraph G {"
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "digraph G {")) {
            break;
        }
    }

    // Ler as arestas
    while (fgets(line, sizeof(line), file)) {
        int src, dest;
        if (sscanf(line, " %d -> %d;", &src, &dest) == 2) {
            // Adicionar a aresta ao grafo
            addEdgeAndSaveToDot(graph, src, dest, "grafo.dot");
        }
    }

    fclose(file);
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

void printVertices(const Graph* graph, const int matrix[MAX_SIZE][MAX_SIZE], int size) {
    for (int v = 0; v < graph->numVertices; ++v) {
        int row = v / size;
        int col = v % size;
        if (matrix[row][col] != -1) { // Ignora vértices removidos
            printf("Vértice %d (Valor na matriz: %d)\n", v, matrix[row][col]);
        }
    }
}



// Ponto 1.3 adicionar vertice ao grafo
void addVertex(Graph* graph, int matrix[MAX_SIZE][MAX_SIZE], int* size, int value) {
    // Verifica se a matriz já atingiu o tamanho máximo
    if (*size >= MAX_SIZE) {
        fprintf(stderr, "Erro: número máximo de vértices atingido.\n");
        return;
    }

    // Adiciona o novo vértice
    int newVertex = graph->numVertices;
    graph->numVertices++;

    graph->adjLists = (Node**)realloc(graph->adjLists, graph->numVertices * sizeof(Node*));
    if (!graph->adjLists) {
        fprintf(stderr, "Erro ao realocar memória para as listas de adjacências.\n");
        exit(EXIT_FAILURE);
    }
    graph->adjLists[newVertex] = NULL;

    // Atribui o valor do vértice à matriz
    int row = newVertex / *size;
    int col = newVertex % *size;
    matrix[row][col] = value;
}

void removeVertex(Graph* graph, int vertex, int matrix[MAX_SIZE][MAX_SIZE], int size) {
    if (vertex >= graph->numVertices) {
        fprintf(stderr, "Erro: Vértice inválido.\n");
        return;
    }

    // Remove todas as arestas que apontam para este vértice
    for (int i = 0; i < graph->numVertices; i++) {
        if (i != vertex) {
            removeEdge(graph, i, vertex);
        }
    }

    // Remove todas as arestas a partir deste vértice
    Node* temp = graph->adjLists[vertex];
    while (temp) {
        Node* next = temp->next;
        free(temp);
        temp = next;
    }
    graph->adjLists[vertex] = NULL;

    // Marca o valor na matriz como inválido
    int row = vertex / size;
    int col = vertex % size;
    matrix[row][col] = -1; // Valor indicando que o vértice foi removido

    printf("Vértice %d removido.\n", vertex);
}





// Ponto 2: Listagem de adjacentes
void printGraph(const Graph* graph, const int matrix[MAX_SIZE][MAX_SIZE], int size) {
    if (!graph || !graph->adjLists) return;

    for (int v = 0; v < graph->numVertices; ++v) {
        int row = v / size;
        int col = v % size;
        if (matrix[row][col] == -1) { // Ignora vértices removidos
            continue;
        }

        printf("\nLista de adjacências do vertice %d (Valor na matriz: %d):\n", v, matrix[row][col]);
        Node* temp = graph->adjLists[v];
        while (temp) {
            if (matrix[temp->vertex / size][temp->vertex % size] != -1) { // Ignora adjacências removidas
                printf("-> %d (Valor na matriz: %d)\n", temp->vertex, matrix[temp->vertex / size][temp->vertex % size]);
            }
            temp = temp->next;
        }
        printf("\n");
    }
    
}




void exportGraphToDot(const Graph* graph, const int matrix[MAX_SIZE][MAX_SIZE], int size, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo %s para escrita.\n", filename);
        return;
    }

    fprintf(file, "digraph G {\n");

    // Escreve os vértices
    for (int v = 0; v < graph->numVertices; ++v) {
        int row = v / size;
        int col = v % size;
        if (matrix[row][col] != -1) { // Ignora vértices removidos
            fprintf(file, "    %d [label=\"%d\"];\n", v, matrix[row][col]);
        }
    }

    // Escreve as arestas
    for (int v = 0; v < graph->numVertices; ++v) {
        int row = v / size;
        int col = v % size;
        if (matrix[row][col] != -1) { // Ignora vértices removidos
            Node* temp = graph->adjLists[v];
            while (temp) {
                int adjRow = temp->vertex / size;
                int adjCol = temp->vertex % size;
                if (matrix[adjRow][adjCol] != -1) { // Ignora adjacências removidas
                    fprintf(file, "    %d -> %d;\n", v, temp->vertex);
                }
                temp = temp->next;
            }
        }
    }

    fprintf(file, "}\n");
    fclose(file);
    printf("Grafo exportado para %s com sucesso.\n", filename);
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
    int size = 2; // Pode ser ajustado conforme necessário
    int matrix[MAX_SIZE][MAX_SIZE];
    Graph* graph = createGraph(size * size); // Cria um grafo com vértices do tamanho da matriz

    loadMatrixFromFile(graph, "matrix.txt", size, matrix);
    loadAdjacenciesFromDotFile(graph, "grafo.dot");

    int option;

    while (true) {
        printf("\nMenu de Opcoes:\n");
        printf("1. Lista de adjacentes\n");
        printf("2. Adicionar uma aresta ao grafo\n");
        printf("3. Remover uma aresta do grafo\n");
        printf("4. Todos os caminhos possiveis\n");
        printf("5. Encontrar o melhor caminho com maior soma\n");
        printf("6. Adicionar um novo vertice ao grafo\n");
        printf("7. Remover um vertice do grafo\n");
        printf("8. Exportar o grafo para grafo.dot\n");
        printf("9. Mostrar os vertices\n");
        printf("10. Sair\n");
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
            addEdgeAndSaveToDot(graph, src, dest, "grafo.dot");
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
            bool visited[MAX_SIZE] = { false };
            int currentPath[MAX_SIZE];
            int bestPath[MAX_SIZE];
            int currentPathIndex = 0;
            int maxSum = 0;
            int bestPathLength = 0;

            // Chamada para encontrar todos os caminhos possíveis
            DFS(graph, 0, visited, currentPath, currentPathIndex, matrix, size, NULL, bestPath, &bestPathLength);
            break;
        case 5:
            printf("\nEncontrar o melhor caminho com maior soma:\n");
            // Reinicializa os vetores visited, currentPath e bestPath
            memset(visited, false, sizeof(visited));
            memset(currentPath, 0, sizeof(currentPath));
            memset(bestPath, 0, sizeof(bestPath));
            currentPathIndex = 0;
            maxSum = 0;
            bestPathLength = 0;

            // Chamada para encontrar o melhor caminho com maior soma
            DFS(graph, 0, visited, currentPath, currentPathIndex, matrix, size, &maxSum, bestPath, &bestPathLength);
            printBestPath(bestPath, bestPathLength, maxSum);
            break;
        case 6:
            printf("Digite o valor do novo vertice: ");
            int value;
            scanf("%d", &value);
            addVertex(graph, matrix, &size, value);
            printf("Novo vertice adicionado com valor %d. Total de vertices: %d\n", value, graph->numVertices);
            break;
        case 7:
            printf("Vértices disponíveis:\n");
            printVertices(graph, matrix, size);
            printf("Digite o número do vértice que deseja remover: ");
            int vertex;
            scanf("%d", &vertex);
            removeVertex(graph, vertex, matrix, size);
            break;
        case 8:
            exportGraphToDot(graph, matrix, size, "grafo.dot");
            break;
        case 9:
            printf("Vértices disponíveis:\n");
            printVertices(graph, matrix, size);
            break;
        case 10:
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






