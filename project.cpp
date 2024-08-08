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
