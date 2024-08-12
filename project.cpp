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
        country = (char*)malloc(MAX_COUNTRY_NAME_LENGTH + 1);  // Allocate memory for MAX_COUNTRY_NAME_LENGTH characters + null terminator
        if (country) {
            strncpy_s(country, MAX_COUNTRY_NAME_LENGTH + 1, c, MAX_COUNTRY_NAME_LENGTH);  // Safe copy up to MAX_COUNTRY_NAME_LENGTH characters
        }
    }

    ~Parcel() {
        free(country);  // Free the allocated memory
    }
};

// Define a struct for a node in the Binary Search Tree
struct BSTDataNode {
    Parcel* parcel;     // Pointer to the parcel
    BSTDataNode* left;      // Left child
    BSTDataNode* right;     // Right child

    BSTDataNode(Parcel* p) : parcel(p), left(nullptr), right(nullptr) {}
};

// Define a struct for the Binary Search Tree
struct BST {
    BSTDataNode* root;      // Root node of the BST

    BST() : root(nullptr) {}

    void insert(Parcel* parcel) {
        root = insertIntoTree(root, parcel);
    }

    void inorder(Parcel**& parcels, int& count, int& capacity) {
        inorderTraversal(root, parcels, count, capacity);
    }

    BSTDataNode* insertIntoTree(BSTDataNode* node, Parcel* parcel) {
        if (!node) {
            return new BSTDataNode(parcel);
        }
        if (parcel->weight < node->parcel->weight) {
            node->left = insertIntoTree(node->left, parcel);
        }
        else {
            node->right = insertIntoTree(node->right, parcel);
        }
        return node;
    }

    void inorderTraversal(BSTDataNode* node, Parcel**& parcels, int& count, int& capacity) {
        if (node) {
            inorderTraversal(node->left, parcels, count, capacity);
            if (count >= capacity) {
                capacity *= 2;
                parcels = (Parcel**)realloc(parcels, capacity * sizeof(Parcel*));
            }
            parcels[count++] = node->parcel;
            inorderTraversal(node->right, parcels, count, capacity);
        }
    }

    ~BST() {
        deleteTree(root);
    }

    void deleteTree(BSTDataNode* node) {
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
void showParcelsList(HashTable& hashTable, const char* country);
void showParcelWeight(HashTable& hashTable, const char* country, int weight, bool higher);
void totalLoadAndValuation(HashTable& hashTable, const char* country);
void showCost(HashTable& hashTable, const char* country);
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
        printf("----------\n");
        printf("1. Enter country name and display all the parcels details\n");
        printf("2. Enter country and weight pair\n");
        printf("3. Display the total parcel load and valuation for the country\n");
        printf("4. Enter the country name and display cheapest and most expensive parcel details\n");
        printf("5. Enter the country name and display lightest and heaviest parcel for the country\n");
        printf("6. Exit the application\n");
        printf("\n");
        printf("Enter your choice: ");

        // Get user input
        fgets(input, sizeof(input), stdin);

        // **Input Validation**: Check if the input is a valid number
        if (sscanf_s(input, "%d", &choice) != 1) {
            printf("\n");
            printf("Invalid input. Please enter number between 1 and 6.\n");
            continue;  // Restart the loop for another input
        }

        // Handle valid menu choices
        switch (choice) {
        case 1:
            getUserInput("Enter country name: ", country, sizeof(country));
            showParcelsList(hashTable, country);
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

            // Call the updated showParcelWeight function
            showParcelWeight(hashTable, country, weight, condition);

            break;
        }
        case 3:
            getUserInput("Enter country name: ", country, sizeof(country));
            totalLoadAndValuation(hashTable, country);
            break;
        case 4:
            getUserInput("Enter country name: ", country, sizeof(country));
            showCost(hashTable, country);
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

/*
* FUNCTION    : showParcelsList
* DESCRIPTION : This function displays all parcels for a given country.
* PARAMETERS  : HashTable& hashTable - The hash table.
*               const char* country - The country name.
*/
void showParcelsList(HashTable& hashTable, const char* country) {
    // Retrieve the binary search tree associated with the given country
    BST* tree = hashTable.getTree(country);

    if (tree) {
        int count = 0, capacity = 10;
        Parcel** parcels = (Parcel**)malloc(capacity * sizeof(Parcel*));
        tree->inorder(parcels, count, capacity); // Get all parcels in sorted order based on weight

        bool found = false;
        printf("Parcels for country %s:\n", country);

        // Iterate over all parcels and display only those matching the specified country
        for (int i = 0; i < count; i++) {
            if (strcmp(parcels[i]->country, country) == 0) {
                found = true;
                printf("Destination: %s, Weight: %d, Valuation: %.2f\n", parcels[i]->country, parcels[i]->weight, parcels[i]->valuation);
            }
        }

        if (!found) {
            printf("No parcels found for country %s\n", country);
        }

        free(parcels);
    }
    else {
        printf("No parcels found for country %s\n", country);
    }
}
/*
* FUNCTION    : showParcelWeight
* DESCRIPTION : This function displays parcels for a given country based on weight condition.
* PARAMETERS  : HashTable& hashTable - The hash table.
*               const char* country - The country name.
*               int weight - The weight condition.
*               bool higher - If true, display parcels with weight higher than the given weight; otherwise, display parcels with weight lower than the given weight.
*/
void showParcelWeight(HashTable& hashTable, const char* country, int weight, bool higher) {
    // Retrieve the BST associated with the given country
    BST* tree = hashTable.getTree(country);

    if (tree) {
        int count = 0, capacity = 10;
        Parcel** parcels = (Parcel**)malloc(capacity * sizeof(Parcel*));
        tree->inorder(parcels, count, capacity); // Retrieve all parcels in the BST

        printf("Parcels for country %s with weight %s than %d:\n", country, higher ? "higher" : "lower", weight);

        bool found = false; // Flag to check if any parcel meets the criteria

        for (int i = 0; i < count; i++) {
            // Ensure the parcel's country matches the specified country
            if (strcmp(parcels[i]->country, country) == 0) {
                // Check if the parcel's weight meets the condition
                if ((higher && parcels[i]->weight > weight) || (!higher && parcels[i]->weight < weight)) {
                    printf("Destination: %s, Weight: %d, Valuation: %.2f\n", parcels[i]->country, parcels[i]->weight, parcels[i]->valuation);
                    found = true; // At least one parcel meets the criteria
                }
            }
        }

        if (!found) {
            printf("No parcels found for country %s with the specified weight condition.\n", country);
        }

        free(parcels);
    }
    else {
        printf("No parcels found for country %s.\n", country);
    }
} 
