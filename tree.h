// tree.h - Binary Tree for Huffman Encoding
#ifndef TREE_H
#define TREE_H

#include <cstring>
#include <iostream>

// Node structure for the binary tree
struct TreeNode
{
    char data;              // Character value
    int frequency;          // Frequency of character
    TreeNode* left;         // Left child
    TreeNode* right;        // Right child
    
    TreeNode(char d, int f) : data(d), frequency(f), left(nullptr), right(nullptr) {}
    TreeNode() : data('\0'), frequency(0), left(nullptr), right(nullptr) {}
};

class Tree
{
private:
    TreeNode* root;
    
    // Helper functions for tree operations
    void deleteTree(TreeNode* node);
    void inorderTraversal(TreeNode* node, char* codes, int depth, char** result, int& count);
    
public:
    Tree();
    ~Tree();
    
    // Basic tree operations
    void setRoot(TreeNode* node);
    TreeNode* getRoot() const;
    
    // Insert node into tree
    void insertNode(TreeNode* node);
    
    // Get code for a character
    void getCharacterCode(char ch, char* code);
    
    // Display tree structure (for debugging)
    void displayTree();
    
    // Clear tree
    void clear();
    
    // Check if tree is empty
    bool isEmpty() const;
};

#endif // TREE_H
