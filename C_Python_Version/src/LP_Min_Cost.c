#include <graph.h> 

#define PRINT_RESULTS

void initi_LP(GRAPH *op, LP_MIN_COST * lp);
// function to check if more memory is needed
void reallocate_memory_check(LP_MIN_COST * lp); 

// functions to get first in_out neighbor of a vertex in H
int get_first_in_neighbor(GRAPH *op, int vertex, int begi_, int end_); 
int get_first_out_neighbor(GRAPH *op, int vertex, int begi_, int end_);

void set_obj_function(GRAPH *op, LP_MIN_COST * lp);

// add constraint to lp, based on ai & aj indexes, and the coeficiente value of the variable
void add_constraint(LP_MIN_COST * lp, int ai_value, int aj_value, double ar_coef_value);
//for every vertex in G Constraint Xui - Xui+1 >=0
void constr_XUi_XUiplus1(GRAPH *op, LP_MIN_COST * lp);

// constraints based if i is available for Xu or Xv
void constr_list_Xui_Xuiplus1(GRAPH *op, LP_MIN_COST * lp);

// for each vertex in G, you check agains all vertex in H if there is an edge, and vice-versa
// and for each pair, we check against all vertex in G again, if there's an edge
// Then, Xv_innbr -Xuj >=0
void constr_in_neighbor(GRAPH *op, LP_MIN_COST * lp);
// Then, Xv_outnbr -Xuj >=0
void constr_out_neighbor(GRAPH *op, LP_MIN_COST * lp);

/*
add constraint for missing edges(From Mix_Max ordering)
to any Diagraph H.
for more details, please check the formulas at the paper, page 7
*/
void constr_missing_edges(GRAPH *op, LP_MIN_COST * lp);

void construct_LP(GRAPH *op, LP_MIN_COST * lp);

void LP_SOLVER(GRAPH *op, LP_MIN_COST * lp)
{
	construct_LP(op,lp);
	// for(int i=0; i<=lp->num_nonzero_constraints; i++)
	// 	printf("ai: %d, aj: %d, ar: %0.f\n",lp->ia[i],lp->ja[i],lp->ar[i] );

	glp_load_matrix(lp->mip, lp->num_nonzero_constraints, lp->ia, lp->ja, lp->ar);

	glp_iocp parm;
	glp_init_iocp(&parm);
	parm.presolve = GLP_ON;
	int err = glp_intopt(lp->mip, &parm);

	double total_cost = glp_mip_obj_val(lp->mip);
	printf("Total Cost of Cut %.2f\n",total_cost );

	#ifdef PRINT_RESULTS 

		int count=0,i=0,g;
		printf("     ");
		for(i; i<op->num_vert_H;i++)
			printf("H %d  ",i);
		i=0;
		printf("\nG %d: ",i );
		for(i; i<op->num_vert_G * op->num_vert_H;i++){
			printf("%.02f ", glp_mip_col_val(lp->mip, i+1)); 
			count++;
			if(count == op->num_vert_H){
				printf("\n");
				g = (i/op->num_vert_H)+1;
				if(g < op->num_vert_G)
					printf("G %d: ",g);
				count =0;
			}
		}
	#endif

	glp_delete_prob(lp->mip);
}

void construct_LP(GRAPH *op, LP_MIN_COST * lp)
{
	initi_LP(op,lp);
	set_obj_function(op,lp);
	constr_XUi_XUiplus1(op,lp);
	constr_list_Xui_Xuiplus1(op,lp);
	constr_in_neighbor(op,lp);
	constr_out_neighbor(op,lp);
	constr_missing_edges(op,lp);

}

void initi_LP(GRAPH *op, LP_MIN_COST * lp)
{
	int i;
	lp->mip = glp_create_prob();
	glp_set_prob_name(lp->mip, "Min Cost Homomophism");
	glp_set_obj_dir(lp->mip, GLP_MIN);

	lp->allocation_size = 1000;
	lp->memory_left			= 	 0;
	lp->num_constraints =    0;

	lp->variable_type = GLP_CV;
	lp->num_nonzero_constraints = 0;

	// alocate for the firs time memory for the arrays that construct the LP
	// we put +1, because GLPK does not count index 0, only from 1;
	// once we start creating the constraint, and memory_left =0, we reallocate more memory
	lp->ia = malloc( (lp->allocation_size +1) * sizeof(int)); 
	lp->ja = malloc( (lp->allocation_size +1) * sizeof(int)); 
	lp->ar = malloc( (lp->allocation_size +1) * sizeof(double)); 

	lp->memory_left = lp->allocation_size;
} 

