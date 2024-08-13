/*
* File          :  project.cpp
* Project       :  Parcel Management System Using Hash Table and Binary Search Tree.
* Programmer    :  Prit Patel (8979395)
                           : Vruti Mistry (8963142)
* First version : 2024-08-07
* Description   : This program handles parcel data for delivery service, saving and
*                 retrieving parcels based on destination country. This hash
*                 table mapping country names maps them to binary search trees which
*                 organize parcels by weight.
*                 The system supports the following operations: displaying of all parcels
*                 for a country, filtering parcels by weight, calculation of the total load
*                 and valuation for a country, and identifying the cheapest, most expensive,
*                 lightest, and heaviest parcels.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#pragma warning(disable: 4996)
#define HASH_TABLE_SIZE 127
#define MAX_COUNTRY_NAME_LENGTH 20  

using namespace std;

// Define a struct to represent a parcel
struct Parcel {
    char* country;      // Pointer to store the country name (destination)
    int weight;        // Weight of the parcel
    float valuation;   // Valuation of the parcel
    // Constructor to initialize a Parcel object with the country name, weight, and valuation

    Parcel(const char* c, int w, float v) : weight(w), valuation(v) {
        // Allocate memory for the country name, with space for the null terminator
        country = (char*)malloc(MAX_COUNTRY_NAME_LENGTH + 1);  // Allocate memory for MAX_COUNTRY_NAME_LENGTH characters + null terminator
        if (country) {
            // Copy the provided country name into the allocated memory, ensuring no buffer overflow
            strncpy_s(country, MAX_COUNTRY_NAME_LENGTH + 1, c, MAX_COUNTRY_NAME_LENGTH);  // Safe copy up to MAX_COUNTRY_NAME_LENGTH characters
        }
    }
    // Destructor to free the allocated memory for the country name
    ~Parcel() {
        free(country);  // Free the allocated memory
    }
};

// Define a struct to represent a node in a Binary Search Tree (BST)
// This node will store a pointer to a Parcel and pointers to its left and right children
struct BSTDataNode {
    Parcel* parcel;     // Pointer to the parcel
    BSTDataNode* left;      // Left child
    BSTDataNode* right;     // Right child
    // Constructor to initialize a BSTDataNode with a given Parcel
    BSTDataNode(Parcel* p) : parcel(p), left(nullptr), right(nullptr) {}
};

// Define a struct for a Binary Search Tree 
// The BST is used to store and manage Parcel objects based on their weight
struct BST {
    BSTDataNode* root;      // Root node of the BST
    // Constructor to initialize an empty BST
    BST() : root(nullptr) {}
    // Function to insert a Parcel into the BST based on its weight
    void insert(Parcel* parcel) {
        root = insertIntoTree(root, parcel);
    }
    // Function to perform an inorder traversal of the BST
    // The function fills the parcels array with pointers to the Parcel objects in sorted order
    void inorder(Parcel**& parcels, int& count, int& capacity) {
        inorderTraversal(root, parcels, count, capacity);
    }
    // Helper function to insert a Parcel into the BST, used recursively
    BSTDataNode* insertIntoTree(BSTDataNode* node, Parcel* parcel) {
        if (!node) {
            return new BSTDataNode(parcel);
        }
        if (parcel->weight < node->parcel->weight) {
            // Insert the parcel into the left subtree if its weight is less than the current node's parcel
            node->left = insertIntoTree(node->left, parcel);
        }
        else {
            node->right = insertIntoTree(node->right, parcel);
        }
        return node;
    }
    // Recursive function to perform an inorder traversal of the BST
// This function populates an array with pointers to Parcel objects in sorted order
    void inorderTraversal(BSTDataNode* node, Parcel**& parcels, int& count, int& capacity) {
        // Base case: If the current node is null, return from the function
        if (node) {
            // Recursively traverse the left subtree
            inorderTraversal(node->left, parcels, count, capacity);

            // Check if the parcels array has reached its capacity
            if (count >= capacity) {
                // Double the capacity of the array to accommodate more Parcel pointers
                capacity *= 2;
                // Allocate more memory for the expanded array
                Parcel** temp = (Parcel**)realloc(parcels, capacity * sizeof(Parcel*));

                // If memory allocation fails, free the existing array and set parcels to null
                if (!temp) {
                    free(parcels);
                    parcels = nullptr;
                    return;  // Exit the function if memory allocation fails
                }

                // Update the parcels pointer to point to the newly allocated memory
                parcels = temp;
            }

            // Add the current node's parcel to the parcels array and increment the count
            parcels[count++] = node->parcel;

            // Recursively traverse the right subtree
            inorderTraversal(node->right, parcels, count, capacity);
        }
    }

    // Destructor to clean up the BST by deleting all its nodes
    ~BST() {
        deleteTree(root);
    }
    // function to delete all nodes in the BST recursively

    void deleteTree(BSTDataNode* node) {
        if (node) {
            deleteTree(node->left); // Delete the left subtree
            deleteTree(node->right); // Delete the right subtree
            delete node->parcel;
            delete node;
        }
    }
};

// Define a struct for the Hash Table
// The Hash Table stores BSTs, each of which manages parcels for a specific hash value
struct HashTable {
    BST* table[HASH_TABLE_SIZE];

    HashTable() {
        for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
            table[i] = new BST();
        }
    }
    // Hash function to compute the hash value of a country name
    unsigned long hashFunction(const char* str) {
        unsigned long hash = 5381;
        while (*str) {
            hash = ((hash << 5) + hash) + *str;  // hash * 33 + c
            str++;
        }
        return hash % HASH_TABLE_SIZE;
    }
    // Function to insert a Parcel into the appropriate BST in the Hash Table
    void insert(Parcel* parcel) {
        unsigned long hashValue = hashFunction(parcel->country);
        table[hashValue]->insert(parcel);
    }
    // Function to retrieve the BST associated with a given country name
    BST* getTree(const char* country) {
        unsigned long hashValue = hashFunction(country);
        return table[hashValue];
    }
    // Destructor to clean up the Hash Table by deleting all BSTs
    ~HashTable() {
        for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
            delete table[i];
        }
    }
};

// Function prototypes

// Shows the details of all parcels for a given country by searching the hash table
void showParcelsList(HashTable& hashTable, const char* country);

// Displays parcels for a specific country that meet a weight condition
// 'higher' determines whether to show parcels heavier or lighter than the specified weight
void showParcelWeight(HashTable& hashTable, const char* country, int weight, bool higher);

// Calculates and prints the total weight and valuation of all parcels for a country
// Returns true if the country has parcels, false otherwise
bool checkTotalLoadAndValuation(HashTable& hashTable, const char* country);

// Displays the cheapest and most expensive parcels for a given country
void showCost(HashTable& hashTable, const char* country);

// Finds and displays the lightest and heaviest parcels for a specific country
void lightestAndHeaviest(HashTable& hashTable, const char* country);

// Reads parcel data from a file and adds them to the hash table
void readFile(HashTable& hashTable, const char* filename);

// Prompts the user for input with a given prompt and stores it in 'buffer'
// Removes any trailing newline characters from the input
void getUserInput(const char* prompt, char* buffer, int size);

int main() {
    // Create a hash table to store parcels
    HashTable hashTable;

    // Read parcel data from the couriers.txt file and populate the hash table
    readFile(hashTable, "couriers.txt");
    // Variable to store user choice from the menu
    int choice;
    char country[MAX_COUNTRY_NAME_LENGTH];
    char input[100];
    // Variable to store the weight 
    int weight;

    do {
        // Display the user menu with options
        printf("\nUser Menu:\n");
        printf("----------\n");
        printf("1. Enter country name and display all the parcels details\n");
        printf("2. Enter country and weight pair\n");
        printf("3. Display the total parcel load and valuation for the country\n");
        printf("4. Enter the country name and display cheapest and most expensive parcel’s details\n");
        printf("5. Enter the country name and display lightest and heaviest parcel for the country\n");
        printf("6. Exit the application\n");
        printf("\n");
        printf("Enter your choice: ");

        // Get the user's choice from the menu
        fgets(input, sizeof(input), stdin);
        sscanf_s(input, "%d", &choice);

        switch (choice) {
        case 1:
            // Case 1: Display all parcel details for a given country
            getUserInput("Enter country name: ", country, sizeof(country));
            showParcelsList(hashTable, country);// Call function to display all parcels for the given country

            break;
        case 2:
            // Case 2: Display parcels for a country based on weight condition
        {
            // Prompt for the country name
            getUserInput("Enter country name: ", country, sizeof(country));

            // Retrieve the BST associated with the given country
            BST* tree = hashTable.getTree(country);

            // Allocate memory for an array of Parcel pointers to store the retrieved parcels
            int count = 0, capacity = 10;
            Parcel** parcels = (Parcel**)malloc(capacity * sizeof(Parcel*));
            if (!parcels) {
                // Handle memory allocation failure
                printf("Memory allocation failed.\n");
                return 1; // Exit the program if memory allocation fails
            }
            // Perform an inorder traversal of the tree to populate the parcels array
            tree->inorder(parcels, count, capacity);

            bool found = false;
            for (int i = 0; i < count; i++) {
                if (strcmp(parcels[i]->country, country) == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                // If no parcels were found for the country, display an error message and exit the case
                printf("\n");
                printf("Invalid country name or no parcels found for country %s.\n", country);
                free(parcels);
                break;
            }

            // Prompt for the weight to compare
            printf("Enter the weight to compare: ");
            fgets(input, sizeof(input), stdin);
            sscanf_s(input, "%d", &weight);

            // Prompt the user to enter the weight for comparison
            char conditionChar;
            do {
                printf("Enter 'H' for weight higher or 'L' for weight lower: ");
                fgets(input, sizeof(input), stdin);
                conditionChar = toupper(input[0]);

                if (conditionChar != 'H' && conditionChar != 'L') {
                    // If the user enters an invalid choice, display an error message
                    printf("\n");
                    printf("Invalid input. Please enter 'H' for higher or 'L' for lower.\n");
                }
            } while (conditionChar != 'H' && conditionChar != 'L');

            // Determine the condition based on user input
            bool condition = (conditionChar == 'H');

            // Call the showParcelWeight function
            showParcelWeight(hashTable, country, weight, condition);

            free(parcels);
            break;
        }

        case 3:
            // Case 3: Display the total parcel load and valuation for a given country
            getUserInput("Enter country name: ", country, sizeof(country));

            // Check if the country has valid parcels
            if (!checkTotalLoadAndValuation(hashTable, country)) {
                // If no parcels are found for the country, display an error message
                printf("\n");
                printf("Invalid country, please try again.\n");
            }
            break;
        case 4:
            // Case 4: Display the cheapest and most expensive parcels for a given country
            getUserInput("Enter country name: ", country, sizeof(country));
            showCost(hashTable, country);
            break;
        case 5:
            // Case 5: Display the lightest and heaviest parcels for a given country
            getUserInput("Enter country name: ", country, sizeof(country));
            lightestAndHeaviest(hashTable, country);
            break;
        case 6:
            // Case 6: Exit the application
            printf("\n");
            printf("Exiting the application.\n");
            break;
        default:
            // Handle invalid menu choices by displaying an error message
            printf("\n");
            printf("Invalid choice, please enter a number between 1 and 6.\n");
            break;
        }
    } while (choice != 6); // Continue looping until the user chooses to exit
    // Return 0 to indicate successful program termination
    return 0;
}


/*
 * FUNCTION    : showParcelsList
 * DESCRIPTION : Displays all parcels for a given country by retrieving them
 *               from the binary search tree associated with that country in
 *               the hash table.
 * PARAMETERS  : HashTable& hashTable - Reference to the hash table.
 *               const char* country  - The name of the country to look up.
 */
