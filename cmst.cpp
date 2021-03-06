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
int currParents[100], neigParents[100], bestParents[100];   // referência do nó pai na árvore geradora
int currNumChild[100], neigNumChild[100], bestNumChild[100];   // número de descendentes na árvore geradora

double initT, T;   // temperatura do simulated annealing
double reduceFactor;   // fator de redução da temperatura durante a heurística
double Kb;   // constante usada no cálculo da probabilidade de transição para um pior estado
double iterFactor;   // constante usada no cálculo do número de iterações de equilíbrio para uma temperatura
int S;   // valor da função objetivo pro estado atual
int neigS;   // valor da função objetivo pro estado vizinho
int best;   // ótimo corrente
long long countTempChange;   // número de iterações de mudança de temperatura
long long countBetterTr, countWorseTr, countStay;   // contagem das transições a cada iteração da heurística
long long countOptFinds;   // contagem de quantas vezes o ótimo foi atualizado
long long countTotalIters;   // contator de iterações totais da heurística
long long countTrackIters;   // contagem de iterações registradas para exibição gráfica

CPUTimer timer;   // timer de tempo de CPU utilizado
int execTime;   // tempo de execução da heurística

// gerador de números aleatórios
int randomNum(int hi) {
	const double scale = rand() / (double(RAND_MAX) + 1.0);
	return int(scale*hi);
}


// imprime a árvore passada como parâmetro no formato dot do graphviz
void printTree(int parents[]) {
	cout << "graph cmst {" << endl;

	for (int i = 0; i < N; i++) {
		cout << i << " -- " << parents[i] << ";" << endl;
	}
	cout << "}" << endl;
	cout << endl;
}


// imprime estatísticas sobre a execução da heurística
void printHeuristicStats() {
	cout << "Initial temperature: " << initT << endl;
	cout << "Reduction factor: " << reduceFactor << endl;
	cout << "Kb: " << Kb << endl;
	cout << "Increasing factor of equilibrium iterations: " << iterFactor << endl;
	cout << "Execution time (seconds): " << execTime << endl;
	cout << endl;
	cout << "Number of temperature changes: " << countTempChange << endl;
	cout << "Final temperature: " << T << endl;
	cout << "Total number of iterations: " << countTotalIters << endl;
	cout << "Transitions to a better state: " << countBetterTr << endl;
	cout << "Transitions to a worse state: " << countWorseTr << endl;
	cout << "Iterations without transition: " << countStay << endl;
	cout << "How many times a better solution was found: " << countOptFinds << endl;
	cout << endl;
}


// registra a iteração corrente da heurística para exibição gráfica
void trackIteration() {	
	timer.stop();
	
	countTrackIters++;
	cerr << countTrackIters << ",";
	cerr << timer.getCPUTotalSecs() << ",";
	cerr << T << ",";
	cerr << S << ",";
	cerr << min(best, S) << endl;
	
	timer.start();
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
	initT = T = 1500000;	// 11,000,000
	reduceFactor = 0.98;	// 0.94
	Kb = 5e-5;				// 5e-5
	iterFactor = pow(C/5.0, 0.4);
	execTime = atoi(argv[2]);

	countTempChange = countBetterTr = countWorseTr = countStay = countOptFinds = countTrackIters = 0;
}


// função que determina o número de iterações para se atingir o equilíbrio em uma dada temperatura T
long long numIters(double temp) {
	return (long long) max(1100 * iterFactor * sqrt(temp), 10000.0);
	//return (long long) max(temp, 10000.0);
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
bool isCurrSameSubTree(int node1, int node2) {
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
			(!isCurrSameSubTree(newParent, newChild) && (currNumChild[getCurrSubRoot(newParent)] + currNumChild[newChild] + 2 > C)));
	
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


// transforma o estado vizinho atualmente armazenado no estado corrente
void switchState() {
	S = neigS;
	copy(neigParents, neigParents + N+1, currParents);
	copy(neigNumChild, neigNumChild + N+1, currNumChild);
}


// atualiza a melhor solução com o estado corrente
void updateBest() {
	best = S;
	copy(currParents, currParents + N+1, bestParents);
	copy(currNumChild, currNumChild + N+1, bestNumChild);
}


// executa a heurística utilizando 'segs' segundos de tempo de CPU
void execHeuristic(int segs) {
	timer.reset();
	
	while (timer.getCPUTotalSecs() < segs) {
		timer.start();
		
		long long iters = numIters(T);
		for (long long j = 0; j < iters; j++) {
			genNeighbor();

			neigS = calcObjFunc(neigParents);
			int delta = neigS - S;
			
			if (delta < 0) {
				switchState();
				countBetterTr++;
			}
			else {
				double u = rand() / (double(RAND_MAX) + 1.0);
				
				if (u < exp(-delta / (Kb * T))) {
					switchState();
					countWorseTr++;
				}
				else {
					countStay++;
				}
			}
			
			// registrando iteração
			if (countTotalIters % 10000 == 0 || S < best) {
				trackIteration();
			}
			
			// atualizando melhor solução encontrada
			if (S < best) {
				updateBest();
				countOptFinds++;
			}
			
			countTotalIters++;
		}
		
		timer.stop();
		T *= reduceFactor;
		countTempChange++;
	}
}


int main(int argc, char *argv[]) {
	srand(time(NULL));
	rand();   // descartando 1o número possivelmente idêntico para seeds diferentes no Mac OS
	
	cerr.setf(ios::fixed, ios::floatfield);
	cerr.precision(4);

	init(argv);
	
	// inicializando melhor solução
	S = calcObjFunc(currParents);
	updateBest();
	
	execHeuristic(execTime);
	
	// imprimindo resultados
	cout << endl << "BEST SOLUTION: " << best << endl << endl;
	printTree(bestParents);	
	printHeuristicStats();
	
	return 0;
}