void reallocate_memory_check(LP_MIN_COST * lp)
{
	if(lp->memory_left ==0){
		lp->ia = realloc(lp->ia, (lp->allocation_size + lp->num_nonzero_constraints+1) * sizeof(int));  
		lp->ja = realloc(lp->ja, (lp->allocation_size + lp->num_nonzero_constraints+1) * sizeof(int)); 
		lp->ar = realloc(lp->ar, (lp->allocation_size + lp->num_nonzero_constraints+1) * sizeof(double)); 
		lp->memory_left = lp->allocation_size;
	}
}

int get_first_in_neighbor(GRAPH *op, int vertex, int begi_, int end_)
{
	int result = -1; // no in neighbor;
	int vertex_pair =0;
	if(begi_ >=0 && end_<op->num_vert_H){
		for(vertex_pair=begi_; vertex_pair<= end_; vertex_pair++){
			if(op->graph_h[vertex_pair][vertex] ==1){
				result = vertex_pair;
				break;
			}
		}
	}

	return result;
} 
int get_first_out_neighbor(GRAPH *op, int vertex, int begi_, int end_)
{
	int result = -1; // no out neighbor;
	int vertex_pair =0;
	if(begi_ >=0 && end_<op->num_vert_H){
		for(vertex_pair=begi_; vertex_pair<= end_; vertex_pair++){
			if(op->graph_h[vertex][vertex_pair] ==1){
				result = vertex_pair;
				break;
			}
		}
	}

	return result;
}

void set_obj_function(GRAPH *op, LP_MIN_COST * lp)
{
	int u,j,start =-1,save_index=1;
	double coef;

	//for every vertex in G 
	for(u=0; u<op->num_vert_G; u++){
		start =-1;
		// check for it's possible assigns in H
		for(j=0; j<op->num_vert_H; j++){
			coef = op->cost_g_h_matrix[u][j];
			if(coef!=0){

				if(start <=0){
					glp_add_cols(lp->mip, 1);
				  glp_set_col_name(lp->mip, save_index, "x");
				  glp_set_col_bnds(lp->mip, save_index, GLP_DB, 0.0, 1.0);
					glp_set_obj_coef(lp->mip, save_index, coef);
					glp_set_col_kind(lp->mip, save_index, lp->variable_type); // set variable to continuos
					start = 1;
				}else{
					coef -= op->cost_g_h_matrix[u][j-1];
					glp_add_cols(lp->mip, 1);
				  glp_set_col_name(lp->mip, save_index, "x");
				  glp_set_col_bnds(lp->mip, save_index, GLP_DB, 0.0, 1.0);
					glp_set_obj_coef(lp->mip, save_index, coef);
					glp_set_col_kind(lp->mip, save_index, lp->variable_type); // set variable to continuos
				}
				save_index++;
			}
		}
	}
}

void add_constraint(LP_MIN_COST * lp, int ia_value, int ja_value, double ar_coef_value)
{
	lp->num_nonzero_constraints +=1;
	lp->ia[lp->num_nonzero_constraints] = ia_value; // index i of matrix
	lp->ja[lp->num_nonzero_constraints] = ja_value; // index j of matrix
	lp->ar[lp->num_nonzero_constraints] = ar_coef_value; // coeficiente of the variable 
	lp->memory_left -=1;
	reallocate_memory_check(lp);

}

void constr_XUi_XUiplus1(GRAPH *op, LP_MIN_COST * lp)
{
	int index_colum_G_H=0, u,j,start;
	//for every vertex in G 
	for(u=0; u<op->num_vert_G; u++){
		start =-1; // used for the first Ui in the order
		// check for it's possible assigns in H
		for(j=0; j<op->num_vert_H; j++){
			if(start<=0){ // for first one --> Xuj | j=0
				index_colum_G_H = INDEX_CONSTRAINT_G_H(u,j,op->num_vert_G, op->num_vert_H); 
				lp->num_constraints +=1;
				glp_add_rows(lp->mip, 1); // add new rows for a new costraint
				glp_set_row_name(lp->mip, lp->num_constraints, "c1");
				glp_set_row_bnds(lp->mip, lp->num_constraints, GLP_FX, 1.0, 1.0);

				add_constraint(lp, lp->num_constraints, index_colum_G_H,1);
				start = 1;
			}else{ // Xuj - Xuj + 1 >= 0
				if(j < op->num_vert_H-1){
					index_colum_G_H = INDEX_CONSTRAINT_G_H(u,j,op->num_vert_G, op->num_vert_H); 
					lp->num_constraints +=1;
					glp_add_rows(lp->mip, 1); // add new rows for a new costraint
					glp_set_row_name(lp->mip, lp->num_constraints, "c1");
					glp_set_row_bnds(lp->mip, lp->num_constraints, GLP_LO, 0.0, 0.0);

					// Xuj = 1
					add_constraint(lp, lp->num_constraints, index_colum_G_H,1);

					// Xuj +1 = -1
					index_colum_G_H = INDEX_CONSTRAINT_G_H(u,j+1,op->num_vert_G, op->num_vert_H); 
					add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);
				}
			}
		}
	}
}

