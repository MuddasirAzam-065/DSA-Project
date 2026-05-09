// queue.h - Circular Queue for Huffman Tree Building
#ifndef QUEUE_H
#define QUEUE_H

#include "tree.h"

#define MAX_QUEUE_SIZE 1000

class Queue
{
private:
    TreeNode* items[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
    
public:
    Queue();
    ~Queue();
    
    // Queue operations
    bool enqueue(TreeNode* item);
    TreeNode* dequeue();
    TreeNode* peek() const;
    
    // Status checks
    bool isEmpty() const;
    bool isFull() const;
    int getSize() const;
    
    // Utility
    void clear();
};

#endif // QUEUE_H
