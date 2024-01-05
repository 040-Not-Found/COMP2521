// Centrality Measures ADT interface
// COMP2521 Assignment 2

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "CentralityMeasures.h"
#include "FloydWarshall.h"
#include "Graph.h"

static EdgeValues newEvs(int GraphNumV);
static double **createDoubleMatrix(int GraphNumV);
static void doedgeBetweennessCentrality(Graph g, EdgeValues evs, 
                                 ShortestPaths sps, int GraphNumV);
static void addValue(Graph g, EdgeValues evs, ShortestPaths sps, 
                                int i, int j);
static void freeDoubleMatrix(double **matrix);
////////////////////////////////////////////////////////////////////////

/**
 * Finds  the  edge  betweenness  centrality  for each edge in the given
 * graph and returns the results in a  EdgeValues  structure.  The  edge
 * betweenness centrality of a non-existant edge should be set to -1.0.
 */
EdgeValues edgeBetweennessCentrality(Graph g) {
	assert(g != NULL);

    ShortestPaths sps = FloydWarshall(g);
	int GraphNumV = GraphNumVertices(g);

	EdgeValues evs = newEvs(GraphNumV);

    doedgeBetweennessCentrality(g, evs, sps, GraphNumV);

	return evs;
}

/**
 * Create a new EdgeValues
 * 
 * The function returns a 'EdgeValues' structure.
*/
static EdgeValues newEvs(int GraphNumV) {
    EdgeValues evs;
	evs.numNodes = GraphNumV;
	evs.values = createDoubleMatrix(GraphNumV);
    return evs;
}

/**
 * Create a 'double' Matrix and return a pointer of pointer to
 * the start of the matrix.
*/
static double **createDoubleMatrix(int GraphNumV) {
	double **m = malloc((GraphNumV + 1) * sizeof(double *));
    if (m == NULL) {
        fprintf(stderr, "error: out of memory");
        exit(EXIT_FAILURE);
    }

    m[0] = (void *)(u_int64_t)(GraphNumV);
    for (int i = 1; i <= GraphNumV; i++) {
        m[i] = calloc(GraphNumV, sizeof(double));
        if (m[i] == NULL) {
            fprintf(stderr, "error: out of memory");
            exit(EXIT_FAILURE);
        }
    }

    return &m[1];
}

/**
 * Finds the edge betweenness centrality for each edge.
 * If there's no path passes an edge, the value will be -1.
 * Else calculate how many times the edge has been pass.
*/
static void doedgeBetweennessCentrality(Graph g, EdgeValues evs, 
                                 ShortestPaths sps, int GraphNumV) {
    for (int i = 0; i < GraphNumV; i++) {
        for (int j = 0; j < GraphNumV; j++) {
            if (i == j || sps.dist[i][j] == INFINITY) {
                evs.values[i][j] = -1;
            } else {
                addValue(g, evs, sps, i, j);
            }
            if (!GraphIsAdjacent(g, i ,j)) {
                evs.values[i][j] = -1;
            }
        }
    }
}

/**
 * Check for all shortest paths, every edge been passed
 * will add 1 in the values matrix.
*/
static void addValue(Graph g, EdgeValues evs, ShortestPaths sps, 
                                int i, int j) {
    if (sps.next[i][j] == j) {
        evs.values[i][j]++;
    } else {
        evs.values[sps.next[i][j]][j]++;
        addValue(g, evs, sps, i, sps.next[i][j]);
    }
}

/**
 * Prints  the  values in the given EdgeValues structure to stdout. This
 * function is purely for debugging purposes and will NOT be marked.
 */
void showEdgeValues(EdgeValues evs) {
    for (int i = 0; i < evs.numNodes; i++) {
        for (int j = 0; j < evs.numNodes; j++) {
            printf("%d %d: %f\n", i , j, evs.values[i][j]);
        }
    }
    printf("\n");
}

/**
 * Frees all memory associated with the given EdgeValues structure.
 */
void freeEdgeValues(EdgeValues evs) {
	freeDoubleMatrix(evs.values);
}

/**
 * Free all memory in the matrix.
*/
static void freeDoubleMatrix(double **matrix) {
	int GraphNumV = (int)(u_int64_t)(matrix[-1]);
    for (int i = 0; i < GraphNumV; i++) {
        free(matrix[i]);
    }
    free(&matrix[-1]);
}
