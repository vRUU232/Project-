#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>

#define HASH_TABLE_SIZE 127

using namespace std;

struct Parcel 
{
    string country;     
    int weight;        
    float valuation;    
    Parcel(const string& c, int w, float v) : country(c), weight(w), valuation(v) {}
};


struct BSTNode 
{
    Parcel* parcel;     
    BSTNode* left;     
    BSTNode* right;    

    BSTNode(Parcel* p) : parcel(p), left(nullptr), right(nullptr) {}
};


struct BST
{
    BSTNode* root;     

    BST() : root(nullptr) {}

    void insert(Parcel* parcel) 
{
        root = insertRec(root, parcel);
    }

    void inorder(Parcel**& parcels, int& count, int& capacity)
{
        inorderRec(root, parcels, count, capacity);
    }

    BSTNode* insertRec(BSTNode* node, Parcel* parcel) 
{
        if (!node)
        {
            return new BSTNode(parcel);
        }
        if (parcel->weight < node->parcel->weight)
        {
            node->left = insertRec(node->left, parcel);
        }
        else
        {
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


struct HashTable 
{
    BST* table[HASH_TABLE_SIZE];

    HashTable() 
{
        for (int i = 0; i < HASH_TABLE_SIZE; ++i) 
        {
            table[i] = new BST();
        }
    }

    unsigned long hashFunction(const string& str)
{
        unsigned long hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + c;  
        }
        return hash % HASH_TABLE_SIZE;
    }

    void insert(Parcel* parcel)
{
        unsigned long hashValue = hashFunction(parcel->country);
        table[hashValue]->insert(parcel);
    }
    BST* getTree(const string& country) 
{
        unsigned long hashValue = hashFunction(country);
        return table[hashValue];
    }

    ~HashTable() {
        for (int i = 0; i < HASH_TABLE_SIZE; ++i)
            {
            delete table[i];
        }
    }
};

void displayParcels(HashTable& hashTable, const char* country);
void displayWeightCondition(HashTable& hashTable, const char* country, int weight, bool higher);
void totalLoadAndValuation(HashTable& hashTable, const char* country);
void cheapestAndMostExpensive(HashTable& hashTable, const char* country);
void lightestAndHeaviest(HashTable& hashTable, const char* country);
void readFile(HashTable& hashTable, const char* filename);
void getUserInput(const char* prompt, char* buffer, int size);

