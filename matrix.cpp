// matrix.cpp - 2D Matrix Implementation for Pixel Storage
#include "matrix.h"
#include <iostream>
#include <cstring>

Matrix::Matrix() : data(nullptr), rows(0), cols(0)
{
}

Matrix::Matrix(int r, int c) : rows(r), cols(c)
{
    allocate();
    fill(0);
}

Matrix::~Matrix()
{
    deallocate();
}

void Matrix::allocate()
{
    if (rows <= 0 || cols <= 0)
    {
        data = nullptr;
        return;
    }
    
    data = new int*[rows];
    for (int i = 0; i < rows; i++)
    {
        data[i] = new int[cols];
    }
}

void Matrix::deallocate()
{
    if (data == nullptr)
        return;
    
    for (int i = 0; i < rows; i++)
    {
        delete[] data[i];
    }
    delete[] data;
    data = nullptr;
    rows = 0;
    cols = 0;
}

void Matrix::setDimensions(int r, int c)
{
    if (rows == r && cols == c)
        return;
    
    deallocate();
    rows = r;
    cols = c;
    allocate();
    fill(0);
}

int Matrix::getRows() const
{
    return rows;
}

int Matrix::getCols() const
{
    return cols;
}

int Matrix::getPixel(int row, int col) const
{
    if (data == nullptr || row < 0 || row >= rows || col < 0 || col >= cols)
        return 0;
    
    return data[row][col];
}

void Matrix::setPixel(int row, int col, int value)
{
    if (data == nullptr || row < 0 || row >= rows || col < 0 || col >= cols)
        return;
    
    data[row][col] = value;
}

Matrix Matrix::add(const Matrix& other) const
{
    if (rows != other.rows || cols != other.cols)
    {
        return Matrix();
    }
    
    Matrix result(rows, cols);
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int sum = data[i][j] + other.data[i][j];
            result.setPixel(i, j, sum);
        }
    }
    
    return result;
}

Matrix Matrix::subtract(const Matrix& other) const
{
    if (rows != other.rows || cols != other.cols)
    {
        return Matrix();
    }
    
    Matrix result(rows, cols);
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int diff = data[i][j] - other.data[i][j];
            result.setPixel(i, j, diff);
        }
    }
    
    return result;
}

Matrix Matrix::multiply(const Matrix& other) const
{
    if (cols != other.rows)
    {
        return Matrix();
    }
    
    Matrix result(rows, other.cols);
    result.fill(0);
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < other.cols; j++)
        {
            int sum = 0;
            for (int k = 0; k < cols; k++)
            {
                sum += data[i][k] * other.data[k][j];
            }
            result.setPixel(i, j, sum);
        }
    }
    
    return result;
}

Matrix Matrix::transpose() const
{
    Matrix result(cols, rows);
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result.setPixel(j, i, data[i][j]);
        }
    }
    
    return result;
}

Matrix Matrix::copy() const
{
    Matrix result(rows, cols);
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            result.setPixel(i, j, data[i][j]);
        }
    }
    
    return result;
}

void Matrix::fill(int value)
{
    if (data == nullptr)
        return;
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            data[i][j] = value;
        }
    }
}

void Matrix::clear()
{
    deallocate();
}

bool Matrix::isEmpty() const
{
    return data == nullptr || rows == 0 || cols == 0;
}

void Matrix::display()
{
    if (isEmpty())
    {
        return;
    }
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            // Print pixel value
        }
    }
}
