/*
	Author: Thiago Santos
	Class to designed to execute the pair consistency.

	You must pass the List and the pair list through argv

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Set bit y (0-indexed) of x to '1' by generating a a mask with a '1' in the proper bit location and ORing x with the mask.
#define SET_BIT(X,Y) X |= (1 << Y)

//Set bit y (0-indexed) of x to '0' by generating a mask with a '0' in the y position and 1's elsewhere then ANDing the mask with x.
#define CLEAR_BIT(X,Y) X &= ~(1<< Y)

//Return '1' if the bit value at position y within x is '1' and '0' if it's 0 
#define READ_BIT(X,Y) ((0u == (X & (1<<Y)))?0u:1u)

#define GET_BYTE_POSITION(byte_position,size_h,pair_hash) byte_position =  size_h - 1 - (pair_hash/8) // from right to left
#define GET_BIT_POSITION(bit_position,size_h,pair_hash,byte_position) bit_position = pair_hash - ( ((size_h - byte_position -1)*8) )
#define KEY_HASH(KEY,X,Y,N) KEY = (X*N)+Y;

typedef 
	unsigned char
uchar;

int NUM_BYTES; // to save how many bytes in to make pairs with H
int size_g;
uchar ** list_matrix; // matrix of G x H that saves each vertice in H available to G
uchar ** pairs_matrix; // Matrix of G squared +G X H squared. Saves(by bit set) each pair(hash key) available for that pair in G(also a key)

void init_list(int num_vert_G, int num_vert_H, const char * list_file);
void init_pairs(int num_vert_G, int num_vert_H, const char *pairs_file);
void print_matrix(uchar ** matrix, int rowns, int columns);

void pair_consistency(int num_vert_G, int num_vert_H);
int pair_reduction(int num_vert_G,int num_vert_H, int x, int y, int z);

int main(int argc, char const *argv[])
{
	
	if(argc <5){

		fprintf(stderr, "Usage <Num Vertices in G>, <Num Vertices in H>, <List file> & <Pair List>\n");
		exit(1);
	}

	int num_vert_G = 	atoi(argv[1]);
	int num_vert_H = 	atoi(argv[2]);

	const char * list_file = argv[3];
	const char * pairs_file = argv[4];

	init_list(num_vert_G, num_vert_H, list_file);
	init_pairs(num_vert_G, num_vert_H, pairs_file);	

	pair_consistency(num_vert_G,num_vert_H);
	puts("");
	print_matrix(pairs_matrix,size_g,NUM_BYTES+1);

	return 0;
}

void init_list(int num_vert_G, int num_vert_H, const char * list_file)
{
	list_matrix = malloc(num_vert_G * sizeof(uchar*));
	int MAX = num_vert_H * num_vert_H * num_vert_H;
	char buf[MAX];
	FILE * fp = fopen(list_file, "r");
	if(fp == NULL){
		fprintf(stderr, "\nError opening file %s\n",list_file );
		exit(0);
	}

	const char needle[2] = " ";
	char *token;
	int index;

	for (int i = 0; i < num_vert_G; ++i){
		list_matrix[i] = malloc(num_vert_H * sizeof(uchar)); // malloc memory 
		// put zero for all positions in H initially
		memset(list_matrix[i],0,num_vert_H);

		if((fgets(buf,MAX,fp)) != NULL){

			/* get the first token */
			token = strtok(buf, needle);
			/* walk through other tokens */
			while( token != NULL ) {
			  index = atoi(token);
			  list_matrix[i][index] = 1;
			  token = strtok(NULL, needle);
			}
		}	
	}
	 // print_matrix(list_matrix,num_vert_G,num_vert_H);
	 // puts("");
}
void init_pairs(int num_vert_G, int num_vert_H, const char *pairs_file)
{
	size_g = (num_vert_G * num_vert_G ) + num_vert_G;
	int size_h = (num_vert_H * num_vert_H)/8; // For each byte, we have 8 bits. Each bit represent on pair(hash key)
	size_h = (num_vert_H * num_vert_H) > size_h*8 ? size_h + 1:size_h;
	NUM_BYTES = size_h;
	pairs_matrix = malloc( size_g *  sizeof(uchar*));

	for (int i = 0; i < size_g; i++){
		pairs_matrix[i] = malloc( (size_h + 1) * sizeof(uchar)); // malloc memory. One more to save with pair exist(has at least one bit set)
		memset(pairs_matrix[i],0,size_h);
	}

	// printf("%d\n", pairs_matrix[6][0]);
	// pairs_matrix[6][0] =0;
	// printf("%d\n", pairs_matrix[6][0]);
	//print_matrix(pairs_matrix,size_g,size_h);

	int x,y, x_pair,y_pair, xy_hash, pair_hash;
	int byte_position=0,bit_position=0;
	int MAX = num_vert_H * num_vert_H * num_vert_H;
	char buf[MAX];
	FILE * fp = fopen(pairs_file, "r");
	if(fp == NULL){
		fprintf(stderr, "\nError opening file %s\n",pairs_file );
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

			KEY_HASH(xy_hash, x,y, num_vert_G);
			// set one bit for the last byte. It's easier to search later with this pair exists, just by looking if its last byte is >0
			SET_BIT(pairs_matrix[xy_hash][NUM_BYTES], 0); 
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

			  KEY_HASH(pair_hash, x_pair,y_pair, num_vert_H);
			  //printf("xy_hash: %d  pair_hash:%d\n",xy_hash, pair_hash);
			  if( xy_hash>=0 && pair_hash >=0){
			  	//byte_position =  size_h - 1 - (pair_hash/8); // from right to left
			  	GET_BYTE_POSITION(byte_position,size_h,pair_hash);
			  	//bit_position = pair_hash - ( ((size_h - byte_position -1)*8) ); // fix the bit position
			  	GET_BIT_POSITION(bit_position,size_h,pair_hash,byte_position);
			  	//printf("Byte: %d Bit: %d\n",byte_position,bit_position);
			  	//printf("test %d\n",(pairs_matrix[xy_hash][byte_position]));
			  	SET_BIT(pairs_matrix[xy_hash][byte_position], bit_position); 	  	
			  }
			}
			key_mode_on *= -1;
		}
	}
	print_matrix(pairs_matrix,size_g,size_h+1);
	
}

