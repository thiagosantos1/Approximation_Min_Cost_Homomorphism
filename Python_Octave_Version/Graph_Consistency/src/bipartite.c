// File disigned to check if a graph is bipartite or not
// Also, to check if graph is connected or not

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h> 

#define CHUNK  		1
#define X		255 // Max size of a line in a file
#define NO_COLOR 	-1
// used to assigned one color to each neighbor. If at any time cannot color, it means it's not bipartite
#define Color_1 	1
#define Color_2		2

// define a struct to be use as adjacency list
typedef struct
{
  int degre; // to now how many neighors this vertex has / size of the linked list
  int size;   // how many itens allocated
  int * nbrs; // represents which vertex are its neighbors
  int color; // save how many colors is available for that vertex
} ADJ_LIST;


ADJ_LIST * adj_list;
short N;        // number of vertexes // dinymic allocated, based on the name of the program

void init_vertex(int num_vertices); // initialize N, that represents how many vertex in the graph
void init_adjList(); // initialize the adj list array
void update_alist(int u, int v);
void display_graph();
int get_vertex_not_colored(); // return a vertex that did not get colored after dfs
int bad_neighbors(int vertex); // return 1 if any neighbor of vertex got the same color as him
//color vertex with color 1 or 2, oposite of the previous call(parent). Then checks if any
//					  neighbor got same color. If so, return false 
int dfs_bipartite_check(int vertex, int color); 
int main(int argc, char const *argv[])
{
  if(argc<2){
    fprintf(stderr, "\nFile name <%s usage> file.nums\n", argv[0]);
    return 0;
  }
  
  const char * filename = argv[1];
  FILE * fp = fopen(filename, "r");
  if(fp == NULL){
    fprintf(stderr, "\nError opening the file\n");
    exit(0);
  }
  int class1, class2;
  int num_vertices =0;
  fscanf(fp,"%d", &num_vertices); 
  init_vertex(num_vertices); // initialize how many vertex there are in the graph, based on the name of the file
  init_adjList();
  
  while( fscanf(fp,"%d %d", &class1, &class2) == 2){
    // update adj list for each arc
    update_alist(class1,class2);
    update_alist(class2,class1);
    
  }
  
  int vertex =0, compontent=1, result = 0, vertex_not_colered = -1;
  
  
  for(;;){ // make sure it runs until cover all edges
    result = dfs_bipartite_check(vertex, Color_1);
    if(result <=0){ // you are then sure it's not bipartite
      printf("Graph is not bipartite\n");
      break;
    }
    
    else{ // it may be a disconected graph. Then, it may be a false positive
      
      // then, check if everybody got a color. If so, you are sure it's bipartite. If not, have to run again the dfs
      // from that node withot a color
      vertex_not_colered = get_vertex_not_colored();
      if(vertex_not_colered >=0){
	printf("Graph is not connected, running dfs again for compontent %d and vertex %d.....\n",++compontent, vertex_not_colered);
	vertex = vertex_not_colered;
      }
      else{ // you are then sure that everybody got colored and it's bipartite
	printf("Graph is bipartite\n");
	break;
      }
      
    }
  }
  //display_graph();
  return 0;
}


void init_vertex(int num_vertices)
{
  N = num_vertices;
}

void init_adjList()
{
  adj_list = malloc(N* sizeof(ADJ_LIST));
  
  for(int i=0;i<N;i++){
    adj_list[i].degre  = 0;
    adj_list[i].size = 0;
    adj_list[i].nbrs = 0;  // null pointer
    adj_list[i].color = NO_COLOR; 
  }
  
}

void update_alist(int u, int v)
{
  int tmp;
  
  if(adj_list[u].degre >= adj_list[u].size){
    if(adj_list[u].size == 0){
      adj_list[u].size = CHUNK;
      adj_list[u].nbrs = malloc(CHUNK * sizeof(int));
    }else{
      adj_list[u].size += CHUNK;
      tmp = adj_list[u].size;
      adj_list[u].nbrs = realloc(adj_list[u].nbrs, tmp * sizeof(int));
    }
  }
  adj_list[u].nbrs[adj_list[u].degre++] = v;
}

void display_graph()
{
  
  for(int i=0; i<N; i++){
    printf("Vertex %d Color %d\nNeighbors: ",i, adj_list[i].color );
    for(int j=0; j<adj_list[i].size; j++){
      printf("Nbr %d Color %d ", adj_list[i].nbrs[j], adj_list[adj_list[i].nbrs[j]].color);
    }
    printf("\n\n");
  }
}

int dfs_bipartite_check(int vertex, int color)
{
  int vert_ngbr, flag=1,i;
  
  adj_list[vertex].color = color;
  color = color==Color_1 ? Color_2:Color_1;
  for(i=0; i<adj_list[vertex].size; i++){
    vert_ngbr = adj_list[vertex].nbrs[i];
    if(adj_list[vert_ngbr].color == NO_COLOR){
      flag = dfs_bipartite_check(vert_ngbr,color);
      if(flag<=0)
	break;
    } else if(adj_list[vert_ngbr].color == adj_list[vertex].color){
    
      return 0;
    }
  }
  
  return flag;
}

int bad_neighbors(int vertex)
{
  for(int i=0; i< adj_list[vertex].size; i++){
    if(adj_list[vertex].color == adj_list[adj_list[vertex].nbrs[i]].color )
      return 1;
  }
  
  return -1; // there is not bad neighbrs
}

int get_vertex_not_colored()
{
  for(int i=0; i< N; i++){
    if(adj_list[i].color == NO_COLOR)
      return i;
  }
  
  return -1;
  
}

