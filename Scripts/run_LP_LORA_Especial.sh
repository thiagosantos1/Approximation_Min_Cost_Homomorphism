#!/bin/sh -e
# This script run the especial H graph

# Run python first to create the list and pairs. This save a nem file in the LP_LORA folder
python3 ../check_graph_homomorphism/bin/graph_consistency.py ../check_graph_homomorphism/bin/graph_g.txt ../check_graph_homomorphism/bin/graph_h.txt ../check_graph_homomorphism/bin/list.txt

# Then run the octave to get the cut.
octave LP_LORA_Especial_H.m 

#END