// add constraint Xuj = Xu,j+1 if j not in L(u)
void constr_list_Xui_Xuiplus1(GRAPH *op, LP_MIN_COST * lp)
{
	int index_colum_G_H=0, u,j;
	//for every vertex in G 
	for(u=0; u<op->num_vert_G; u++){
		// check for it's possible assigns in H
		for(j=0; j<op->num_vert_H; j++){

			if (op->list_homom_matrix[u][j] != 1){ //j not in L(u)
				// not the last one. Thus --> #Xuj = Xuj+1 if j not in L(u)
				if(j < op->num_vert_H-1){ 
					index_colum_G_H = INDEX_CONSTRAINT_G_H(u,j,op->num_vert_G, op->num_vert_H); 
					lp->num_constraints +=1;
					glp_add_rows(lp->mip, 1); // add new rows for a new costraint
					glp_set_row_name(lp->mip, lp->num_constraints, "c1");
					glp_set_row_bnds(lp->mip, lp->num_constraints, GLP_FX, 0.0, 0.0);

					//Xuj = 1 (coeficiente)
					add_constraint(lp, lp->num_constraints, index_colum_G_H,1);

					//Xuj+1 = -1 (coeficiente)
					index_colum_G_H = INDEX_CONSTRAINT_G_H(u,j+1,op->num_vert_G, op->num_vert_H); 
					add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);

				}else{ // if it's the last one, Xuj = 0 if j not in L(u)
					index_colum_G_H = INDEX_CONSTRAINT_G_H(u,j,op->num_vert_G, op->num_vert_H); 
					lp->num_constraints +=1;
					glp_add_rows(lp->mip, 1); // add new rows for a new costraint
					glp_set_row_name(lp->mip, lp->num_constraints, "c1");
					glp_set_row_bnds(lp->mip, lp->num_constraints, GLP_FX, 0.0, 0.0);

					add_constraint(lp, lp->num_constraints, index_colum_G_H,1);

				}
			}

		}
	}

}

void constr_in_neighbor(GRAPH *op, LP_MIN_COST * lp)
{
	int u,j,v,in_nbr_j,index_colum_G_H;
	//for every vertex in G 
	for(u=0; u<op->num_vert_G; u++){
		// check for it's possible assigns in H
		for(j=0; j<op->num_vert_H; j++){
			//then for each pair, you compare with all vertex in G, if there is an edge
			for(v=0; v<op->num_vert_G; v++){
				if(op->graph_g[u][v] ==1){
					in_nbr_j = get_first_in_neighbor(op,j,0,op->num_vert_H -1);
					// if there is one
					if(in_nbr_j >=0){
						//Xj_innbr -Xuj >=0
						index_colum_G_H = INDEX_CONSTRAINT_G_H(u,j,op->num_vert_G, op->num_vert_H); 
						lp->num_constraints +=1;
						glp_add_rows(lp->mip, 1); // add new rows for a new costraint
						glp_set_row_name(lp->mip, lp->num_constraints, "c1");
						glp_set_row_bnds(lp->mip, lp->num_constraints, GLP_LO, 0.0, 0.0);

						// Xuj = -1
						add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);

						// Xj_innbr = 1
						index_colum_G_H = INDEX_CONSTRAINT_G_H(v,in_nbr_j,op->num_vert_G, op->num_vert_H); 
						add_constraint(lp, lp->num_constraints, index_colum_G_H,1);

					}
				}

			}
		}
	}
}

