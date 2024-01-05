// Girvan-Newman Algorithm for community discovery
// COMP2521 Assignment 2

#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "CentralityMeasures.h"
#include "GirvanNewman.h"
#include "Graph.h"

#define LEAF -1

static Dendrogram newDendrogram(int v);
static void setUpCpnMatrix(Graph g, int GraphNumV, EdgeValues evs, 
						   int componentMatrix[GraphNumV][GraphNumV], 
						   int nComponents[GraphNumV]);
static int largestEvs(Graph g, EdgeValues evs, int GraphNumV);
static void removeLargestEvs(Graph g, EdgeValues evs, int GraphNumV, 
					  		 int maxEvs);
static int components(Graph g, int GraphNumV, int i, 
			   		  int componentMatrix[GraphNumV][GraphNumV]);
static void dfsComponent(Graph g, int GraphNumV, int i, int v, int id, 
				  		 int componentMatrix[GraphNumV][GraphNumV]);
static Dendrogram sameComponent(int GraphNumV, 
								int componentMatrix[GraphNumV][GraphNumV], 
						 		int sameCpn[GraphNumV][GraphNumV], 
						 		int nComponents[GraphNumV], int addedToD[GraphNumV] , 
						 		int row);
static int setSameCpn(int GraphNumV, int componentMatrix[GraphNumV][GraphNumV], 
			   		  int sameCpn[GraphNumV][GraphNumV], 
					  int addedToD[GraphNumV], 
			   		  int cpn, int row);
////////////////////////////////////////////////////////////////////////

/**
 * Generates a Dendrogram for the given graph g using the Girvan-Newman
 * algorithm.
 * 
 * The function returns a 'Dendrogram' structure.
 */
Dendrogram GirvanNewman(Graph g) {
	assert(g != NULL);

	Dendrogram d = newDendrogram(LEAF);

	int GraphNumV = GraphNumVertices(g);
	EdgeValues evs = edgeBetweennessCentrality(g);

	int componentMatrix[GraphNumV][GraphNumV];
	memset(componentMatrix, -1, sizeof(int)*GraphNumV*GraphNumV);
	int *nComponents;
	nComponents = calloc(GraphNumV, sizeof(int));

	setUpCpnMatrix(g, GraphNumV, evs, componentMatrix, nComponents);

	int sameCpn[GraphNumV][GraphNumV];
	memset(sameCpn, -1, sizeof(int)*GraphNumV*GraphNumV);
	int *addedToD;
	addedToD = calloc(GraphNumV, sizeof(int));

	d->left = sameComponent(GraphNumV, componentMatrix, sameCpn, 
			  nComponents, addedToD, 0);
	d->right = sameComponent(GraphNumV, componentMatrix, sameCpn, 
			  nComponents, addedToD, 0);

	return d;
}

/**
 * Create a new Dendrogram.
 * 
 * The function returns a 'Dendrogram' structure.
*/
static Dendrogram newDendrogram(int v) {
	Dendrogram d = malloc(sizeof(*d));
	d->vertex = v;
	d->left = NULL;
	d->right = NULL;

	return d;
}

/**
 * 1. Find the largest evs and remove the edge(s) with the largest evs.
 * 2. Recalculate the edgeBetweennessCentrality
 * 
 * 3. If there are new seperated component(s),  find the component id 
 * 	  of the vertices and put the id into the componentMatrix in a row.
 * 	  (i.e. componentMatrix[row][v], where 
 * 	   componentMatrix[row + 1][v] represents 
 * 	   the subcomponents)
 * 
 * Repeat steps 1-3 until there are no edges.
*/
static void setUpCpnMatrix(Graph g, int GraphNumV, EdgeValues evs, 
						   int componentMatrix[GraphNumV][GraphNumV], 
						   int nComponents[GraphNumV]) {
	int maxEvs = -1;
	for (int row = 0; row < GraphNumV; row++) {
		maxEvs = largestEvs(g, evs, GraphNumV);
		
		if (maxEvs == -1) {
			break;
		}

		removeLargestEvs(g, evs, GraphNumV, maxEvs);
		evs = edgeBetweennessCentrality(g);

		nComponents[row] = components(g, GraphNumV, row, componentMatrix);

		// no new components
		if (nComponents[row] == nComponents[row - 1]) {
			for (int v = 0; v < GraphNumV; v++) {
				componentMatrix[row][v] = -1;
			}
			row--;
		}
	}
}

