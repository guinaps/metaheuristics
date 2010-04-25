
Descrição das Instâncias

- Abaixo são descritos os arquivos de instâncias para o
problema da Árvore Geradora com Restrições de Capacidade

Em todas as instâncias, todos os vértices têm demanda igual a 1.

Conforme visto abaixo,  as capacidades das árvores ligadas ao
nó raiz (o último nos arquivos, n+1) são todas iguais.

As instâncias correspondem a capacidade 3, 5 e 10.
Portanto serão no total 39 instâncias, visto que estão em anexo
os arquivos te16.txt, tc40-x.txt  e  te40-x.txt para x=1,2,3,4 e 5,  
tc80-1.txt e te80-1.txt.

Neste zip,  as instâncias pequenas te16  são para testar a corretude do algoritmo.

Todos os arquivos de instâncias são .txt. 
O arquivo te16.crd contém as coordenadas dos vértices
no plano. Isto permite desenhar a instância para visualizar
a solução obtida.

Segue a descrição original das instâncias.
------------------------------------------------------------------------

This file contains tc (central depot) and te data (end depot)
for the CMST with unit demands.

L. Gouveia, A comparison of directed formulations for the
capacitated minimal spanning tree problem, Telecommunication
Systems vol 1, 1993, 51-76.

Each vertex i (i=1,...n) has unit weight and each subtree connected to the 
depot N (N=n+1) in the final solution should have a total weight of less than 
or equal to Q.

For each problem type (tc,te), there are 10 problems each, 5 of size n=40
and 5 of size n=80.  Note that the depot is vertex number 41 and 81
respectively.

For size n=40 (N=41) : tc40-*.txt and te40-*.txt (*=1,...,5)
capacity Q can take three values: Q=3, 5, or 10.
Total number of problems = 2*5*3 = 30 (15 type tc and 15 type te).

For size n=80 (N=81) : tc80-*.txt and te80-*.txt (*=1,...,5)
capacity Q can take three values: Q=5, 10, or 20.
Total number of problems = 2*5*3 = 30 (15 type tc and 15 type te).

The format of the file is as follows:

problem name
problem size n
edge costs c(i,j), i=1,N, j=1,N

(Note c(i,i) is taken as a large number)


