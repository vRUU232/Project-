#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>

#define HASH_TABLE_SIZE 127

using namespace std;

// Define a struct for Parcel
struct Parcel {
    char* country;     // Country name (destination)
    int weight;        // Weight of the parcel
    float valuation;   // Valuation of the parcel

    Parcel(const char* c, int w, float v) : weight(w), valuation(v) {
        country = (char*)malloc(strlen(c) + 1);  // Allocate memory for the country name
        if (country) {
            strcpy_s(country, strlen(c) + 1, c);  // Copy the country name safely
        }
    }

    ~Parcel() {
        free(country);  // Free the allocated memory
    }
};

// Define a struct for a node in the Binary Search Tree
struct BSTNode {
    Parcel* parcel;     // Pointer to the parcel
    BSTNode* left;      // Left child
    BSTNode* right;     // Right child

    BSTNode(Parcel* p) : parcel(p), left(nullptr), right(nullptr) {}
};

// Define a struct for the Binary Search Tree
struct BST {
    BSTNode* root;      // Root node of the BST

    BST() : root(nullptr) {}

    void insert(Parcel* parcel) {
        root = insertRec(root, parcel);
    }

    void inorder(Parcel**& parcels, int& count, int& capacity) {
        inorderRec(root, parcels, count, capacity);
    }

    BSTNode* insertRec(BSTNode* node, Parcel* parcel) {
        if (!node) {
            return new BSTNode(parcel);
        }
        if (parcel->weight < node->parcel->weight) {
            node->left = insertRec(node->left, parcel);
        }
        else {
            node->right = insertRec(node->right, parcel);
        }
        return node;
    }

    void inorderRec(BSTNode* node, Parcel**& parcels, int& count, int& capacity) {
        if (node) {
            inorderRec(node->left, parcels, count, capacity);
            if (count >= capacity) {
                capacity *= 2;
                parcels = (Parcel**)realloc(parcels, capacity * sizeof(Parcel*));
            }
            parcels[count++] = node->parcel;
            inorderRec(node->right, parcels, count, capacity);
        }
    }

    ~BST() {
        deleteTree(root);
    }

    void deleteTree(BSTNode* node) {
        if (node) {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node->parcel;
            delete node;
        }
    }
};

// Define a struct for the Hash Table
struct HashTable {
    BST* table[HASH_TABLE_SIZE];

    HashTable() {
        for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
            table[i] = new BST();
        }
    }

    unsigned long hashFunction(const char* str) {
        unsigned long hash = 5381;
        while (*str) {
            hash = ((hash << 5) + hash) + *str;  // hash * 33 + c
            str++;
        }
        return hash % HASH_TABLE_SIZE;
    }

    void insert(Parcel* parcel) {
        unsigned long hashValue = hashFunction(parcel->country);
        table[hashValue]->insert(parcel);
    }

    BST* getTree(const char* country) {
        unsigned long hashValue = hashFunction(country);
        return table[hashValue];
    }

    ~HashTable() {
        for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
            delete table[i];
        }
    }
};

// Prototypes
void displayParcels(HashTable& hashTable, const char* country);
void displayWeightCondition(HashTable& hashTable, const char* country, int weight, bool higher);
void totalLoadAndValuation(HashTable& hashTable, const char* country);
void cheapestAndMostExpensive(HashTable& hashTable, const char* country);
void lightestAndHeaviest(HashTable& hashTable, const char* country);
void readFile(HashTable& hashTable, const char* filename);
void getUserInput(const char* prompt, char* buffer, int size);

int main() {
    // Create a hash table
    HashTable hashTable;

    // Read data from couriers.txt file
    readFile(hashTable, "couriers.txt");

    int choice;
    char country[100];
    char input[100];
    int weight;
    bool condition;

    do {
        // Display the user menu
        printf("\nUser Menu:\n");
        printf("1. Enter country name and display all the parcels details\n");
        printf("2. Enter country and weight pair\n");
        printf("3. Display the total parcel load and valuation for the country\n");
        printf("4. Enter the country name and display cheapest and most expensive parcel details\n");
        printf("5. Enter the country name and display lightest and heaviest parcel for the country\n");
        printf("6. Exit the application\n");
        printf("Enter your choice: ");

        // Get user choice
        fgets(input, sizeof(input), stdin);
        sscanf_s(input, "%d", &choice);