void constr_out_neighbor(GRAPH *op, LP_MIN_COST * lp)
{
	int u,j,v,out_nbr_j,index_colum_G_H;
	//for every vertex in G 
	for(u=0; u<op->num_vert_G; u++){
		// check for it's possible assigns in H
		for(j=0; j<op->num_vert_H; j++){
			//then for each pair, you compare with all vertex in G, if there is an edge
			for(v=0; v<op->num_vert_G; v++){
				if(op->graph_g[u][v] ==1){
					out_nbr_j = get_first_out_neighbor(op,j,0,op->num_vert_H -1);
					// if there is one
					if(out_nbr_j >=0){
						//Xj_outnbr -Xuj >=0
						index_colum_G_H = INDEX_CONSTRAINT_G_H(u,j,op->num_vert_G, op->num_vert_H); 
						lp->num_constraints +=1;
						glp_add_rows(lp->mip, 1); // add new rows for a new costraint
						glp_set_row_name(lp->mip, lp->num_constraints, "c1");
						glp_set_row_bnds(lp->mip, lp->num_constraints, GLP_LO, 0.0, 0.0);

						// Xuj = -1
						add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);

						// Xj_outnbr = 1
						index_colum_G_H = INDEX_CONSTRAINT_G_H(v,out_nbr_j,op->num_vert_G, op->num_vert_H); 
						add_constraint(lp, lp->num_constraints, index_colum_G_H,1);

					}
				}
			}
		}
	}
}