void showParcelsList(HashTable& hashTable, const char* country) {
    // Retrieve the binary search tree associated with the given country
    BST* tree = hashTable.getTree(country);

    if (tree) {
        int count = 0, capacity = 10;

        // Allocate memory for an array of Parcel pointers to store the retrieved parcels
        Parcel** parcels = (Parcel**)malloc(capacity * sizeof(Parcel*));
        if (!parcels) {
            // Handle memory allocation failure
            printf("Memory allocation failed.\n");
            return;
        }

        // Perform an inorder traversal to populate the parcels array
        tree->inorder(parcels, count, capacity);

        bool found = false;  // Flag to check if any parcel is found for the given country

        // Iterate over all parcels and display those that match the specified country
        for (int i = 0; i < count; i++) {
            if (strcmp(parcels[i]->country, country) == 0) {
                // If a matching parcel is found, print its details in the specified format
                found = true;
                printf("Destination:%s,Weight:%d,Valuation:%.2f\n", parcels[i]->country, parcels[i]->weight, parcels[i]->valuation);
            }
        }

        // If no parcels were found for the country, notify the user
        if (!found) {
            printf("No parcels found for country %s\n", country);
        }

        // Free the allocated memory for the parcels array
        free(parcels);
    }
    else {
        // If the tree for the specified country is not found, notify the user
        printf("No parcels found for country %s\n", country);
    }
}

