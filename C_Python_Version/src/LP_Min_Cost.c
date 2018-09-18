#include <graph.h> 
#include <LP_Min_Cost.h>

//#define PRINT_RESULTS

void defult_configurations(LP_user_params * lp_param)
{
	lp_param->type_solution = 	1; 
	lp_param->pair_consistency = 1;

}

void set_LP_configuration(GRAPH *op, LP_MIN_COST * lp, LP_user_params * lp_param,USER_PARAMS * ip)
{
	if(lp_param->type_solution == 2)
		lp->variable_type = GLP_IV;
	else
		lp->variable_type = GLP_CV;

	if(lp_param->pair_consistency == 1)
		pair_consistency(op, ip);
}

void LP_SOLVER(GRAPH *op, LP_MIN_COST * lp, LP_user_params * lp_param,USER_PARAMS * ip)
{
	construct_LP(op,lp,lp_param);

	glp_load_matrix(lp->mip, lp->num_nonzero_constraints, lp->ia, lp->ja, lp->ar);

	glp_iocp parm;
	glp_init_iocp(&parm);
	parm.presolve = GLP_ON;
	glp_intopt(lp->mip, &parm);

	#ifdef PRINT_RESULTS 
		print_results(op,lp);
	#else
		save_to_file(op,lp,ip,lp_param);
	#endif

	glp_delete_prob(lp->mip);
}

void construct_LP(GRAPH *op, LP_MIN_COST * lp, LP_user_params * lp_param)
{
	initi_LP(op,lp);
	set_obj_function(op,lp);
	constr_XUi_XUiplus1(op,lp);
	constr_list_Xui_Xuiplus1(op,lp);
	constr_in_neighbor(op,lp);
	constr_out_neighbor(op,lp);
	constr_missing_edges(op,lp);

	if(lp_param->pair_consistency ==1) // if pairs_constraint is set to true
		constr_pairs_list(op,lp);

}

void initi_LP(GRAPH *op, LP_MIN_COST * lp)
{
	lp->mip = glp_create_prob();
	glp_set_prob_name(lp->mip, "Min Cost Homomophism");
	glp_set_obj_dir(lp->mip, GLP_MIN);

	lp->allocation_size = 1000;
	lp->memory_left			= 	 0;
	lp->num_constraints =    0;

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

void constr_pairs_list(GRAPH *op, LP_MIN_COST * lp)
{
	int g_u, g_v, h_i, h_i_, i_is_in_gu =-1, h_j, h_j_;
	int key=0,pair,byte_position=0,bit_position=0,index_colum_G_H=0;

	//for every vertex in G 
	for(g_u=0; g_u<op->num_vert_G; g_u++){

		// check for it's possible assigns in H
		for(h_i=0; h_i<op->num_vert_H; h_i++){
			h_i_ = -1;
			if(h_i < op->num_vert_H -1) // if it's not the last one, then set the next one
				h_i_ = h_i +1;

			i_is_in_gu = op->list_homom_matrix[g_u][h_i] == 1 ? 1:-1;

			//check if i is in the list of U first. Then, add the constraint
			if (i_is_in_gu == 1){ 

				// add new constraint
				lp->num_constraints +=1;
				glp_add_rows(lp->mip, 1); // add new rows for a new costraint
				glp_set_row_name(lp->mip, lp->num_constraints, "c1");
				glp_set_row_bnds(lp->mip, lp->num_constraints, GLP_LO, 0.0, 0.0);

				// for u # for last one, just add -1
				if(h_i < op->num_vert_H -1){ // not last one
					// Xui = -1(coeficiente)
					index_colum_G_H = INDEX_CONSTRAINT_G_H(g_u,h_i,op->num_vert_G, op->num_vert_H); 
					add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);

					// Xui +1 = 1(coeficiente)
					index_colum_G_H = INDEX_CONSTRAINT_G_H(g_u,h_i_,op->num_vert_G, op->num_vert_H); 
					add_constraint(lp, lp->num_constraints, index_colum_G_H, 1);

				}else{ // last one
					// Xui = -1(coeficiente)
					index_colum_G_H = INDEX_CONSTRAINT_G_H(g_u,h_i,op->num_vert_G, op->num_vert_H); 
					add_constraint(lp, lp->num_constraints, index_colum_G_H,-1);
				}

				// 									SUM PART
				// now, for all ui & ui+1, construct the constraint for all vj & vj+1
				//Sum(Xvj - Xv,j+1) - Xui + Xu,i+1 >=0
        //get if (h_i,h_j) is in the list(g_u,g_v)
				for(g_v=0; g_v<op->num_vert_G; g_v++){
					if(g_u != g_v){ // if they are not the same

						// new line of constraint for every ui,u+1 v # Sum part
						h_j_ = -1;
						for(h_j=0; h_j<op->num_vert_H; h_j++){
							if(h_j < op->num_vert_H -1) 
								h_j_ = h_j +1;

							// Sum(Xvj - Xv,j+1) - Xui + Xu,i+1 >=0
              // get if (h_i,h_j) is in the list(g_u,g_v)
              KEY_HASH(key,g_u,g_v,op->num_vert_G);
              if( op->pairs_matrix[key][op->num_bytes ] <=0 ){
              	fprintf(stderr, "Error pairs list. key %d doesn't have any pair", key);
              	exit(1);
              }

              // get the key that represents the pair we are looking for it
              KEY_HASH(pair,h_i,h_j,op->num_vert_H);
              // get the byte and bit of the pair
							GET_BYTE_POSITION(byte_position,op->num_bytes ,pair);
				  		GET_BIT_POSITION(bit_position,op->num_bytes ,pair,byte_position);

				  		// if pair is in the list of key (if its bit is set)
				  		if( (READ_BIT(op->pairs_matrix[key][byte_position],bit_position)) >0){

				  			// Xvj = 1(coeficiente)
								index_colum_G_H = INDEX_CONSTRAINT_G_H(g_v,h_j,op->num_vert_G, op->num_vert_H); 
								if(index_colum_G_H != lp->ja[lp->num_nonzero_constraints ])
									add_constraint(lp, lp->num_constraints, index_colum_G_H, 1);

								if(h_j < op->num_vert_H -1){ // if there is a next one
									// Xvj_ = 1(coeficiente)
									index_colum_G_H = INDEX_CONSTRAINT_G_H(g_v,h_j_,op->num_vert_G, op->num_vert_H);
									if(index_colum_G_H != lp->ja[lp->num_nonzero_constraints ]) 
										add_constraint(lp, lp->num_constraints, index_colum_G_H, -1);
								}
				  		}
						}
					}

				}
			}

		}
	}
}