void pair_consistency(int num_vert_G, int num_vert_H)
{
	int done = -1,stop = 1; 

	while( done <=0 ){

		done = 1;

		for(int x=0; x<num_vert_G; x++){ // for all vertices in G

			for(int y=0; y<num_vert_G; y++){ // check with all others vertices

				if(x!=y){

					for(int z=0; z<num_vert_G; z++){  // check with a third variable

						if(z!=y){
							stop = pair_reduction(num_vert_G, num_vert_H, x, y, z);
							if(stop <=0)
								done = -1;
						}
					}
				}
			}
		}
	}
}

int pair_reduction(int num_vert_G,int num_vert_H, int x, int y, int z)
{
	int done = 1, eliminate_a_b = 1, a_lis = 0, b_lis = 0, c_lis = 0, pair  = 0, pair_a_c = 0, pair_b_c = 0;
	int key_a = 0, key_b =0, key=0,byte_position=0,bit_position=0;

	KEY_HASH(key_a,x,z,num_vert_G);
	KEY_HASH(key_b,y,z,num_vert_G);
	KEY_HASH(key,x,y,num_vert_G);

	// if there's no pair for any of the key, or there's no such key
	if( ( pairs_matrix[key_a][NUM_BYTES] <=0 ) || ( pairs_matrix[key_b][NUM_BYTES] <=0 ) || ( pairs_matrix[key][NUM_BYTES] <=0 ))
		return 1; // done 

	for(a_lis=0; a_lis<num_vert_H; a_lis++){ // for all elem in the list(x) of possible match
		if(list_matrix[x][a_lis]>0){ // if exist in the list of X

			for(b_lis=0; b_lis<num_vert_H; b_lis++){ // for all elem in the list(y) of possible match
				if(list_matrix[y][b_lis]>0){ // if exist in the list of y

					for(c_lis=0; c_lis<num_vert_H; c_lis++){ // for all elem in the list(z) of possible match
						if(list_matrix[z][c_lis]>0){ // if exist in the list of Z

							KEY_HASH(pair_a_c,a_lis,c_lis,num_vert_H);
							KEY_HASH(pair_b_c,b_lis,c_lis,num_vert_H);
							KEY_HASH(pair,a_lis,b_lis,num_vert_H);

							// get the byte and bit of each key 
							GET_BYTE_POSITION(byte_position,NUM_BYTES,pair_a_c);
			  			GET_BIT_POSITION(bit_position,NUM_BYTES,pair_a_c,byte_position);
			  			// if pair_a_c is in the list of key_a (if its bit is set)
			  			if( (READ_BIT(pairs_matrix[key_a][byte_position],bit_position)) >0){
			  				// pair_b_c also must be in the list of key_b
			  				GET_BYTE_POSITION(byte_position,NUM_BYTES,pair_b_c);
				  			GET_BIT_POSITION(bit_position,NUM_BYTES,pair_b_c,byte_position);
				  			// if pair_b_c is in the list of key_a (if its bit is set)
				  			if( (READ_BIT(pairs_matrix[key_b][byte_position],bit_position)) >0)
				  				eliminate_a_b = -1; // then, don't remove pair
			  			}
						}
					}

					if(eliminate_a_b >0){
						GET_BYTE_POSITION(byte_position,NUM_BYTES,pair);
		  			GET_BIT_POSITION(bit_position,NUM_BYTES,pair,byte_position);
		  			// if pair_a_c is in the list of key_a (if its bit is set)
		  			if( (READ_BIT(pairs_matrix[key][byte_position],bit_position)) >0){
		  				CLEAR_BIT(pairs_matrix[key][byte_position],bit_position); // remove pair(set bit to zero)
		  				done = -1;
		  			}
					}
					eliminate_a_b = 1;
				}
			}
		}
	}
	return done;
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






