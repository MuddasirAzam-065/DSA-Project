// matrix.h - 2D Matrix for Pixel Storage and Operations
#ifndef MATRIX_H
#define MATRIX_H

class Matrix
{
private:
    int** data;
    int rows;
    int cols;
    
    // Helper functions
    void allocate();
    void deallocate();
    
public:
    Matrix();
    Matrix(int r, int c);
    ~Matrix();
    
    // Dimension management
    void setDimensions(int r, int c);
    int getRows() const;
    int getCols() const;
    
    // Pixel operations
    int getPixel(int row, int col) const;
    void setPixel(int row, int col, int value);
    
    // Matrix operations
    Matrix add(const Matrix& other) const;
    Matrix subtract(const Matrix& other) const;
    Matrix multiply(const Matrix& other) const;
    Matrix transpose() const;
    Matrix copy() const;
    
    // Utility functions
    void fill(int value);
    void clear();
    bool isEmpty() const;
    void display();
};

#endif // MATRIX_H
