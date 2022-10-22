#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct Node {
    int frequency;
    struct Arc *arcs;
    short arcs_count;
} Node;

typedef struct Arc {
    struct Node node;
    short symbol;
} Arc;

Node fst_create(uint16_t *block, uint64_t block_size, int depth) {
    // Initialize root FST
    Node *root = malloc(sizeof(Node));
    root->frequency = 0;
    root->arcs = NULL;
    root->arcs_count = 0;

    // Iterate through block with block_size and depth size
    int current_largest_count = 0;
    int *current_largest_substring = malloc(depth * sizeof(int));
    int *current_substring = malloc(depth * sizeof(int));
    for (uint64_t i = 0; i < block_size; i++) {
        if (i % 500000 == 0) {
            printf("%ld / %ld\n", i, block_size);
        }
        Node *node_current = root;

        for (int j = 0; j < depth; j++) {
            // Find the arc with the current symbol
            int arc_found = 0;
            Arc arc;
            for (int k = 0; k < node_current->arcs_count; k++) {
                arc = node_current->arcs[k];

                if (arc.symbol == block[i + j]) {
                    arc_found = 1;
                    break;
                }
            }

            if (arc_found == 1) {
                // If we found the arc with the symbol, increment the next nodes frequency, add set it to node_current
                node_current = &arc.node;
                node_current->frequency++;
                
            } else {
                // If no arc was found with symbol, add a new arc for that symbol with a new node

                // New node
                Node *node_new = malloc(sizeof(Node));
                node_new->frequency = 1;
                node_new->arcs = NULL;
                node_new->arcs_count = 0;

                // Init new arc
                Arc *arc_new = malloc(sizeof(Arc));
                arc_new->symbol = block[i + j];
                arc_new->node = *node_new;

                // Add arc to current node (copy arc array, and increment arcs_count)
                if (node_current->arcs_count == 0) {
                    // Zero number of arcs
                    node_current->arcs_count = 1;
                    node_current->arcs = malloc(sizeof(Arc));
                    node_current->arcs[0] = *arc_new;

                } else {
                    // Add new arc to the array of arcs (copy array)
                    Arc *arcs_new = malloc((node_current->arcs_count + 1) * sizeof(Arc)); 
                    for (int k = 0; k < node_current->arcs_count; k++) {
                        arcs_new[k] = node_current->arcs[k];
                    }
                    arcs_new[node_current->arcs_count] = *arc_new;
                
                    // Free old arc array
                    free(node_current->arcs);
                    node_current->arcs = arcs_new;

                    node_current->arcs_count++;
                }

                // Set current node to the new node
                node_current = node_new;

            }
        }
    }
}