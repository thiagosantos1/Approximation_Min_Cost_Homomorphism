#include <graph.h> 

void initi_LP(GRAPH *op, LP_MIN_COST * lp);
void set_obj_function(GRAPH *op, LP_MIN_COST * lp);
//for every vertex in G Constraint Xui - Xui+1 >=0
void add_constr_XUi_XUiplus1(GRAPH *op, LP_MIN_COST * lp);

void construct_LP(GRAPH *op, LP_MIN_COST * lp)
{
	initi_LP(op,lp);
	set_obj_function(op,lp);
	add_constr_XUi_XUiplus1(op,lp);

}

void initi_LP(GRAPH *op, LP_MIN_COST * lp)
{
	int i;
	lp->index_saving_matrix = malloc(op->num_vert_G * sizeof(int *));
	for(i=0; i<op->num_vert_G; i++){
  	lp->index_saving_matrix[i] = malloc(op->num_vert_H * sizeof(int));
		memset(lp->index_saving_matrix[i],0,op->num_vert_H);
	}

	lp->mip = glp_create_prob();
	glp_set_prob_name(lp->mip, "Min Cost Homomophism");
	glp_set_obj_dir(lp->mip, GLP_MIN);

	lp->allocation_size = 1000;

	lp->variable_type = GLP_CV;
} 

void set_obj_function(GRAPH *op, LP_MIN_COST * lp)
{
	int i,j,start =-1,save_index=1;
	double coef;

	//for every vertex in G 
	for(i=0; i<op->num_vert_G; i++){
		start =-1;
		// check for it's possible assigns in H
		for(j=0; j<op->num_vert_H; j++){
			coef = op->cost_g_h_matrix[i][j];
			if(coef!=0){

				if(start <=0){
					glp_add_cols(lp->mip, 1);
				  glp_set_col_name(lp->mip, save_index, "x");
				  glp_set_col_bnds(lp->mip, save_index, GLP_LO, 0.0, 0.0);
					glp_set_obj_coef(lp->mip, save_index, coef);
					glp_set_col_kind(lp->mip, save_index, lp->variable_type); // set variable to continuos
					lp->index_saving_matrix[i][j] = save_index;
					start = 1;
				}else{
					coef -= op->cost_g_h_matrix[i][j-1];
					glp_add_cols(lp->mip, 1);
				  glp_set_col_name(lp->mip, save_index, "x");
				  glp_set_col_bnds(lp->mip, save_index, GLP_LO, 0.0, 0.0);
					glp_set_obj_coef(lp->mip, save_index, coef);
					glp_set_col_kind(lp->mip, save_index, lp->variable_type); // set variable to continuos
					lp->index_saving_matrix[i][j] = save_index;
				}
				save_index++;
			}
		}
	}

 //  for (int i = 0; i < op->num_vert_G; i++){
	// 	for (int j = 0; j < op->num_vert_H; j++){
	// 		printf("%d ", lp->index_saving_matrix[i][j] );
	// 	}
	// 	puts("");
	// }
}

void add_constr_XUi_XUiplus1(GRAPH *op, LP_MIN_COST * lp)
{
	
}

