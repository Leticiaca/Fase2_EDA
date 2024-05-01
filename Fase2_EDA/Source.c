#include <stdio.h>
#include <stdlib.h>

typedef struct Edge {
	int vertexIndex;
	struct Edge* next;
}Edge;

typedef struct Vertex {
	int id;
	int value;
	Edge* edges;
}Vertex;

typedef struct Graph {
	int numVertices;
	Vertex* vertices;
}Graph;

Graph* createGraph(int numVertices) {
	Graph* graph = (Graph*)malloc(sizeof(Graph));
	graph->numVertices = numVertices;
	graph->vertices = (Vertex*)malloc(numVertices * sizeof(Vertex));

	for (int i = 0; i < numVertices; i++) {
		graph->vertices[i].id = i;
		graph->vertices[i].edges = NULL;
	}

	return graph;
}

void addEdge(Graph* graph, int src, int dest) {
	Edge* edge = (Edge*)malloc(sizeof(Edge));
	edge->vertexIndex = dest;
	edge->next = graph->vertices[src].edges;
	graph->vertices[src].edges = edge;
}

/*void configureVertices(Graph* graph, int* matrix, int rows, int cols) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			int index = i * cols + j;
			graph->vertices[index].value = matrix[index];
			for (int k = 0; k < cols; k++) {
				if (k != j) {
					addEdge(graph, index, i * cols + j);
				}
			}
			for (int k = 0; k < rows; k++) {
				if (k != i) {
					addEdge(graph, index, k * cols + j);
				}
			}
		}
	}
}*/

void destroyGraph(Graph* graph) {
	for (int i = 0; i < graph->numVertices; i++) {
		Edge* edge = graph->vertices[i].edges;
		while (edge) {
			Edge* tmp = edge;
			edge = edge->next;
			free(tmp);
		}
	}
	free(graph->vertices);
	free(graph);
}

void loadGraphFromFile(Graph* graph, const char* filename) {
	FILE* file = fopen("matrix.txt", "r");
	if (!file) {
		perror("Falha ao abrir");
		return;
	}

	char line[1024];
	int vertexId = 0;
	while (fgets(line, sizeof(line), file)) {
		char* token = strtok(line, ";");
		while (token) {
			int value = atoi(token);
			graph->vertices[vertexId].value = value;
			vertexId++;
			token = strtok(NULL, ";");
		}
	}
	fclose(file);
}

int main() {
	int numVertices = 9;
	Graph* graph = createGraph(numVertices);
	loadGraphFromFile(graph, "matrix.txt");
	destroyGraph(graph);
	return 0;
}
