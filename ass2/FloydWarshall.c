// Floyd Warshall ADT interface
// COMP2521 Assignment 2

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "FloydWarshall.h"
#include "Graph.h"

static ShortestPaths newSps(int GraphNumV);
static int **createIntMatrix(int GraphNumV);
static void addEdges(Graph g, int GraphNumV, ShortestPaths sps);
static void findShorestPaths(ShortestPaths sps, int GraphNumV);
static void freeIntMatrix(int **matrix);
////////////////////////////////////////////////////////////////////////

/**
 * Finds all shortest paths between all pairs of nodes.
 * 
 * The  function  returns  a 'ShortestPaths' structure with the required
 * information:
 * - the number of vertices in the graph
 * - distance matrix
 * - matrix of intermediates (see description above)
 */
ShortestPaths FloydWarshall(Graph g) {
	assert(g != NULL);

	// Initial set ups
	int GraphNumV = GraphNumVertices(g);
	ShortestPaths sps = newSps(GraphNumV);

	addEdges(g, GraphNumV, sps);
	findShorestPaths(sps, GraphNumV);
	
	return sps;
}

/**
 * Create a new ShortestPaths.
 * 
 * The function returns a 'ShortestPaths' structure.
*/
static ShortestPaths newSps(int GraphNumV) {
	ShortestPaths sps;
	sps.numNodes = GraphNumV;
	sps.dist = createIntMatrix(GraphNumV);
	sps.next = createIntMatrix(GraphNumV);
	return sps;
}

/**
 * Create an 'int' Matrix and return a pointer of pointer to
 * the start of the matrix.
*/
static int **createIntMatrix(int GraphNumV) {
	int **m = malloc((GraphNumV + 1) * sizeof(int *));
    if (m == NULL) {
        fprintf(stderr, "error: out of memory");
        exit(EXIT_FAILURE);
    }

    m[0] = (void *)(u_int64_t)(GraphNumV);
    for (int i = 1; i <= GraphNumV; i++) {
        m[i] = calloc(GraphNumV, sizeof(int));
        if (m[i] == NULL) {
            fprintf(stderr, "error: out of memory");
            exit(EXIT_FAILURE);
        }
    }

    return &m[1];
}

/**
 * Add the edges in the graph to the dist matrix.
 * If there's no edge, such as vertices [i][j], 
 * the distance, dist[i][j], is infinity.
 * The next matrix for the edges are the destination vertice.
*/
static void addEdges(Graph g, int GraphNumV, ShortestPaths sps) {
	AdjList l;
	for (int i = 0; i < GraphNumV; i++) {
		l = GraphOutIncident(g, i);
		while (l != NULL) {
			sps.dist[i][l->v] = l->weight;
			sps.next[i][l->v] = l->v;
			l = l->next;
		}
	}
	for (int i = 0; i < GraphNumV; i++) {
		for (int j = 0; j < GraphNumV; j++) {
			if (i != j && !sps.dist[i][j]) {
				sps.dist[i][j] = INFINITY;
			}
		}
	}
}

/**
 * Find the shortest paths between two vertices.
 * If there's a shorter/lighter path, replace it 
 * in the dist matrix, and put the pass by vertice
 * into the next martrix. 
 * (i.e. i->j passed by k, 
 * 		dist[i][j] = dist[i][k]+dist[k][j]
 * 		next[i][j] = k)
 * If there's no path between i and j,
 * the dist[i][j] will by INFINITY
 * and the next[i][j] is -1.
*/
static void findShorestPaths(ShortestPaths sps, int GraphNumV) {
	for (int i = 0; i < GraphNumV; i++) {
		for (int j = 0; j < GraphNumV; j++) {
			for (int k = 0; k < GraphNumV; k++) {
				if (sps.dist[j][i] != INFINITY && sps.dist[i][k]!= INFINITY &&
					sps.dist[j][k] > sps.dist[j][i] + sps.dist[i][k]) {
					sps.dist[j][k] = sps.dist[j][i] + sps.dist[i][k];
					sps.next[j][k] = i;
				}		
				if (sps.dist[j][k] == 0 || sps.dist[j][k] == INFINITY) {
					sps.next[j][k] = -1;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////

/**
 * This  function  is  for  you to print out the ShortestPaths structure
 * while you are debugging/testing your implementation. 
 * 
 * We will not call this function during testing, so you may  print  out
 * the  given  ShortestPaths  structure in whatever format you want. You
 * may choose not to implement this function.
 */
void showShortestPaths(ShortestPaths sps) {
	for (int i = 0; i < sps.numNodes; i++) {
		for (int j = 0; j < sps.numNodes; j++) {
			printf("vertice '%d' to '%d' have %d distance\n", 
					i, j, sps.dist[i][j]);
		}
	}
}

/**
 * Frees all memory associated with the given ShortestPaths structure.
 */
void freeShortestPaths(ShortestPaths sps) {
	freeIntMatrix(sps.dist);
	freeIntMatrix(sps.next);
}
/**
 * Free all memory in the matrix.
 */
static void freeIntMatrix(int **matrix) {
	int GraphNumV = (int)(u_int64_t)(matrix[-1]);
    for (int i = 0; i < GraphNumV; i++) {
        free(matrix[i]);
    }
    free(&matrix[-1]);
}