void print_results(GRAPH *op, LP_MIN_COST * lp)
{
	double total_cost = glp_mip_obj_val(lp->mip);
	fprintf(stderr,"Total Cost of Cut %.2f\n",total_cost );
	int count=0,i=0,g;
	fprintf(stderr,"     ");
	for(i=i; i<op->num_vert_H;i++)
		fprintf(stderr,"H %d  ",i);
	i=0;
	fprintf(stderr,"\nG %d: ",i );
	for(i=i; i<op->num_vert_G * op->num_vert_H;i++){
		fprintf(stderr,"%.02f ", glp_mip_col_val(lp->mip, i+1)); 
		count++;
		if(count == op->num_vert_H){
			fprintf(stderr,"\n");
			g = (i/op->num_vert_H)+1;
			if(g < op->num_vert_G)
				fprintf(stderr,"G %d: ",g); 
			count =0;
		}
	}
}

void save_to_file(GRAPH *op, LP_MIN_COST * lp,USER_PARAMS * ip, LP_user_params * lp_param)
{
	FILE * fp;
	char folder_path[op->num_vert_G *2];
	char str_g[op->num_vert_G];
	char str_h[op->num_vert_H];

	strcpy(folder_path,"../Tests/G_");

	sprintf(str_g, "%d", op->num_vert_G);
	sprintf(str_h, "%d", op->num_vert_H);

	strcat(folder_path,str_g);
	strcat(folder_path,"_H");
	strcat(folder_path,str_h);

	// create new folder to save to tests, if needed
	struct stat st = {0};
	if (stat(folder_path, &st) == -1) {
    mkdir(folder_path, 0700);
	}

	strcat(folder_path,"/");
	strcat(folder_path,ip->results_out_put);
	fp = fopen (folder_path,"a"); // open to append

	double total_cost = glp_mip_obj_val(lp->mip);

	if(lp->variable_type == GLP_CV){
		if(total_cost >0)
			fprintf (fp,"Size of G: %d | G is Bipartite: %s | Num Partitions in G: %d | Size of H: %d | H is Bipartite: %s | Num Partitions in H: %d | Pair Consistency: %s | Total Cost: %0.2f | Continuos Solution\n", 
		      		op->num_vert_G,op->isBipartite_G ==1 ? "Yes":"No ",op->numPartitions_G, op->num_vert_H,op->isBipartite_H ==1 ? "Yes":"No ",op->numPartitions_H, lp_param->pair_consistency ==1 ? "Used    ":"Not used" , total_cost);
		else
			fprintf (fp,"Size of G: %d | G is Bipartite: %s | Num Partitions in G: %d | Size of H: %d | H is Bipartite: %s | Num Partitions in H: %d | Pair Consistency: %s | SOLUTION NOT FOUND | Continuos Solution\n", 
		      		op->num_vert_G,op->isBipartite_G ==1 ? "Yes":"No ",op->numPartitions_G, op->num_vert_H,op->isBipartite_H ==1 ? "Yes":"No ",op->numPartitions_H, lp_param->pair_consistency ==1 ? "Used    ":"Not used");
	}else{
		if(total_cost > 0)
			fprintf (fp,"Size of G: %d | G is Bipartite: %s | Num Partitions in G: %d | Size of H: %d | H is Bipartite: %s | Num Partitions in H: %d | Pair Consistency: %s | Total Cost: %0.2f | Integral Solution\n", 
		      		op->num_vert_G,op->isBipartite_G ==1 ? "Yes":"No ",op->numPartitions_G, op->num_vert_H,op->isBipartite_H ==1 ? "Yes":"No ",op->numPartitions_H, lp_param->pair_consistency ==1 ? "Used    ":"Not used" , total_cost);
		else
			fprintf (fp,"Size of G: %d | G is Bipartite: %s | Num Partitions in G: %d | Size of H: %d | H is Bipartite: %s | Num Partitions in H: %d | Pair Consistency: %s | SOLUTION NOT FOUND | Integral Solution\n", 
		      		op->num_vert_G,op->isBipartite_G ==1 ? "Yes":"No ",op->numPartitions_G, op->num_vert_H,op->isBipartite_H ==1 ? "Yes":"No ",op->numPartitions_H, lp_param->pair_consistency ==1 ? "Used    ":"Not used");
	}
 
  /* close the file*/  
  fclose (fp);
}



