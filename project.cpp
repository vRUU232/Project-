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
