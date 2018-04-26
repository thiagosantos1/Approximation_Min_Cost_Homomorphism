/* Author: Thiago Santos. --> https://github.com/thiagosantos1/      */
		
/***************************************************************************
*
* Main class to set all parameters to our LP
* As well to set all user parameters to load all files needed 
* to make our graph and its aditional informations
*
****************************************************************************/

#include <graph.h> 

int main(int argc, char const *argv[])
{
	USER_PARAMS userpar;
  GRAPH graph;
  LP_MIN_COST lp;
  LP_user_params LP_user_param;
  defult_configurations(&LP_user_param); // set all default configurations to LP

  if(argc <6){
		fprintf(stderr, "Usage <Graph G>, <Graph H>, <List Cost>, <List Homom> & <Pair List>\n");
		exit(1);
		
	}else if( argc ==8){

		/* User LP paramters  --> set by argv */
		LP_user_param.pair_consistency = atoi(argv[6]); // 1 to true & 2 to false
		LP_user_param.type_solution		 = atoi(argv[7]); // 1 to continuos & 2 to integral
	}else{

		/* User LP paramters  --> if not set by argv */
		LP_user_param.pair_consistency = 2; // 1 to true & 2 to false
		LP_user_param.type_solution		 = 1; // 1 to continuos & 2 to integral
	}

	/* User graphs paramters */
	userpar.graph_g = strdup(argv[1]);
	userpar.graph_h = strdup(argv[2]);
	userpar.list_cost = strdup(argv[3]);
	userpar.list_homom = strdup(argv[4]);
	userpar.list_pairs = strdup(argv[5]);
	userpar.results_out_put = strdup("results_LP.txt");

	makegraph(&graph, &userpar);

	set_LP_configuration(&graph,&lp,&LP_user_param,&userpar);

	LP_SOLVER(&graph, &lp,&LP_user_param,&userpar);
	
	return 0;
}
// gcc main.c makegraph.c pair_consistency.c LP_Min_Cost.c -lglpk -o ../etc/main -I../include/
// time ./main graph_g.txt graph_h.txt list_cost.txt list_homom.txt list_pairs.txt > continuos.txt
