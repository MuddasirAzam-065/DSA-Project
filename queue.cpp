// queue.cpp - Circular Queue Implementation
#include "queue.h"

Queue::Queue() : front(0), rear(-1), size(0)
{
    for (int i = 0; i < MAX_QUEUE_SIZE; i++)
    {
        items[i] = nullptr;
    }
}

Queue::~Queue()
{
    clear();
}

bool Queue::enqueue(TreeNode* item)
{
    if (isFull())
        return false;
    
    rear = (rear + 1) % MAX_QUEUE_SIZE;
    items[rear] = item;
    size++;
    
    return true;
}

TreeNode* Queue::dequeue()
{
    if (isEmpty())
        return nullptr;
    
    TreeNode* item = items[front];
    items[front] = nullptr;
    front = (front + 1) % MAX_QUEUE_SIZE;
    size--;
    
    return item;
}

TreeNode* Queue::peek() const
{
    if (isEmpty())
        return nullptr;
    
    return items[front];
}

bool Queue::isEmpty() const
{
    return size == 0;
}

bool Queue::isFull() const
{
    return size >= MAX_QUEUE_SIZE;
}

int Queue::getSize() const
{
    return size;
}

void Queue::clear()
{
    while (!isEmpty())
    {
        dequeue();
    }
    
    front = 0;
    rear = -1;
    size = 0;
}
