/* Author: Thiago Santos. --> https://github.com/thiagosantos1/
   Copyright (C)
*/

#include <graph.h>  
#include <makegraph.h> 

void makegraph(GRAPH *op, USER_PARAMS * ip)
{

	init_graph_g(op,ip->graph_g);
	init_graph_h(op,ip->graph_h);
	init_list_homom(op,ip->list_homom);
	init_pairs(op,ip->list_pairs);	
	init_cost_g_h(op,ip->list_cost);	

}

void init_graph_g(GRAPH *op, char * file_name)
{
	FILE * fp = fopen(file_name, "r");
	if(fp == NULL){
		fprintf(stderr, "\nError opening file %s\n",file_name );
		exit(0);
	}
	int i,j;
	char str1[10000],str2[10000],str3[10000],str4[10000];
  fscanf(fp, "%s %s %s", str1,str3,str4);
  op->num_vert_G = atoi(str1);
  op->isBipartite_G = atoi(str3);
  op->numPartitions_G = atoi(str4);

  op->graph_g = malloc(op->num_vert_G * sizeof(uchar *));
  for(i=0; i<op->num_vert_G; i++){
  	op->graph_g[i] = malloc(op->num_vert_G * sizeof(uchar));
		memset(op->graph_g[i],0,op->num_vert_G);
	}

  while( (fscanf(fp, "%s %s", str1,str2)) ==2){
  	i = atoi(str1);
  	j = atoi(str2);
  	op->graph_g[i][j] = 1;
  }

}

void init_graph_h(GRAPH *op, char * file_name)
{
	FILE * fp = fopen(file_name, "r");
	if(fp == NULL){
		fprintf(stderr, "\nError opening file %s\n",file_name );
		exit(0);
	}
	int i,j;
	char str1[10000],str2[10000],str3[10000],str4[10000];
  fscanf(fp, "%s %s %s", str1,str3,str4);
  op->num_vert_H = atoi(str1);
  op->isBipartite_H = atoi(str3);
  op->numPartitions_H = atoi(str4);

  op->graph_h = malloc(op->num_vert_H * sizeof(uchar *));
  for(i=0; i<op->num_vert_H; i++){
  	op->graph_h[i] = malloc(op->num_vert_H * sizeof(uchar));
		memset(op->graph_h[i],0,op->num_vert_H);
	}

  while( (fscanf(fp, "%s %s", str1,str2)) ==2){
  	i = atoi(str1);
  	j = atoi(str2);
  	op->graph_h[i][j] = 1;
  }
}

