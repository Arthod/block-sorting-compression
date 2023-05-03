#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct FSTNode {
    uint32_t frequency;
    struct FSTArc *arcs;
    uint16_t arcs_count;
} FSTNode;

typedef struct FSTArc {
    struct FSTNode node;
    uint16_t symbol;
} FSTArc;

FSTNode fst_create(uint16_t *block, uint64_t block_size, int depth) {
    // Initialize root FST
    FSTNode *root = malloc(sizeof(FSTNode));
    root->frequency = 0;
    root->arcs = NULL;
    root->arcs_count = 0;

    // Largest substring
    int substring_largest_frequency = 0;
    int *substring_largest = malloc(depth * sizeof(int));
    int *substring_current = malloc(depth * sizeof(int));

    // Iterate through block with block_size and depth size
    for (uint64_t i = 0; i < block_size; i++) {
        //printf("Current char %c (%ld/%ld)\n", block[i], i, block_size);
        printf("Returned to root\n");
        FSTNode *node_current = root;

        for (int j = 0; j < depth; j++) {
            substring_current[j] = block[i + j];

            // Find the arc with the current symbol
            int arc_found = 0;
            FSTArc *arc;
            for (int k = 0; k < node_current->arcs_count; k++) {
                arc = &node_current->arcs[k];

                if (arc->symbol == block[i + j]) {
                    // If we found the arc with the symbol, increment the next nodes frequency, add set it to node_current
                    arc_found = 1;
                    node_current = &arc->node;
                    arc->node.frequency++;
                    printf("Existing arc with symbol %c, new frequency is: %d\n", block[i + j], node_current->frequency);
                    break;
                }
            }

            if (arc_found == 0) {
                // If no arc was found with symbol, add a new arc for that symbol with a new node
                printf("Adding arc with symbol %c\n", block[i + j]);

                // New node
                FSTNode *node_new = malloc(sizeof(FSTNode));
                node_new->frequency = 1;
                node_new->arcs = NULL;
                node_new->arcs_count = 0;

                // Init new arc
                FSTArc *arc_new = malloc(sizeof(FSTArc));
                arc_new->symbol = block[i + j];
                arc_new->node = *node_new;

                // Add arc to current node (copy arc array, and increment arcs_count)
                if (node_current->arcs_count == 0) {
                    // Zero number of arcs
                    node_current->arcs_count = 1;
                    node_current->arcs = malloc(sizeof(FSTArc));
                    node_current->arcs[0] = *arc_new;

                } else {
                    // Add new arc to the array of arcs (copy array)
                    FSTArc *arcs_new = malloc((node_current->arcs_count + 1) * sizeof(FSTArc)); 
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
        // Print longest substring with frequency
        /*printf("Just saw substring '");
        for (int i = 0; i < depth; i++) {
            printf("%c", substring_current[i]);
        }
        printf("'. Which has frequency: %d\n", node_current->frequency);*/


        // See if node current has larger frequency than current largest
        if (node_current->frequency > substring_largest_frequency) {
            for (int j = 0; j < depth; j++) {
                substring_largest[j] = substring_current[j];
            }
            substring_largest_frequency = node_current->frequency;

        
            // Print longest substring with frequency
            printf("Current largest substring of length: %d, has frequency: %d, and is '", depth, substring_largest_frequency);
            for (int i = 0; i < depth; i++) {
                printf("%c", substring_largest[i]);
            }
            printf("'.\n");
        }
    }

    // Print longest substring with frequency
    printf("Largest substring of length: %d, has frequency: %d, and is '", depth, substring_largest_frequency);
    for (int i = 0; i < depth; i++) {
        printf("%c", substring_largest[i]);
    }
    printf("'.\n");
}