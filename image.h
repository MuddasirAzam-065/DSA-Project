// image.h - Image Processing and Huffman Compression
#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"
#include "tree.h"
#include "queue.h"

#define MAX_FILENAME 256
#define MAX_TREE_TABLE 256

struct HuffmanCode
{
    char code[256];  // Huffman code as string of 0s and 1s
    int length;      // Length of code
};

class Image
{
private:
    int width;
    int height;
    int maxPixelValue;
    char filename[MAX_FILENAME];
    bool isCompressed;
    
    Matrix pixelData;                           // Stores pixel values
    HuffmanCode huffmanTable[MAX_TREE_TABLE];   // Huffman codes for each pixel value
    int frequency[MAX_TREE_TABLE];              // Frequency of each pixel value
    Tree huffmanTree;                           // Huffman tree
    
    // Helper functions
    int clampPixel(int value);
    void buildFrequencyTable();
    void buildHuffmanTree();
    void generateHuffmanCodes();
    void generateCodesHelper(TreeNode* node, char* code, int depth);
    
public:
    Image();
    ~Image();
    
    // File I/O
    bool loadPPM(const char* filepath);
    bool savePPM(const char* filepath);
    bool saveCompressed(const char* filepath);
    bool loadCompressed(const char* filepath);
    
    // Getters
    int getWidth() const;
    int getHeight() const;
    int getMaxPixelValue() const;
    const char* getFilename() const;
    bool getIsCompressed() const;
    
    // Setters
    void setDimensions(int w, int h);
    void setMaxPixelValue(int val);
    void setPixel(int row, int col, int value);
    int getPixel(int row, int col) const;
    
    // Image operations
    void rgbToGrayscale();
    void edgeDetection();
    void imageSharpening();
    void imageScaling(int newWidth, int newHeight);
    void invertImage();
    
    // Compression operations
    void compressImage();
    void decompressImage();
    
    // Huffman related
    HuffmanCode* getHuffmanTable() const;
    Tree* getHuffmanTree();
    int* getFrequencyTable() const;
    
    // Utility
    void clear();
};

#endif // IMAGE_H
