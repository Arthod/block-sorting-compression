#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define NUM_SYMBOLS 258

typedef struct Node {
    uint16_t symbol;
    unsigned int freq;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct Code {
    uint16_t symbol;
    uint32_t code;
    unsigned int length;
} Code;

Node* newNode(uint16_t symbol, unsigned int freq) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->symbol = symbol;
    node->freq = freq;
    node->left = node->right = NULL;
    return node;
}

void insertHeap(Node** heap, int* heapSize, Node* node) {
    int i = *heapSize;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap[parent]->freq <= node->freq) {
            break;
        }
        heap[i] = heap[parent];
        i = parent;
    }
    heap[i] = node;
    (*heapSize)++;
}

Node* removeMin(Node** heap, int* heapSize) {
    Node* min = heap[0];
    (*heapSize)--;
    Node* last = heap[*heapSize];

    int i = 0;
    while (1) {
        int left = 2 * i + 1;
        int right = left + 1;

        int minChild = left;
        if (right < *heapSize && heap[right]->freq < heap[left]->freq) {
            minChild = right;
        }
        if (minChild >= *heapSize || heap[minChild]->freq >= last->freq) {
            break;
        }
        heap[i] = heap[minChild];
        i = minChild;
    }
    if (*heapSize > 0) {
        heap[i] = last;
    }
    return min;
}

Node* buildHuffmanTree(const uint16_t* data, size_t dataSize) {
    unsigned int freqs[NUM_SYMBOLS] = {0};
    for (size_t i = 0; i < dataSize; ++i) {
        freqs[data[i]]++;
    }

    Node* heap[2 * NUM_SYMBOLS];
    memset(heap, 0, sizeof(Node*) * 2 * NUM_SYMBOLS); // Initialize the heap array with NULL pointers
    int heapSize = 0;
    for (uint16_t i = 0; i < NUM_SYMBOLS; ++i) {
        if (freqs[i] > 0) {
            insertHeap(heap, &heapSize, newNode(i, freqs[i]));
        }
    }

    if (heapSize == 1) {
        // Special case: only one unique symbol in the input data
        return heap[0];
    }

    while (heapSize > 1) {
        Node* left = removeMin(heap, &heapSize);
        Node* right = removeMin(heap, &heapSize);
        Node* merged = newNode(0, left->freq + right->freq);
        merged->left = left;
        merged->right = right;
        insertHeap(heap, &heapSize, merged);
    }

    return heap[0];
}

void traverseTree(const Node* node, uint32_t code, unsigned int length, Code* codes, unsigned int* codesCount) {
    if (!node->left && !node->right) {
        codes[*codesCount].symbol = node->symbol;
        codes[*codesCount].code = code;
        codes[*codesCount].length = length;
        (*codesCount)++;
        return;
    }

    if (node->left) {
        traverseTree(node->left, code << 1, length + 1, codes, codesCount);
    }

    if (node->right) {
        traverseTree(node->right, (code << 1) | 1, length + 1, codes, codesCount);
    }
}

void compressDataToFile(uint16_t *data, size_t dataSize, Code *codes, unsigned int codesCount, FILE *outputFile) {
    unsigned char buffer = 0;
    unsigned int bufferBitsCount = 0;

    for (size_t i = 0; i < dataSize; ++i) {
        uint16_t symbol = data[i];

        for (unsigned int j = 0; j < codesCount; ++j) {
            if (codes[j].symbol == symbol) {
                for (unsigned int k = 0; k < codes[j].length; ++k) {
                    buffer <<= 1;
                    buffer |= (codes[j].code >> (codes[j].length - 1 - k)) & 1;
                    bufferBitsCount++;

                    if (bufferBitsCount == 8) {
                        fwrite(&buffer, sizeof(buffer), 1, outputFile);
                        buffer = 0;
                        bufferBitsCount = 0;
                    }
                }
                break;
            }
        }
    }

    if (bufferBitsCount > 0) {
        buffer <<= (8 - bufferBitsCount);
        fwrite(&buffer, sizeof(buffer), 1, outputFile);
    }
}

void freeHuffmanTree(Node *root) {
    if (!root) {
        return;
    }

    if (root->left) {
        freeHuffmanTree(root->left);
    }

    if (root->right) {
        freeHuffmanTree(root->right);
    }

    free(root);
}