/*
 * FUNCTION    : showParcelWeight
 * DESCRIPTION : Displays parcels for a given country based on a weight
 *               condition. The function can filter parcels that are either
 *               heavier or lighter than the specified weight.
 * PARAMETERS  : HashTable& hashTable - Reference to the hash table.
 *               const char* country  - The name of the country to look up.
 *               int weight           - The weight to compare against.
 *               bool higher          - If true, shows parcels heavier than
 *                                      the specified weight; otherwise, shows
 *                                      parcels lighter than the specified weight.
 */
void showParcelWeight(HashTable& hashTable, const char* country, int weight, bool higher) {
    BST* tree = hashTable.getTree(country);

    if (tree) {
        int count = 0, capacity = 10;
        Parcel** parcels = (Parcel**)malloc(capacity * sizeof(Parcel*));
        if (!parcels) {
            printf("\n");
            printf("Memory allocation failed.\n");
            return;
        }

        // Initialize memory to NULL
        memset(parcels, 0, capacity * sizeof(Parcel*));

        tree->inorder(parcels, count, capacity);

        // Ensure the rest of the function properly handles cases where parcels might still be NULL
        // ... (rest of the function)

        free(parcels);
    }
    else {
        printf("\n");
        printf("No parcels found for country %s.\n", country);
    }
}

