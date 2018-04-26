# Bi-Arc Digraphs and Approximation of H-coloring

We study the class of digraphs analogous to complement of circular arc bigraphs. They are defined via a geometric representation by two families of arcs from a circle satisfying  the  condition  of  complement  of  circular  arc  graphs  with  clique  cover  two (circular arc bigraphs); hence we call them
Bi-arc digraph. </br>
This digraphs introduced recently by Hell and Rafiey.  They admit a number of equivalent definitions, including an ordering characterization by so-called min-orderings and the existence of semi-lattice polymorphisms.  Min-orderings arose in the study of list  homomorphism  problems. If H admits  a  min-ordering  (or  a  certain  extension
of  min-orderings),  then  the  list  homomorphism  problem  to H is  known  to  admit  a polynomial time algorithm.The bi-arc digraphs contain the class of monotone proper- interval bigraphs the digraphs that admit a min-max ordering.If a digraph H admits a min-max (and extended min-max) ordering then minimum cost homomorphism to H, MinHOM(H), is polynomial time solvable and NP-complete otherwise.</br>
We show another importance of this class of digraphs by designing a constant ap- proximation algorithm for minimum cost homomorphism.  When H is a target digraphs and admits a min-ordering then MinHOM(H), can be approximated within a constant factor.

## How to setup and run the code

1) Wanna change paramters of LP manualy, such as, optimal or continuos solution?
	* Go to folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/src) and open file main.c
		* Inside of main, you can change any parameter of the data structe LP_user_params you like, to the options given.
		* To check all available paramters and options, go to folder include (Approximation_Min_Cost_Homomorphism/C_Python_Version/C_Python_Version/include) and open file graph.h
2) Passing basic paramters by Argv
	* The 2 basics parameters(pair_consistency and type_solution) you can pass by argv, after the first 6(must have) paramters.

3) Compile the program
	* Inside of folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/C_Python_Version/src) run the following, at terminal:
		* make
4) Want to create new graphs(G &/or H) ? 
	* Inside of folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/C_Python_Version/src) run the following, at terminal:
		* For a graph G: python3 create_bip_graph.py <num_vertices>, <num_of_partition(>=2)> & <graph_g>
		* For a graph H: python3 create_bip_graph_h.py <num_vertices>, <num_of_partition(>=2)> & <graph_h>

5) Have you created or added a new graph(G &/or H)? If yes, do the following:
	* Inside of folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/C_Python_Version/src) run the following, at terminal:
		* make data
		* make list

6) Run the program
	* Inside of folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/C_Python_Version/src) run the following, at terminal:
		* make run 

## Contributors

Dr. Arash Rafiey

M.Sc. Thiago Santos

