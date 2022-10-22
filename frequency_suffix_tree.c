#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct Node {
    uint32_t frequency;
    struct Arc *arcs;
    uint16_t arcs_count;
} Node;

typedef struct Arc {
    struct Node node;
    uint16_t symbol;
} Arc;

Node fst_create(uint16_t *block, uint64_t block_size, int depth) {
    // Initialize root FST
    Node *root = malloc(sizeof(Node));
    root->frequency = 0;
    root->arcs = NULL;
    root->arcs_count = 0;

    // Largest substring
    int substring_largest_frequency = 0;
    int *substring_largest = malloc(depth * sizeof(int));
    int *substring_current = malloc(depth * sizeof(int));

    // Iterate through block with block_size and depth size
    for (uint64_t i = 0; i < block_size; i++) {
        if (i % 500000 == 0) {
            printf("%ld / %ld\n", i, block_size);
            printf("Root: %d\n", root->frequency);
        }
        Node *node_current = root;

        for (int j = 0; j < depth; j++) {
            substring_current[j] = block[i + j];

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

            node_current->frequency++;

            if (arc_found == 1) {
                // If we found the arc with the symbol, increment the next nodes frequency, add set it to node_current
                node_current = &arc.node;
                node_current->frequency++;
                //printf("Incremented frequency. Now: %d\n", node_current->frequency);
                
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

        node_current = root;
    }

    // Print longest substring with frequency
    printf("Largest substring of length: %d, has frequency: %d, and is '", depth, substring_largest_frequency);
    for (int i = 0; i < depth; i++) {
        printf("%c", substring_largest[i]);
    }
    printf("'.\n");
}