/*
 * FUNCTION    : checkTotalLoadAndValuation
 * DESCRIPTION : Calculates and displays the total load (weight) and total
 *               valuation of all parcels for a specified country.
 * PARAMETERS  : HashTable& hashTable - Reference to the hash table.
 *               const char* country  - The name of the country to look up.
 * RETURNS     : bool - Returns true if the country has parcels; false otherwise.
 */
bool checkTotalLoadAndValuation(HashTable& hashTable, const char* country) {
    BST* tree = hashTable.getTree(country);
    if (tree) {
        int count = 0, capacity = 10;
        Parcel** parcels = (Parcel**)malloc(capacity * sizeof(Parcel*));
        if (!parcels) {
            printf("\n");
            printf("Memory allocation failed.\n");
            return false;
        }
        tree->inorder(parcels, count, capacity);

        int totalWeight = 0;
        float totalValuation = 0;
        bool found = false;

        for (int i = 0; i < count; i++) {
            if (strcmp(parcels[i]->country, country) == 0) {  // Check if the parcel's country matches the input country
                totalWeight += parcels[i]->weight;
                totalValuation += parcels[i]->valuation;
                found = true;
            }
        }

        free(parcels);

        if (found) {
            printf("\n");
            printf("Total parcel load for country %s: %d\n", country, totalWeight);
            printf("\n");
            printf("Total parcel valuation for country %s: %.2f\n", country, totalValuation);
            return true;
        }
    }

    return false; // Return false if no parcels found for the country
}

/*
 * FUNCTION    : showCost
 * DESCRIPTION : Displays the details of the cheapest and most expensive
 *               parcels for a given country by evaluating all parcels stored
 *               in the associated binary search tree.
 * PARAMETERS  : HashTable& hashTable - Reference to the hash table.
 *               const char* country  - The name of the country to look up.
 */