void constr_missing_edges(GRAPH *op, LP_MIN_COST * lp)
{
	int u,j,v,i,s,t,first_in_nbr_j,first_out_nbr_i,index_colum_G_H;
	//for every vertex in G 
	for(u=0; u<op->num_vert_G; u++){
		//# for all uv | uv E A(G)
		for(v=0; v<op->num_vert_G; v++){
			if(op->graph_g[u][v] ==1){
				for(i=0; i<op->num_vert_H; i++){ //for possible assigns in u( in the homom list)
					if(op->list_homom_matrix[u][i]){ // if i is available in the list of u

						for(j=0; j<op->num_vert_H; j++){ //for possible assigns in v( in the homom list)
							if(op->list_homom_matrix[v][j]){ // if j is available in the list of v

								if(op->graph_h[i][j] != 1){ // if ij !E A(H)
									first_in_nbr_j = get_first_in_neighbor(op,j,0,op->num_vert_H -1);
									first_out_nbr_i = get_first_out_neighbor(op,i,0,j); // between 0 and j, inclusive
									if(first_in_nbr_j >=0 && first_out_nbr_i>=0){ // if there is one 

										if(first_in_nbr_j <i && first_out_nbr_i <j){

											//first base constrain -- For out-neighbors
                    	//first out neighbor of i, after j  
											s = get_first_out_neighbor(op,i,j+1,op->num_vert_H -1); // after j
											if(s >=0){
												//base Xvs - Xui
												lp->num_constraints +=1;
												glp_add_rows(lp->mip, 1); // add new rows for a new costraint
												glp_set_row_name(lp->mip, lp->num_constraints, "c1");
												glp_set_row_bnds(lp->mip, lp->num_constraints, GLP_LO, 0.0, 0.0);

												// Xvs = 1(coeficiente)
												index_colum_G_H = INDEX_CONSTRAINT_G_H(v,s,op->num_vert_G, op->num_vert_H); 
												add_constraint(lp, lp->num_constraints, index_colum_G_H,1);

												// Xui = -1(coeficiente)
												index_colum_G_H = INDEX_CONSTRAINT_G_H(u,i,op->num_vert_G, op->num_vert_H); 
												add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);
												// Sum then
												for(t=0;t<j;t++){
													if(op->graph_h[i][t] == 1){
														// Xvt = 1(coeficiente)
														index_colum_G_H = INDEX_CONSTRAINT_G_H(v,t,op->num_vert_G, op->num_vert_H); 
														if(index_colum_G_H != lp->ja[lp->num_nonzero_constraints]) 
															add_constraint(lp, lp->num_constraints, index_colum_G_H,1);
														if(t < op->num_vert_H -1){ // if not last one
															// Xvt+1 = -1(coeficiente)
															index_colum_G_H = INDEX_CONSTRAINT_G_H(v,t+1,op->num_vert_G, op->num_vert_H); 
															if(index_colum_G_H != lp->ja[lp->num_nonzero_constraints]) 
																add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);
														}
													}
												}

											}else{
												lp->num_constraints +=1;
												glp_add_rows(lp->mip, 1); // add new rows for a new costraint
												glp_set_row_name(lp->mip, lp->num_constraints, "c1");
												glp_set_row_bnds(lp->mip, lp->num_constraints, GLP_LO, 0.0, 0.0);

												if(i < op->num_vert_H -1){
													// Xui+1 = 1(coeficiente)
													index_colum_G_H = INDEX_CONSTRAINT_G_H(u,i+1,op->num_vert_G, op->num_vert_H); 
													add_constraint(lp, lp->num_constraints, index_colum_G_H,1);
												}
												// Xui = -1(coeficiente)
												index_colum_G_H = INDEX_CONSTRAINT_G_H(u,i,op->num_vert_G, op->num_vert_H); 
												add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);
												// Sum then
												for(t=0;t<op->num_vert_H;t++){
													if(op->graph_h[i][t] == 1){
														// Xvt = 1(coeficiente)
														index_colum_G_H = INDEX_CONSTRAINT_G_H(v,t,op->num_vert_G, op->num_vert_H); 

														if(index_colum_G_H != lp->ja[lp->num_nonzero_constraints ])
															add_constraint(lp, lp->num_constraints, index_colum_G_H,1);

														if(t < op->num_vert_H -1){ // if not last one
															// Xvt+1 = -1(coeficiente)
															index_colum_G_H = INDEX_CONSTRAINT_G_H(v,t+1,op->num_vert_G, op->num_vert_H);
															if(index_colum_G_H != lp->ja[lp->num_nonzero_constraints]) 
																add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);
														}

													}
												}
											}

											//second base constrain -- For in-neighbors
                    	//first in neighbor of j, after i  
											s = get_first_in_neighbor(op,j,i+1,op->num_vert_H -1); // after i
											if(s >=0){
												//base Xvs - Xui
												lp->num_constraints +=1;
												glp_add_rows(lp->mip, 1); // add new rows for a new costraint
												glp_set_row_name(lp->mip, lp->num_constraints, "c1");
												glp_set_row_bnds(lp->mip, lp->num_constraints, GLP_LO, 0.0, 0.0);

												// Xus = 1(coeficiente)
												index_colum_G_H = INDEX_CONSTRAINT_G_H(u,s,op->num_vert_G, op->num_vert_H); 
												add_constraint(lp, lp->num_constraints, index_colum_G_H,1);

												// Xvj = -1(coeficiente)
												index_colum_G_H = INDEX_CONSTRAINT_G_H(v,j,op->num_vert_G, op->num_vert_H); 
												add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);

												// Sum then
												for(t=0;t<i;t++){
													if(op->graph_h[t][j] == 1){
														// Xut = 1(coeficiente)
														index_colum_G_H = INDEX_CONSTRAINT_G_H(u,t,op->num_vert_G, op->num_vert_H); 
														if(index_colum_G_H != lp->ja[lp->num_nonzero_constraints]) 
															add_constraint(lp, lp->num_constraints, index_colum_G_H,1);
														if(t < op->num_vert_H -1){ // if not last one
															// Xut+1 = -1(coeficiente)
															index_colum_G_H = INDEX_CONSTRAINT_G_H(u,t+1,op->num_vert_G, op->num_vert_H); 
															if(index_colum_G_H != lp->ja[lp->num_nonzero_constraints]) 
																add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);
														}
													}
												}

											}else{
												lp->num_constraints +=1;
												glp_add_rows(lp->mip, 1); // add new rows for a new costraint
												glp_set_row_name(lp->mip, lp->num_constraints, "c1");
												glp_set_row_bnds(lp->mip, lp->num_constraints, GLP_LO, 0.0, 0.0);

												if(j < op->num_vert_H -1){
													// Xvj+1 = 1(coeficiente)
													index_colum_G_H = INDEX_CONSTRAINT_G_H(v,j+1,op->num_vert_G, op->num_vert_H); 
													add_constraint(lp, lp->num_constraints, index_colum_G_H,1);
												}
												// Xvj = -1(coeficiente)
												index_colum_G_H = INDEX_CONSTRAINT_G_H(v,j,op->num_vert_G, op->num_vert_H); 
												add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);

												// Sum then
												for(t=0;t<i;t++){
													if(op->graph_h[i][t] == 1){
														// Xvt = 1(coeficiente)
														index_colum_G_H = INDEX_CONSTRAINT_G_H(u,t,op->num_vert_G, op->num_vert_H); 
														if(index_colum_G_H != lp->ja[lp->num_nonzero_constraints]) 
															add_constraint(lp, lp->num_constraints, index_colum_G_H,1);
														if(t < op->num_vert_H -1){ // if not last one
															// Xvt+1 = -1(coeficiente)
															index_colum_G_H = INDEX_CONSTRAINT_G_H(u,t+1,op->num_vert_G, op->num_vert_H); 
															if(index_colum_G_H != lp->ja[lp->num_nonzero_constraints]) 
																add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}