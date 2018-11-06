/* Author: Thiago Santos. --> https://github.com/thiagosantos1/   */

/***************************************************************************
* This class is designed to have all the information about the class
* makegraph.c, as well with more information about what each method does.
* 
* The Idea of this implementation is to get a homomophis between 2 graphs
* with the minimum cost as possible. In order to do that, we have to be sure 
* to consider few constraints, such as Min-Max ordering and others.
* 
* Copyright (C)
*
****************************************************************************/

/* Based on the file given from user, this method initialize the list homomphis 
   which is all possible assigments in H, for each vertex in G*/
void init_list_homom(GRAPH *op, char * file_name);

/* Based on the file given from user, this method initialize the list of all possible paris
   for each pair of vertex in G. This is based on key(hash) */
void init_pairs(GRAPH *op, char * file_name);

/* Initialize graph */
void init_graph_g(GRAPH *op, char * file_name);

/* Initialize graph */
void init_graph_h(GRAPH *op, char * file_name);

/* Initialize matrix with all cost of mapping a vertex G to a vertex in H */
void init_cost_g_h(GRAPH *op, char * file_name);

/* print a given matrix */
void print_matrix(uchar ** matrix, int rowns, int columns);