void showCost(HashTable& hashTable, const char* country) {
    BST* tree = hashTable.getTree(country);
    if (tree) {
        int count = 0, capacity = 10;
        Parcel** parcels = (Parcel**)malloc(capacity * sizeof(Parcel*));
        if (!parcels) {
            printf("\n");
            printf("Memory allocation failed.\n");
            return;
        }
        tree->inorder(parcels, count, capacity);

        if (count == 0) {
            printf("\n");
            printf("No parcels found for country %s\n", country);
            free(parcels);
            return;
        }

        Parcel* cheapest = parcels[0];
        Parcel* mostExpensive = parcels[0];
        for (int i = 1; i < count; i++) {
            if (parcels[i]->valuation < cheapest->valuation) {
                cheapest = parcels[i];
            }
            if (parcels[i]->valuation > mostExpensive->valuation) {
                mostExpensive = parcels[i];
            }
        }
        printf("\n");
        printf("Cheapest parcel for country %s: Destination: %s, Weight: %d, Valuation: %.2f\n", country, cheapest->country, cheapest->weight, cheapest->valuation);
        printf("\n");
        printf("Most expensive parcel for country %s: Destination: %s, Weight: %d, Valuation: %.2f\n", country, mostExpensive->country, mostExpensive->weight, mostExpensive->valuation);

        free(parcels);
    }
    else {
        printf("\n");
        printf("No parcels found for country %s\n", country);
    }
}

/*
 * FUNCTION    : lightestAndHeaviest
 * DESCRIPTION : Displays the details of the lightest and heaviest parcels for
 *               a given country by evaluating all parcels stored in the
 *               associated binary search tree.
 * PARAMETERS  : HashTable& hashTable - Reference to the hash table.
 *               const char* country  - The name of the country to look up.
 */
void lightestAndHeaviest(HashTable& hashTable, const char* country) {
    BST* tree = hashTable.getTree(country);
    if (tree) {
        int count = 0, capacity = 10;
        Parcel** parcels = (Parcel**)malloc(capacity * sizeof(Parcel*));
        if (!parcels) {
            printf("Memory allocation failed.\n");
            return;
        }
        tree->inorder(parcels, count, capacity);

        if (count == 0) {
            printf("\n");
            printf("No parcels found for country %s\n", country);
            free(parcels);
            return;
        }

        Parcel* lightest = parcels[0];
        Parcel* heaviest = parcels[0];
        for (int i = 1; i < count; i++) {
            if (parcels[i]->weight < lightest->weight) {
                lightest = parcels[i];
            }
            if (parcels[i]->weight > heaviest->weight) {
                heaviest = parcels[i];
            }
        }
        printf("\n");
        printf("Lightest parcel for country %s: Destination: %s, Weight: %d, Valuation: %.2f\n", country, lightest->country, lightest->weight, lightest->valuation);
        printf("\n");
        printf("Heaviest parcel for country %s: Destination: %s, Weight: %d, Valuation: %.2f\n", country, heaviest->country, heaviest->weight, heaviest->valuation);

        free(parcels);
    }
    else {
        printf("\n");
        printf("No parcels found for country %s\n", country);
    }
}

/*
 * FUNCTION    : readFile
 * DESCRIPTION : Reads parcel data from a file and inserts each parcel into
 *               the hash table. The file should contain country, weight, and
 *               valuation data for each parcel.
 * PARAMETERS  : HashTable& hashTable - Reference to the hash table.
 *               const char* filename - The name of the file to read from.
 */
void readFile(HashTable& hashTable, const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        printf("Failed to open file %s\n", filename);
        return;
    }
    char country[MAX_COUNTRY_NAME_LENGTH];
    int weight;
    float valuation;
    while (file >> country >> weight >> valuation) {
        Parcel* parcel = new Parcel(country, weight, valuation);
        hashTable.insert(parcel);
    }
    file.close();
}


/*
 * FUNCTION    : getUserInput
 * DESCRIPTION : Prompts the user for input and stores the result in a buffer.
 *               The function also removes any trailing newline characters from
 *               the input string.
 * PARAMETERS  : const char* prompt - The prompt message to display to the user.
 *               char* buffer       - The buffer to store the user's input.
 *               int size           - The size of the buffer.
 */
void getUserInput(const char* prompt, char* buffer, int size) {
    // Function implementation here for getUserInput
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    }
}
