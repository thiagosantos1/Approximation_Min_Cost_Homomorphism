#include <graph.h> 

//#define PAIRS_CONSTRAINT

int main(int argc, char const *argv[])
{
	USER_PARAMS userpar;
  GRAPH graph;
  LP_MIN_COST lp;

  if(argc <6){
		fprintf(stderr, "Usage <Graph G>, <Graph H>, <List Cost>, <List Homom> & <Pair List>\n");
		exit(1);
	}

	userpar.graph_g = strdup(argv[1]);
	userpar.graph_h = strdup(argv[2]);
	userpar.list_cost = strdup(argv[3]);
	userpar.list_homom = strdup(argv[4]);
	userpar.list_pairs = strdup(argv[5]);

	lp.PAIRS_CONST = 0; // set to false

	makegraph(&graph, &userpar);

	#ifdef PAIRS_CONSTRAINT
		pair_consistency(&graph, &userpar);
		lp.PAIRS_CONST = 1; // set to true
	#endif
	
	LP_SOLVER(&graph, &lp);
	
	return 0;
}
// gcc main.c makegraph.c pair_consistency.c LP_Min_Cost.c -lglpk -o ../etc/main -I../include/
// time ./main graph_g.txt graph_h.txt list_cost.txt list_homom.txt list_pairs.txt > continuos.txt
