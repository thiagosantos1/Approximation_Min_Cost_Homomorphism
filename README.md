# Bi-Arc Digraphs and Approximation of H-coloring

We study the class of digraphs analogous to complement of circular arc bigraphs. They are defined via a geometric representation by two families of arcs from a circle satisfying  the  condition  of  complement  of  circular  arc  graphs  with  clique  cover  two (circular arc bigraphs); hence we call them
Bi-arc digraph. </br>
This digraphs introduced recently by Dr.Rafiey. They admit a number of equivalent definitions, including an ordering characterization by so-called min-orderings and the existence of semi-lattice polymorphisms.  Min-orderings arose in the study of list  homomorphism  problems. If H admits  a  min-ordering  (or  a  certain  extension
of  min-orderings),  then  the  list  homomorphism  problem  to H is  known  to  admit  a polynomial time algorithm.The bi-arc digraphs contain the class of monotone proper- interval bigraphs the digraphs that admit a min-max ordering.If a digraph H admits a min-max (and extended min-max) ordering then minimum cost homomorphism to H, MinHOM(H), is polynomial time solvable and NP-complete otherwise.</br>
We show another importance of this class of digraphs by designing a constant ap- proximation algorithm for minimum cost homomorphism.  When H is a target digraphs and admits a min-ordering then MinHOM(H), can be approximated within a constant factor.

## Results
</br></br> Examples of output :
  <table border=1>
     <tr align='center' > 
        <td><img src="https://github.com/thiagosantos1/Approximation_Min_Cost_Homomorphism/blob/master/C_Python_Version/Tests/plots/results_1.png" width="450"                  title="hover text"></td>         
       <td><img src="https://github.com/thiagosantos1/Approximation_Min_Cost_Homomorphism/blob/master/C_Python_Version/Tests/plots/results_1.png" width="350" title="hover        text"></td>
     </tr>
    <tr align='center' > 
        <td><img src="https://github.com/thiagosantos1/Approximation_Min_Cost_Homomorphism/blob/master/C_Python_Version/Tests/plots/results_1.png" width="350"                title="hover text"></td>         
       <td><img src="https://github.com/thiagosantos1/Approximation_Min_Cost_Homomorphism/blob/master/C_Python_Version/Tests/plots/results_1.png" width="350" title="hover        text"></td>
     </tr>
  </table>

## How to setup and run the code 

1) Wanna change paramters of LP manualy, such as, optimal or continuos solution?
	* Go to folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/src) and open file main.c
		* Inside of main, you can change any parameter of the data structe LP_user_params you like, to the options given.
		* To check all available paramters and options, go to folder include (Approximation_Min_Cost_Homomorphism/C_Python_Version/C_Python_Version/include) and open file graph.h
2) Passing basic paramters by Argv
	* The 2 basics parameters(pair_consistency and type_solution) you can pass by argv, after the first 6(must have) paramters.

3) Compile the program
	* Inside of folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/src) run the following, at terminal:
		* make
4) Want to create new graphs(G &/or H) ? 
	* Inside of folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/src) run the following, at terminal:
		* For a graph G: python3 create_bip_graph.py <num_vertices>, <num_of_partition(>=2)> & <graph_g>
		* For a graph H: python3 create_bip_graph_h.py <num_vertices>, <num_of_partition(>=2)> & <graph_h>

5) Have you created or added a new graph(G &/or H)? If yes, do the following:
	* Inside of folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/src) run the following, at terminal:
		* make data
		* make list

6) Run the program
	* Inside of folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/src) run the following, at terminal:
		* make run (It runs with default configuration, unless you have changed, in step 1) 
7) Run the program for multiple instances of H and G(Both random created), with a script
	* Inside of folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/src) run the following, at terminal:
		* ./run_loop_script.sh <pair_consistency_flag> <num_vertices G>, <num_vertices H>, <num_of_partitions>, <is_bipartite>, <num_of_graphs_H> & <Loop repetitions for each H(N)>
		* How the script works?
			* For each random H created, we creante N different graphs G, and run the LP twice for each G. Once for Continuos & once for Integral.
		* Paramters 
			* pair_consistency_flag --> 1 to run program with pair consistency and 2 without it
			* num_vertices G --> How many vertices you wanna in G
			* num_vertices H --> How many vertices you wanna in H
			* num_of_partitions --> How many partitions the graph has
			* is_bipartite --> 1 to bipartite and 2 for not.
			* num_of_graphs_H --> How many graphs H you want to create. 
			* Loop repetitions for each H(N) --> For each graph H created, how many G you want to create ?

8) Run the program for multiple instances of G, from a fixed graph H, with a script
	* Inside of folder src (Approximation_Min_Cost_Homomorphism/C_Python_Version/src) run the following, at terminal:
		* ./run_loop_fixed_H_script.sh <pair_consistency_flag> <num_vertices G>, <num_of_partitions>, <is_bipartite> & <num_of_graphs_G> 
		* How the script works?
			* For a fixed graph h(located in etc/graph_h.txt), we creante N different graphs G, and run the LP twice for each G. Once for Continuos & once for Integral.
		* Paramters 
			* pair_consistency_flag --> 1 to run program with pair consistency and 2 without it
			* num_vertices G --> How many vertices you wanna in G
			* num_of_partitions --> How many partitions the graph has
			* is_bipartite --> 1 to bipartite and 2 for not(If more than one partition, we check if each partition n & n+1 is bipartite).
			* num_of_graphs_G --> How many graphs G you want to create. 
			


## Contributors

Dr. Arash Rafiey

M.Sc. Thiago Santos

