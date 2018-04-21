/*
	Author: Thiago Santos
	Class to designed to execute the pair consistency.

	You must pass the List and the pair list through argv

*/

#include <graph.h> 


int pair_reduction(GRAPH *op, USER_PARAMS * ip, int num_vert_G,int num_vert_H, int x, int y, int z);

void pair_consistency(GRAPH *op, USER_PARAMS * ip)
{
	int done = -1,stop = 1; 

	while( done <=0 ){

		done = 1;

		for(int x=0; x<op->num_vert_G ; x++){ // for all vertices in G

			for(int y=0; y<op->num_vert_G ; y++){ // check with all others vertices

				if(x!=y){

					for(int z=0; z<op->num_vert_G ; z++){  // check with a third variable

						if(z!=y){
							stop = pair_reduction(op, ip, op->num_vert_G , op->num_vert_H , x, y, z);
							if(stop <=0)
								done = -1;
						}
					}
				}
			}
		}
	}
}

int pair_reduction(GRAPH *op, USER_PARAMS * ip, int num_vert_G,int num_vert_H, int x, int y, int z)
{
	int done = 1, eliminate_a_b = 1, a_lis = 0, b_lis = 0, c_lis = 0, pair  = 0, pair_a_c = 0, pair_b_c = 0;
	int key_a = 0, key_b =0, key=0,byte_position=0,bit_position=0;

	KEY_HASH(key_a,x,z,num_vert_G);
	KEY_HASH(key_b,y,z,num_vert_G);
	KEY_HASH(key,x,y,num_vert_G);

	// if there's no pair for any of the key, or there's no such key
	if( ( op->pairs_matrix[key_a][op->num_bytes ] <=0 ) || ( op->pairs_matrix[key_b][op->num_bytes ] <=0 ) || ( op->pairs_matrix[key][op->num_bytes ] <=0 ))
		return 1; // done 

	for(a_lis=0; a_lis<num_vert_H; a_lis++){ // for all elem in the list(x) of possible match
		if(op->list_homom_matrix[x][a_lis]>0){ // if exist in the list of X

			for(b_lis=0; b_lis<num_vert_H; b_lis++){ // for all elem in the list(y) of possible match
				if(op->list_homom_matrix[y][b_lis]>0){ // if exist in the list of y

					for(c_lis=0; c_lis<num_vert_H; c_lis++){ // for all elem in the list(z) of possible match
						if(op->list_homom_matrix[z][c_lis]>0){ // if exist in the list of Z

							KEY_HASH(pair_a_c,a_lis,c_lis,num_vert_H);
							KEY_HASH(pair_b_c,b_lis,c_lis,num_vert_H);
							KEY_HASH(pair,a_lis,b_lis,num_vert_H);

							// get the byte and bit of each key 
							GET_BYTE_POSITION(byte_position,op->num_bytes ,pair_a_c);
				  		GET_BIT_POSITION(bit_position,op->num_bytes ,pair_a_c,byte_position);
				  		// if pair_a_c is in the list of key_a (if its bit is set)
				  		if( (READ_BIT(op->pairs_matrix[key_a][byte_position],bit_position)) >0){
				  			// pair_b_c also must be in the list of key_b
				  			GET_BYTE_POSITION(byte_position,op->num_bytes ,pair_b_c);
					  		GET_BIT_POSITION(bit_position,op->num_bytes ,pair_b_c,byte_position);
					 			// if pair_b_c is in the list of key_a (if its bit is set)
					 			if( (READ_BIT(op->pairs_matrix[key_b][byte_position],bit_position)) >0)
				  				eliminate_a_b = -1; // then, don't remove pair
			  			}
						}
					}

					if(eliminate_a_b >0){
							GET_BYTE_POSITION(byte_position,op->num_bytes ,pair);
			  			GET_BIT_POSITION(bit_position,op->num_bytes ,pair,byte_position);
			  			// if pair is in the list of key(if its bit is set)
			  			if( (READ_BIT(op->pairs_matrix[key][byte_position],bit_position)) >0){
			  				CLEAR_BIT(op->pairs_matrix[key][byte_position],bit_position); // remove pair(set bit to zero)
			  				done = -1;

			  				// if the key does not have any other pair, set it's last bit to zero
			  				int list_empty = 1;
			  				for(int i=0; i<op->num_bytes ;i++){
			  					if(op->pairs_matrix[key][i]>0){
			  						list_empty = -1;
			  						break;
			  					}
			  				}
			  				if(list_empty >0)
			  					CLEAR_BIT(op->pairs_matrix[key][op->num_bytes ],0); // set list of Key to empty
			  			}
					}
					eliminate_a_b = 1;
				}
			}
		}
	}
	return done;
}

