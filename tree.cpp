// tree.cpp - Binary Tree Implementation for Huffman Encoding
#include "tree.h"

Tree::Tree() : root(nullptr)
{
}

Tree::Tree(const Tree& other) : root(nullptr)
{
    root = cloneTree(other.root);
}

Tree& Tree::operator=(const Tree& other)
{
    if (this == &other)
        return *this;
    
    clear();
    root = cloneTree(other.root);
    return *this;
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

TreeNode* Tree::cloneTree(const TreeNode* node)
{
    if (node == nullptr)
        return nullptr;
    
    TreeNode* copy = new TreeNode(node->data, node->frequency);
    copy->left = cloneTree(node->left);
    copy->right = cloneTree(node->right);
    return copy;
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
