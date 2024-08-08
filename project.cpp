#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Parcel
{
    char* destination;
    int weight;
    float value;
} Parcel;

typedef struct BSTNode
{
    Parcel* parcel;    
    struct BSTNode* left;    
    struct BSTNode* right;    
} BSTNode;

typedef struct BST 
{
    BSTNode* root;     
} BST;

unsigned int hashFunction(char* key);
BSTNode* createBSTNode(char* destination, int weight, float value);
BSTNode* insertIntoBST(BSTNode* root, char* destination, int weight, float value);

void insertIntoHashTable(HashTable* hashTable[], char* destination, int weight, float value);
void loadData(HashTable* hashTable[], const char* filename);
