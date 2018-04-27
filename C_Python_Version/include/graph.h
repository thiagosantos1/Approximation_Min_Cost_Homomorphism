/* Author: Thiago Santos. --> https://github.com/thiagosantos1/  */

/***************************************************************************
* This class is designed to have all the information needed for other 
* classes that uses any information about the graph, as well about the LP
* 
* The User(main) has visibility to only the methods included here. 
*
* The Idea of this implementation is to get a homomophis between 2 graphs
* with the minimum cost as possible. In order to do that, we have to be sure 
* to consider few constraints, such as Min-Max ordering and others.
*
* Copyright (C)
*
****************************************************************************/

#include <stdio.h>  
#include <stdlib.h>
#include <string.h> 
#include <glpk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//Set bit y (0-indexed) of x to '1' by generating a a mask with a '1' in the proper bit location and ORing x with the mask.
#define SET_BIT(X,Y) X |= (1 << Y)

//Set bit y (0-indexed) of x to '0' by generating a mask with a '0' in the y position and 1's elsewhere then ANDing the mask with x.
#define CLEAR_BIT(X,Y) X &= ~(1<< Y)

//Return '1' if the bit value at position y within x is '1' and '0' if it's 0 
#define READ_BIT(X,Y) ((0u == (X & (1<<Y)))?0u:1u)

#define GET_BYTE_POSITION(byte_position,size_h,pair_hash) byte_position =  size_h - 1 - (pair_hash/8) // from right to left
#define GET_BIT_POSITION(bit_position,size_h,pair_hash,byte_position) bit_position = pair_hash - ( ((size_h - byte_position -1)*8) )
#define KEY_HASH(KEY,X,Y,N) KEY = (X*N)+Y;

// return the index of pair U,V in the constraint --> pairs starting at 0,0
#define INDEX_CONSTRAINT_G_H(U,V,NUM_G,NUM_H) ( ((U) * NUM_H) + V+1) 

typedef 
	unsigned char
uchar;

typedef struct graphdata {
  uchar ** list_homom_matrix; // matrix of G x H that saves each vertice in H available to G
	uchar ** pairs_matrix; // Matrix of G squared +G X H squared. Saves(by bit set) each pair(hash key) available for that pair in G(also a key)
	double ** cost_g_h_matrix;

	uchar ** graph_g;
	uchar ** graph_h;

	int num_bytes; // to save how many bytes in to make pairs with H
	int num_vert_G;
	int num_vert_H;
	int size_g_pairs; // use to create pairs_matrix and to acess it
	int size_h_pairs;

} GRAPH;

typedef struct userdata {
  char *graph_g;  // Graph G file name
  char *graph_h;
  char *list_cost;
  char *full_list;
  char *list_homom;
  char *list_pairs;
  char * results_out_put;

} USER_PARAMS;


typedef struct LPdata {
	int *ia; // save the index i of the constraint matrix
	int *ja; // save the index j of the constraint matrix
	double *ar; // save the value/coeficient of the constraing at [i,j]
	int num_constraints;
	int allocation_size; // size to realocate memory
	int memory_left; // keep track if we still have space in ia,ja & ar
	int variable_type;
	int num_nonzero_constraints;

	glp_prob *mip;

} LP_MIN_COST;

typedef struct LPparam {
	/* 1 to continuos(default) 
	   2 to integral  - Optimal*/
	int type_solution; 

	/* 1(default) to run program with pair consistency
	   2 to run without it. */
	int pair_consistency;

} LP_user_params;


/* Make graph in the class makegraph.c by reading all the files set by the user in user params*/
void makegraph(GRAPH *op, USER_PARAMS *ip);

/* run pair consistency, in the class pair_consistency.c*/
void pair_consistency(GRAPH *op, USER_PARAMS * ip);

/* Create and solve the LP desired, in the class LP_Min_Cost.c */
void LP_SOLVER(GRAPH *op, LP_MIN_COST * lp, LP_user_params * lp_param,USER_PARAMS * ip);

/* Set the default configuration to LP in the class LP_Min_Cost.c */
void defult_configurations(LP_user_params * lp_param);

/* Set configuration of LP, based on LP_user_params in the class LP_Min_Cost.c */
void set_LP_configuration(GRAPH *op, LP_MIN_COST * lp, LP_user_params * lp_param,USER_PARAMS * ip);