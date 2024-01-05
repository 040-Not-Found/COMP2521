
#include <stdio.h>
#include <stdlib.h>

#include "Graph.h"

void donumReachable(Graph g, int src, int reachableArray, int visited) {
	visited[src] = true;
	for (int i = 0; i < numV; i++) {
		if (GraphIsAdjacent(g, src, i)) {
			reachableArray[i] = true;
		}
	}
	for (int i = 0; i < numV; i++) {
		if (reachableArray[i] = true && !visited[i])) {
			donumReachable(g, i, reachableArray, visited);
		}
	}
}

int numReachable(Graph g, int src) {
	int numV = GraphNumVertices(g);
	int sum = 0;
	int *reachableArray;
	reachableArray = calloc(numV, sizeof(int));
	reachableArray[src] = true;
	int *visited;
	visited = calloc(numV, sizeof(int));
	donumReachable(g, src, reachableArray, visited);
	
	return num;
}