void write_huffman_tree(const Node *node, FILE *outputFile) {
    if (!node) return;

    unsigned char is_leaf = !node->left && !node->right;
    fwrite(&is_leaf, sizeof(is_leaf), 1, outputFile);

    if (is_leaf) {
        fwrite(&node->symbol, sizeof(node->symbol), 1, outputFile);
    } else {
        write_huffman_tree(node->left, outputFile);
        write_huffman_tree(node->right, outputFile);
    }
}

Node *read_huffman_tree(FILE *inputFile) {
    unsigned char is_leaf;
    if (fread(&is_leaf, sizeof(is_leaf), 1, inputFile) != 1) {
        return NULL;
    }

    if (is_leaf) {
        uint16_t symbol;
        if (fread(&symbol, sizeof(symbol), 1, inputFile) != 1) {
            return NULL;
        }
        return newNode(symbol, 0);
    } else {
        Node *left = read_huffman_tree(inputFile);
        Node *right = read_huffman_tree(inputFile);
        Node *merged = newNode(0, 0);
        merged->left = left;
        merged->right = right;
        return merged;
    }
}

void huffman_encode(uint16_t *data, size_t dataSize, FILE *outputFile) {
    Node *huffmanTree = buildHuffmanTree(data, dataSize);

    Code codes[NUM_SYMBOLS];
    unsigned int codesCount = 0;
    traverseTree(huffmanTree, 0, 0, codes, &codesCount);

    fwrite(&dataSize, sizeof(dataSize), 1, outputFile);
    write_huffman_tree(huffmanTree, outputFile);

    compressDataToFile(data, dataSize, codes, codesCount, outputFile);

    freeHuffmanTree(huffmanTree);
}

int huffman_decode(FILE *inputFile, uint16_t **decodedData, size_t *decodedDataSize) {
    if (fread(decodedDataSize, sizeof(*decodedDataSize), 1, inputFile) != 1) {
        return 1;
    }

    Node *huffmanTree = read_huffman_tree(inputFile);
    if (!huffmanTree) {
        return 1;
    }

    *decodedData = (uint16_t *)malloc(*decodedDataSize * sizeof(uint16_t));
    unsigned char buffer;
    size_t dataIndex = 0;
    Node *currentNode = huffmanTree;

    while (fread(&buffer, sizeof(buffer), 1, inputFile) == 1) {
        for (int i = 0; i < 8 && dataIndex < *decodedDataSize; ++i) {
            currentNode = (buffer & (1 << (7 - i))) ? currentNode->right : currentNode->left;
            if (!currentNode->left && !currentNode->right) {
                (*decodedData)[dataIndex++] = currentNode->symbol;
                currentNode = huffmanTree;
            }
        }
    }

    if (dataIndex != *decodedDataSize) {
        free(*decodedData);
        *decodedData = NULL;
        freeHuffmanTree(huffmanTree);
        return 1;
    }

    freeHuffmanTree(huffmanTree);
    return 0;
}

/*
int main() {
    uint16_t data[] = {1, 3, 3, 5, 6, 7, 8, 8, 8, 9, 9};
    size_t dataSize = sizeof(data) / sizeof(data[0]);

    FILE *outputFile = fopen("compressed.bin", "wb");
    if (!outputFile) {
        fprintf(stderr, "Error opening output file.\n");
        return 1;
    }

    huffman_encode(data, dataSize, outputFile);
    fclose(outputFile);

    FILE *inputFile = fopen("compressed.bin", "rb");
    if (!inputFile) {
        fprintf(stderr, "Error opening input file.\n");
        return 1;
    }

    uint16_t *decodedData;
    size_t decodedDataSize;
    if (huffman_decode(inputFile, &decodedData, &decodedDataSize) != 0) {
        fprintf(stderr, "Error decoding input file.\n");
        fclose(inputFile);
        return 1;
    }

    fclose(inputFile);

    // Verify that the decoded data is the same as the original data
    if (decodedDataSize != dataSize) {
        fprintf(stderr, "Decoded data size does not match the original data size.\n");
        return 1;
    }

    for (size_t i = 0; i < dataSize; ++i) {
        if (data[i] != decodedData[i]) {
            fprintf(stderr, "Decoded data does not match the original data.\n");
            return 1;
        }
    }

    free(decodedData);

    printf("Huffman encoding and decoding successful!\n");
    return 0;
}
*/