void init_list_homom(GRAPH *op, char * file_name)
{
	op->list_homom_matrix = malloc(op->num_vert_G  * sizeof(uchar*));
	int MAX = op->num_vert_H * op->num_vert_H * op->num_vert_H;
	char buf[MAX];
	FILE * fp = fopen(file_name, "r");
	if(fp == NULL){
		fprintf(stderr, "\nError opening file %s\n",file_name );
		exit(0);
	}

	const char needle[2] = " ";
	char *token;
	int index;

	for (int i = 0; i < op->num_vert_G ; ++i){
		op->list_homom_matrix[i] = malloc(op->num_vert_H * sizeof(uchar)); // malloc memory 
		// put zero for all positions in H initially
		memset(op->list_homom_matrix[i],0,op->num_vert_H);

		if((fgets(buf,MAX,fp)) != NULL){

			/* get the first token */
			token = strtok(buf, needle);
			/* walk through other tokens */
			while( token != NULL ) {
			  index = atoi(token);
			  op->list_homom_matrix[i][index] = 1;
			  token = strtok(NULL, needle);
			}
		}	
	}
	//print_matrix(op->list_homom_matrix,op->num_vert_G,op->num_vert_H);
	 // puts("");
}
void init_pairs(GRAPH *op, char * file_name)
{
	op->size_g_pairs = (op->num_vert_G * op->num_vert_G ) + op->num_vert_G;
	op->size_h_pairs = (op->num_vert_H * op->num_vert_H)/8; // For each byte, we have 8 bits. Each bit represent on pair(hash key)
	op->size_h_pairs = (op->num_vert_H * op->num_vert_H) > op->size_h_pairs*8 ? op->size_h_pairs + 1:op->size_h_pairs;
	op->num_bytes = op->size_h_pairs;
	op->pairs_matrix = malloc( op->size_g_pairs *  sizeof(uchar*));

	for (int i = 0; i < op->size_g_pairs; i++){
		op->pairs_matrix[i] = malloc( (op->size_h_pairs + 1) * sizeof(uchar)); // malloc memory. One more to save with pair exist(has at least one bit set)
		memset(op->pairs_matrix[i],0,op->size_h_pairs);
	}

	// printf("%d\n", op->pairs_matrix[6][0]);
	// op->pairs_matrix[6][0] =0;
	// printf("%d\n", op->pairs_matrix[6][0]);
	//print_matrix(op->pairs_matrix,op->size_g_pairs,op->size_h_pairs);

	int x,y, x_pair,y_pair, xy_hash, pair_hash;
	int byte_position=0,bit_position=0;
	int MAX = op->num_vert_H * op->num_vert_H * op->num_vert_H;
	char buf[MAX];
	FILE * fp = fopen(file_name, "r");
	if(fp == NULL){
		fprintf(stderr, "\nError opening file %s\n",file_name );
		exit(0);
	}
	const char needle[2] = " ";
	char *token;

	int key_mode_on = 1; // first line represents the pair for which we will read its possible pairs in next lines
	while( (fgets(buf,MAX,fp)) != NULL){

		if(key_mode_on==1){
			token = strtok(buf, needle);
			x = atoi(token);
			token = strtok(NULL, needle);
			y = atoi(token);
			key_mode_on *= -1;

			KEY_HASH(xy_hash, x,y, op->num_vert_G);
			// set one bit for the last byte. It's easier to search later with this pair exists, just by looking if its last byte is >0
			SET_BIT(op->pairs_matrix[xy_hash][op->num_bytes], 0); 
		}
		else{
			// for the x and y read on previous line, set the bit for it's pairs
			token = strtok(buf, needle);

			/* walk through other tokens */
			while( token != NULL ) {
			  x_pair = atoi(token);
			  token = strtok(NULL, needle);
			  y_pair = atoi(token);
			  token = strtok(NULL, needle);

			  KEY_HASH(pair_hash, x_pair,y_pair, op->num_vert_H);
			  //printf("xy_hash: %d  pair_hash:%d\n",xy_hash, pair_hash);
			  if( xy_hash>=0 && pair_hash >=0){
			  	//byte_position =  op->size_h_pairs - 1 - (pair_hash/8); // from right to left
			  	GET_BYTE_POSITION(byte_position,op->size_h_pairs,pair_hash);
			  	//bit_position = pair_hash - ( ((op->size_h_pairs - byte_position -1)*8) ); // fix the bit position
			  	GET_BIT_POSITION(bit_position,op->size_h_pairs,pair_hash,byte_position);
			  	//printf("Byte: %d Bit: %d\n",byte_position,bit_position);
			  	//printf("test %d\n",(op->pairs_matrix[xy_hash][byte_position]));
			  	SET_BIT(op->pairs_matrix[xy_hash][byte_position], bit_position); 	  	
			  }
			}
			key_mode_on *= -1;
		}
	}
	//print_matrix(op->pairs_matrix,op->size_g_pairs,op->size_h_pairs+1);
}

void init_cost_g_h(GRAPH *op, char * file_name)
{
	int i,j;
	double cost;
	char str1[op->num_vert_G +1],str2[op->num_vert_H +1],str3[1000000];
	op->cost_g_h_matrix = malloc(op->num_vert_G * sizeof(double *));
  for(i=0; i<op->num_vert_G; i++){
  	op->cost_g_h_matrix[i] = malloc(op->num_vert_H * sizeof(double));
		memset(op->cost_g_h_matrix[i],0,op->num_vert_H);
	}

	FILE * fp = fopen(file_name, "r");
	if(fp == NULL){
		fprintf(stderr, "\nError opening file %s\n",file_name );
		exit(0);
	}

	while( (fscanf(fp, "%s %s %s", str1,str2,str3)) ==3){
  	i = atoi(str1);
  	j = atoi(str2);
  	cost = atof(str3);
  	op->cost_g_h_matrix[i][j] = cost;
  }

 //  for (int i = 0; i < op->num_vert_G; i++){
	// 	for (int j = 0; j < op->num_vert_H; j++){
	// 		printf("%.0f ", op->cost_g_h_matrix[i][j] );
	// 	}
	// 	puts("");
	// }
}

void print_matrix( uchar ** matrix, int rowns, int columns)
{
	for (int i = 0; i < rowns; i++){
		for (int j = 0; j < columns; j++){
			printf("%d ", matrix[i][j] );
		}
		puts("");
	}
}


