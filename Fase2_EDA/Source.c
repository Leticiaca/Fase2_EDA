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

}