/**
 * Find and return the largest evs in the graph.
*/
static int largestEvs(Graph g, EdgeValues evs, int GraphNumV) {
	int max = -1;
	for (int i = 0; i < GraphNumV; i++) {
		for (int j = 0; j < GraphNumV; j++) {
			if (evs.values[i][j] > max) {
				max = evs.values[i][j];
			}
		}
	}
	return max;
}

/**
 * Remove the edge(s) with the largest evs.
*/
static void removeLargestEvs(Graph g, EdgeValues evs, int GraphNumV, 
					  	     int maxEvs) {
	assert(maxEvs > 0);

	for (int i = 0; i < GraphNumV; i++) {
		for (int j = 0; j < GraphNumV; j++) {
			if (evs.values[i][j] == maxEvs) {
				GraphRemoveEdge(g, i, j);
			}
		}
	}
}

/**
 * Assign vertices to connected components and return the number of 
 * components.
*/
static int components(Graph g, int GraphNumV, int i, 
			   		  int componentMatrix[GraphNumV][GraphNumV]) {
	int compID = 0;
	for (int v = 0; v < GraphNumV; v++) {
		if (componentMatrix[i][v] == -1) {
			dfsComponent(g, GraphNumV, i, v, compID, componentMatrix);
			compID++;
		}
	}
	return compID;
}

/**
 * DFS scan of one connected component.
 * Put the component id into the componentMatrix for every vertice.
*/
static void dfsComponent(Graph g, int GraphNumV, int i, int v, int id, 
				  		 int componentMatrix[GraphNumV][GraphNumV]) {
	componentMatrix[i][v] = id;
	for (int w = 0; w < GraphNumV; w++) {
		if (GraphIsAdjacent(g, v, w) && componentMatrix[i][w] == -1) {
			dfsComponent(g, GraphNumV, i, w, id, componentMatrix);
		}
	}
}

/**
 * If there's an independent vertice, add this vertice to the 
 * dendrogram tree. Otherwise, by looking down the componentMatrix, 
 * row++, and looking in the subComponents, add the independent
 * vertices into the dendrogram tree.
 * 
 * This function will return a leaf node contain the independent 
 * vertice or it's subNodes containing independent vertice.
*/
static Dendrogram sameComponent(int GraphNumV, 
						 		int componentMatrix[GraphNumV][GraphNumV], 
						 		int sameCpn[GraphNumV][GraphNumV], 
						 		int nComponents[GraphNumV], 
								int addedToD[GraphNumV], 
						 		int row) {
	Dendrogram d = NULL;
	int nCpn;
	for (int cpn = 0; cpn < nComponents[row]; cpn++) {
		nCpn = setSameCpn(GraphNumV, componentMatrix, sameCpn, addedToD, 
						  cpn, row);
		if (nCpn == 1) {
			for (int i = 0; i < GraphNumV; i++) {
				if (sameCpn[row][i] && !addedToD[i]) {
					addedToD[i] = true;
					return newDendrogram(i);
				}
			}
		} else if (nCpn > 1){
			d = newDendrogram(LEAF);
			d->left = sameComponent(GraphNumV, componentMatrix, sameCpn, 
									nComponents, addedToD, row + 1);
			d->right = sameComponent(GraphNumV, componentMatrix, sameCpn, 
									 nComponents, addedToD, row + 1);
			return d;
		}
	}
	
	return d;
}

/**
 * Set the subComponent to 'true' in the sameCpn array.
 * This is for finding the independent vertice in a subComponent.
 * The function will return the number of components in a subComponent.
*/
static int setSameCpn(int GraphNumV, int componentMatrix[GraphNumV][GraphNumV], 
			   int sameCpn[GraphNumV][GraphNumV], int addedToD[GraphNumV], 
			   int cpn, int row) {
	int num = 0;
	for (int v = 0; v < GraphNumV; v++) {
		if (componentMatrix[row][v] == cpn && !addedToD[v]) {
			if (row == 0 || sameCpn[row - 1][v]) {
				sameCpn[row][v] = true;
				num++;
			}
		} else {
			sameCpn[row][v] = false;
		}
	}
	return num;
}

/**
 * Frees all memory associated with the given Dendrogram  structure.
 */
void freeDendrogram(Dendrogram d) {
	if (d != NULL) {
		freeDendrogram(d->left);
		freeDendrogram(d->right);
		free(d);
	}
}