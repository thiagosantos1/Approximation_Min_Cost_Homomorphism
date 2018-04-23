/* Author: Thiago Santos. --> https://github.com/thiagosantos1/   */

/***************************************************************************
* This class is designed to have all the information about the main class
* LP_Min_Cost, as well with more information about what each method does.
* 
* The Idea of this implementation is to get a homomophis between 2 graphs
* with the minimum cost as possible. In order to do that, we have to be sure 
* to consider few constraints, such as Min-Max ordering and others.
* 
* Copyright (C)
*
****************************************************************************/


/* function to initialize LP, with the desired paramters and consigurations */
void initi_LP(GRAPH *op, LP_MIN_COST * lp);

/* function to check if more memory is needed */
void reallocate_memory_check(LP_MIN_COST * lp); 

/* function to get first in_neighbor of a vertex in H, based on the range given */
int get_first_in_neighbor(GRAPH *op, int vertex, int begi_, int end_); 

/* function to get first out_neighbor of a vertex in H, based on the range given */
int get_first_out_neighbor(GRAPH *op, int vertex, int begi_, int end_);

/* Function to set the objective function of the LP */
void set_obj_function(GRAPH *op, LP_MIN_COST * lp);

/* add constraint to lp, based on ai & aj indexes, and the coeficiente value of the variable */
void add_constraint(LP_MIN_COST * lp, int ai_value, int aj_value, double ar_coef_value);

/* for every vertex in G Constraint Xui - Xui+1 >=0 */
void constr_XUi_XUiplus1(GRAPH *op, LP_MIN_COST * lp);

/* constraints based if i is available for Xu or Xv */
void constr_list_Xui_Xuiplus1(GRAPH *op, LP_MIN_COST * lp);

/* for each vertex in G, you check agains all vertex in H if there is an edge, and vice-versa
   and for each pair, we check against all vertex in G again, if there's an edge
   Then, Xv_innbr -Xuj >=0 */
void constr_in_neighbor(GRAPH *op, LP_MIN_COST * lp);

/* for each vertex in G, you check agains all vertex in H if there is an edge, and vice-versa
   and for each pair, we check against all vertex in G again, if there's an edge
   Then, Xv_outnbr -Xuj >=0 */
void constr_out_neighbor(GRAPH *op, LP_MIN_COST * lp);

/*
add constraint for missing edges(From Mix_Max ordering)
to any Diagraph H.
for more details, please check the formulas at the paper, page 7
*/
void constr_missing_edges(GRAPH *op, LP_MIN_COST * lp);


/* Constraint based on pair consistency 
	Xui - Xu,i+1 <= Sum(Xvj - Xv,j+1) --> For all u,v in G | (i,j) in L(u,v)
	L(u,v) = { (i,j), (i,j+1)....}
	Thus --> Sum(Xvj - Xv,j+1) - Xui + Xu,i+1 >=0
	L(u,v) = { (i,j), (i,j+1)....}
*/
void constr_pairs_list(GRAPH *op, LP_MIN_COST * lp);

/* Construct the LP, by calling all needed functions, such as the set_obj_function 
   and all constraints needed for the LP */
void construct_LP(GRAPH *op, LP_MIN_COST * lp, LP_user_params * lp_param);

/* Print results from solution found by LP*/
void print_results(GRAPH *op, LP_MIN_COST * lp);