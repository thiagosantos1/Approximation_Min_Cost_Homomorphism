#include <graph.h> 

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

	makegraph(&graph, &userpar);
	//pair_consistency(&graph, &userpar);
	LP_SOLVER(&graph, &lp);
	return 0;
}

//gcc main.c makegraph.c pair_consistency.c LP_Min_Cost.c -lglpk -o ../etc/main -I../include/
