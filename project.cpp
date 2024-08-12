#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>

#define HASH_TABLE_SIZE 127
#define MAX_COUNTRY_NAME_LENGTH 20 

using namespace std;

// Define a struct for Parcel
struct Parcel {
    char* country;     // Country name (destination)
    int weight;        // Weight of the parcel
    float valuation;   // Valuation of the parcel

    Parcel(const char* c, int w, float v) : weight(w), valuation(v) {
        country = (char*)malloc(MAX_COUNTRY_NAME_LENGTH + 1);  // Allocate memory for MAX_COUNTRY_NAME_LENGTH 
            strncpy_s(country, MAX_COUNTRY_NAME_LENGTH + 1, c, _TRUNCATE);  
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
    char input[100];
    char country[100];
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

        // Get user input
        fgets(input, sizeof(input), stdin);

        // **Input Validation**: Check if the input is a valid number
        if (sscanf_s(input, "%d", &choice) != 1) {
            printf("Error: Invalid input. Please enter a number between 1 and 6.\n");
            continue;  // Restart the loop for another input
        }

        // Handle valid menu choices
        switch (choice) {
        case 1:
            getUserInput("Enter country name: ", country, sizeof(country));
            displayParcels(hashTable, country);
            break;
        case 2: {
            // Prompt for the country name
            getUserInput("Enter country name: ", country, sizeof(country));

            // Prompt for the weight to compare
            printf("Enter the weight to compare: ");
            fgets(input, sizeof(input), stdin);
            sscanf_s(input, "%d", &weight);

            // Ask the user if they want parcels with higher or lower weights
            printf("Enter 'H' for weight higher or 'L' for weight lower: ");
            fgets(input, sizeof(input), stdin);

            // Convert the first character of the input to uppercase
            char conditionChar = toupper(input[0]);

            // Determine the condition based on user input
            if (conditionChar == 'H') {
                condition = true;  // Higher
            }
            else if (conditionChar == 'L') {
                condition = false; // Lower
            }
            else {
                printf("Invalid input. Please enter 'H' for higher or 'L' for lower.\n");
                break;
            }

            // Call the updated displayWeightCondition function
            displayWeightCondition(hashTable, country, weight, condition);

            break;
        }
        case 3:
            getUserInput("Enter country name: ", country, sizeof(country));
            totalLoadAndValuation(hashTable, country);
            break;
        case 4:
            getUserInput("Enter country name: ", country, sizeof(country));
            cheapestAndMostExpensive(hashTable, country);
            break;
        case 5:
            getUserInput("Enter country name: ", country, sizeof(country));
            lightestAndHeaviest(hashTable, country);
            break;
        case 6:
            printf("Exiting the application.\n");
            break;
        default:
            // **Invalid Menu Choice Handling**: Handle choices outside the range of 1 to 6
            printf("Error: Invalid choice. Please select a number between 1 and 6.\n");
            break;
        }
    } while (choice != 6);

    return 0;
}
