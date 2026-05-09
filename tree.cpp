// tree.cpp - Binary Tree Implementation for Huffman Encoding
#include "tree.h"

Tree::Tree() : root(nullptr)
{
}

Tree::~Tree()
{
    clear();
}

void Tree::deleteTree(TreeNode* node)
{
    if (node == nullptr)
        return;
    
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

void Tree::setRoot(TreeNode* node)
{
    root = node;
}

TreeNode* Tree::getRoot() const
{
    return root;
}

void Tree::insertNode(TreeNode* node)
{
    if (node == nullptr)
        return;
    
    if (root == nullptr)
    {
        root = node;
    }
}

void Tree::getCharacterCode(char, char* code)
{
    if (code == nullptr)
        return;
    
    code[0] = '\0';
}

void Tree::displayTree()
{
    // Simple display function for debugging
}

void Tree::clear()
{
    deleteTree(root);
    root = nullptr;
}

bool Tree::isEmpty() const
{
    return root == nullptr;
}
