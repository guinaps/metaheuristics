#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

#include "CPUTimer.h"

using namespace std;


int N;   // número de nós do problema
int C;   // restrição de capacidade do CMST
int edgeCost[100][100];   // matriz de custos das arestas do grafo
int currParents[100], neigParents[100];   // referência do nó pai na árvore geradora
int currNumChild[100], neigNumChild[100];   // número de descendentes na árvore geradora

double T;   // temperatura do simulated annealing


// gerador de números aleatórios
int randomNum(int hi) {
	const float scale = rand() / float(RAND_MAX);
	return int(scale*hi);
}


// inicializa os dados do problema e os parâmetros da heurística
void init(char *argv[]) {
	// inicializando restrição de capacidade
	C = atoi(argv[1]);
	
	// lendo número de nós
	cin >> N;
	
	// lendo matriz de custos de arestas
	for (int i = 0; i < N+1; i++) {
		for (int j = 0; j < N+1; j++) {
			cin >> edgeCost[i][j];
		}
	}
	
	// inicializando pais e número de descendentes na árvore geradora
	currParents[N] = -1;
	currNumChild[N] = N;

	for (int i = 0; i < N; i++) {
		currParents[i] = N;
		currNumChild[i] = 0;
	}
	
	// inicializando parâmetros da heurística
	T = 1000;
}


// calcula a função objetivo para a árvore geradora passada como parâmetro
int calcObjFunc(int parents[]) {
	int cost = 0;
	
	for (int i = 0; i < N; i++) {
		cost += edgeCost[i][parents[i]];
	}

	return cost;
}


// verifica se 'candidate' é ancestral de 'descendant' na árvore geradora corrente
bool isCurrAncestral(int candidate, int descendant) {
	if (descendant == candidate) return true;
	if (descendant < 0) return false;
	
	return isCurrAncestral(candidate, currParents[descendant]);
}


// obtém a raiz da subárvore à qual 'node' pertence na árvore geradora corrente
int getCurrSubRoot(int node) {
	if (node == N || node < 0) return -1;
	if (currParents[node] == N) return node;

	return getCurrSubRoot(currParents[node]);
}


// verifica se 'node1' e 'node2' estão na mesma subárvore na árvore geradora corrente
int isCurrSameSubTree(int node1, int node2) {
	return (getCurrSubRoot(node1) == getCurrSubRoot(node2));
}


// gera um vizinho para o estado corrente
void genNeighbor() {
	int newParent, newChild;
	
	// gera aleatoriamente uma transformação na árvore corrente: atribui a um dado nó um novo pai, desde que
	// a transformação seja válida
	do {
		newParent = randomNum(N+1);
		newChild = randomNum(N+1);
	} while (newParent == newChild || newParent == currParents[newChild] || isCurrAncestral(newChild, newParent) ||
			(!isCurrSameSubTree(newParent, newChild) && currNumChild[getCurrSubRoot(newParent)] + currNumChild[newChild] + 2 > C));
	
	// gerando atributos do vizinho
	copy(currParents, currParents + N+1, neigParents);
	copy(currNumChild, currNumChild + N+1, neigNumChild);
	
	int presNode = newParent;
	while (presNode != N) {
		neigNumChild[presNode] += neigNumChild[newChild] + 1;
		presNode = neigParents[presNode];
	}

	presNode = neigParents[newChild];
	while (presNode != N) {
		neigNumChild[presNode] -= neigNumChild[newChild] + 1;
		presNode = neigParents[presNode];
	}

	neigParents[newChild] = newParent;
}


int main(int argc, char *argv[]) {
	srand(time(NULL));
	rand();   // descartando 1o número possivelmente idêntico para seeds diferentes no Mac OS X

	init(argv);
	
	for (int i = 0; i < 100000; i++) {
		genNeighbor();
		copy(neigParents, neigParents + N+1, currParents);
		copy(neigNumChild, neigNumChild + N+1, currNumChild);
	}

	cout << "graph oi {" << endl;
	for (int i = 0; i < N; i++) {
		cout << i << " -- " << currParents[i] << ";" << endl;
	}
	cout << "}" << endl;
	
	return 0;
}

