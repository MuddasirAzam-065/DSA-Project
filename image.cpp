// image.cpp - Image Processing and Huffman Compression Implementation
#include "image.h"
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <queue>
#include <vector>

// Comparator for priority queue (for building Huffman tree)
struct CompareNode
{
    bool operator()(TreeNode* a, TreeNode* b)
    {
        return a->frequency > b->frequency;  // Min heap
    }
};

Image::Image()
{
    width = 0;
    height = 0;
    maxPixelValue = 255;
    filename[0] = '\0';
    isCompressed = false;
    
    for (int i = 0; i < MAX_TREE_TABLE; i++)
    {
        huffmanTable[i].code[0] = '\0';
        huffmanTable[i].length = 0;
        frequency[i] = 0;
    }
}

Image::~Image()
{
    clear();
}

int Image::clampPixel(int value)
{
    if (value < 0)
        return 0;
    if (value > 255)
        return 255;
    return value;
}

bool Image::loadPPM(const char* filepath)
{
    std::ifstream file(filepath, std::ios::binary);
    
    if (!file.is_open())
    {
        return false;
    }
    
    char magic[3];
    file.read(magic, 2);
    magic[2] = '\0';
    
    // Check if it's P3 (ASCII) or P6 (binary)
    if (strcmp(magic, "P3") != 0 && strcmp(magic, "P6") != 0)
    {
        file.close();
        return false;
    }
    
    // Skip whitespace and comments
    char c;
    while (file.get(c))
    {
        if (c == '#')
        {
            while (file.get(c) && c != '\n');
        }
        else if (!isspace(c))
        {
            file.unget();
            break;
        }
    }
    
    // Read width and height
    file >> width >> height >> maxPixelValue;
    file.get();  // Skip whitespace
    
    pixelData.setDimensions(height, width * 3);  // RGB has 3 channels
    
    strncpy(filename, filepath, MAX_FILENAME - 1);
    filename[MAX_FILENAME - 1] = '\0';
    isCompressed = false;
    
    if (strcmp(magic, "P3") == 0)
    {
        // ASCII PPM
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width * 3; j++)
            {
                int pixel;
                file >> pixel;
                pixelData.setPixel(i, j, pixel);
            }
        }
    }
    else
    {
        // Binary PPM (P6)
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width * 3; j++)
            {
                unsigned char pixel;
                file.read((char*)&pixel, 1);
                pixelData.setPixel(i, j, (int)pixel);
            }
        }
    }
    
    file.close();
    buildFrequencyTable();
    return true;
}

bool Image::savePPM(const char* filepath)
{
    std::ofstream file(filepath, std::ios::binary);
    
    if (!file.is_open())
    {
        return false;
    }
    
    // Write P6 (binary) PPM header
    file << "P6\n";
    file << width << " " << height << "\n";
    file << maxPixelValue << "\n";
    
    // Write pixel data
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width * 3; j++)
        {
            unsigned char pixel = (unsigned char)clampPixel(pixelData.getPixel(i, j));
            file.write((char*)&pixel, 1);
        }
    }
    
    file.close();
    return true;
}

bool Image::saveCompressed(const char* filepath)
{
    std::ofstream file(filepath, std::ios::binary);
    
    if (!file.is_open())
    {
        return false;
    }
    
    // Write custom header to indicate compressed file
    file << "HUFFMAN_COMPRESSED\n";
    file << width << " " << height << "\n";
    file << maxPixelValue << "\n";
    
    // Write frequency table
    for (int i = 0; i < 256; i++)
    {
        file.write((char*)&frequency[i], sizeof(int));
    }
    
    // Write Huffman codes table
    for (int i = 0; i < 256; i++)
    {
        int len = huffmanTable[i].length;
        file.write((char*)&len, sizeof(int));
        if (len > 0)
        {
            file.write(huffmanTable[i].code, len);
        }
    }
    
    // Encode and write pixel data using Huffman codes
    std::vector<bool> bitstream;
    
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width * 3; j++)
        {
            int pixelValue = pixelData.getPixel(i, j);
            const char* code = huffmanTable[pixelValue].code;
            int codeLen = huffmanTable[pixelValue].length;
            
            for (int k = 0; k < codeLen; k++)
            {
                bitstream.push_back(code[k] == '1');
            }
        }
    }
    
    // Write bitstream length
    int bitstreamLen = (int)bitstream.size();
    file.write((char*)&bitstreamLen, sizeof(int));
    
    // Write bitstream as bytes
    for (size_t i = 0; i < bitstream.size(); i += 8)
    {
        unsigned char byte = 0;
        for (int j = 0; j < 8 && (size_t)(i + j) < bitstream.size(); j++)
        {
            if (bitstream[i + j])
                byte |= (1 << (7 - j));
        }
        file.write((char*)&byte, 1);
    }
    
    file.close();
    isCompressed = true;
    return true;
}

bool Image::loadCompressed(const char* filepath)
{
    std::ifstream file(filepath, std::ios::binary);
    
    if (!file.is_open())
    {
        return false;
    }
    
    char header[20];
    file.getline(header, 20);
    
    if (strcmp(header, "HUFFMAN_COMPRESSED") != 0)
    {
        file.close();
        return false;
    }
    
    file >> width >> height >> maxPixelValue;
    file.get();
    
    pixelData.setDimensions(height, width * 3);
    
    // Read frequency table
    for (int i = 0; i < 256; i++)
    {
        file.read((char*)&frequency[i], sizeof(int));
    }
    
    // Read Huffman codes table
    for (int i = 0; i < 256; i++)
    {
        int len;
        file.read((char*)&len, sizeof(int));
        huffmanTable[i].length = len;
        if (len > 0)
        {
            file.read(huffmanTable[i].code, len);
            huffmanTable[i].code[len] = '\0';
        }
        else
        {
            huffmanTable[i].code[0] = '\0';
        }
    }
    
    // Read and decode bitstream
    int bitstreamLen;
    file.read((char*)&bitstreamLen, sizeof(int));
    
    std::vector<bool> bitstream;
    for (int i = 0; i < (bitstreamLen + 7) / 8; i++)
    {
        unsigned char byte;
        file.read((char*)&byte, 1);
        for (int j = 0; j < 8 && (int)bitstream.size() < bitstreamLen; j++)
        {
            bitstream.push_back((byte >> (7 - j)) & 1);
        }
    }
    
    // Rebuild Huffman tree from frequency table
    buildHuffmanTree();
    
    // Decode bitstream back to pixels
    TreeNode* current = huffmanTree.getRoot();
    int pixelIndex = 0;
    
    for (size_t bit_idx = 0; bit_idx < bitstream.size(); bit_idx++)
    {
        if (current == nullptr)
            break;
            
        if (bitstream[bit_idx])
            current = current->right;
        else
            current = current->left;
        
        // Leaf node - store pixel value
        if (current != nullptr && current->left == nullptr && current->right == nullptr)
        {
            if (pixelIndex < height * width * 3)
            {
                int row = pixelIndex / (width * 3);
                int col = pixelIndex % (width * 3);
                pixelData.setPixel(row, col, (int)(unsigned char)current->data);
                pixelIndex++;
            }
            current = huffmanTree.getRoot();
        }
    }
    
    strncpy(filename, filepath, MAX_FILENAME - 1);
    filename[MAX_FILENAME - 1] = '\0';
    isCompressed = true;
    file.close();
    return true;
}

int Image::getWidth() const { return width; }
int Image::getHeight() const { return height; }
int Image::getMaxPixelValue() const { return maxPixelValue; }
const char* Image::getFilename() const { return filename; }
bool Image::getIsCompressed() const { return isCompressed; }

void Image::setDimensions(int w, int h)
{
    width = w;
    height = h;
    pixelData.setDimensions(h, w * 3);
}

void Image::setMaxPixelValue(int val)
{
    maxPixelValue = val;
}

int Image::getPixel(int row, int col) const
{
    return pixelData.getPixel(row, col);
}

void Image::setPixel(int row, int col, int value)
{
    pixelData.setPixel(row, col, clampPixel(value));
}

void Image::rgbToGrayscale()
{
    if (width == 0 || height == 0)
        return;
    
    Matrix grayData(height, width);
    
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int r = pixelData.getPixel(i, j * 3);
            int g = pixelData.getPixel(i, j * 3 + 1);
            int b = pixelData.getPixel(i, j * 3 + 2);
            
            int gray = (r * 299 + g * 587 + b * 114) / 1000;
            grayData.setPixel(i, j, clampPixel(gray));
        }
    }
    
    // Store grayscale back to pixelData
    pixelData.setDimensions(height, width);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            pixelData.setPixel(i, j, grayData.getPixel(i, j));
        }
    }
}

void Image::edgeDetection()
{
    if (width == 0 || height == 0)
        return;
    
    // Sobel edge detection
    Matrix edges = pixelData.copy();
    
    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < pixelData.getCols() - 1; j++)
        {
            int gx = -pixelData.getPixel(i-1, j-1) - 2*pixelData.getPixel(i, j-1) - pixelData.getPixel(i+1, j-1)
                     + pixelData.getPixel(i-1, j+1) + 2*pixelData.getPixel(i, j+1) + pixelData.getPixel(i+1, j+1);
            
            int gy = -pixelData.getPixel(i-1, j-1) - 2*pixelData.getPixel(i-1, j) - pixelData.getPixel(i-1, j+1)
                     + pixelData.getPixel(i+1, j-1) + 2*pixelData.getPixel(i+1, j) + pixelData.getPixel(i+1, j+1);
            
            int magnitude = (int)sqrt(gx*gx + gy*gy);
            edges.setPixel(i, j, clampPixel(magnitude));
        }
    }
    
    pixelData = edges.copy();
}

void Image::imageSharpening()
{
    if (width == 0 || height == 0)
        return;
    
    // Unsharp mask sharpening
    Matrix sharpened = pixelData.copy();
    
    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < pixelData.getCols() - 1; j++)
        {
            int blur = (pixelData.getPixel(i-1, j-1) + pixelData.getPixel(i-1, j) + pixelData.getPixel(i-1, j+1)
                       + pixelData.getPixel(i, j-1) + pixelData.getPixel(i, j) + pixelData.getPixel(i, j+1)
                       + pixelData.getPixel(i+1, j-1) + pixelData.getPixel(i+1, j) + pixelData.getPixel(i+1, j+1)) / 9;
            
            int sharp = pixelData.getPixel(i, j) + (pixelData.getPixel(i, j) - blur);
            sharpened.setPixel(i, j, clampPixel(sharp));
        }
    }
    
    pixelData = sharpened.copy();
}

void Image::imageScaling(int newWidth, int newHeight)
{
    if (newWidth <= 0 || newHeight <= 0 || width == 0 || height == 0)
        return;
    
    Matrix scaled(newHeight, newWidth * 3);
    
    for (int i = 0; i < newHeight; i++)
    {
        for (int j = 0; j < newWidth * 3; j++)
        {
            int oldI = (i * height) / newHeight;
            int oldJ = (j * pixelData.getCols()) / (newWidth * 3);
            
            oldI = (oldI >= height) ? height - 1 : oldI;
            oldJ = (oldJ >= pixelData.getCols()) ? pixelData.getCols() - 1 : oldJ;
            
            scaled.setPixel(i, j, pixelData.getPixel(oldI, oldJ));
        }
    }
    
    width = newWidth;
    height = newHeight;
    pixelData = scaled.copy();
}

void Image::invertImage()
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < pixelData.getCols(); j++)
        {
            int inverted = 255 - pixelData.getPixel(i, j);
            pixelData.setPixel(i, j, inverted);
        }
    }
}

void Image::buildFrequencyTable()
{
    for (int i = 0; i < 256; i++)
        frequency[i] = 0;
    
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < pixelData.getCols(); j++)
        {
            int pixel = pixelData.getPixel(i, j);
            if (pixel >= 0 && pixel < 256)
                frequency[pixel]++;
        }
    }
}

void Image::buildHuffmanTree()
{
    // Create priority queue of tree nodes
    std::priority_queue<TreeNode*, std::vector<TreeNode*>, CompareNode> pq;
    
    // Insert all pixel values with frequency > 0
    for (int i = 0; i < 256; i++)
    {
        if (frequency[i] > 0)
        {
            TreeNode* node = new TreeNode((char)i, frequency[i]);
            pq.push(node);
        }
    }
    
    // Build tree by combining nodes
    while (pq.size() > 1)
    {
        TreeNode* left = pq.top();
        pq.pop();
        TreeNode* right = pq.top();
        pq.pop();
        
        // Create parent node
        TreeNode* parent = new TreeNode('\0', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        
        pq.push(parent);
    }
    
    // Set root
    if (!pq.empty())
    {
        huffmanTree.setRoot(pq.top());
    }
}

void Image::generateCodesHelper(TreeNode* node, char* code, int depth)
{
    if (node == nullptr)
        return;
    
    // Leaf node - store the code
    if (node->left == nullptr && node->right == nullptr)
    {
        int pixelValue = (unsigned char)node->data;
        strncpy(huffmanTable[pixelValue].code, code, 255);
        huffmanTable[pixelValue].code[depth] = '\0';
        huffmanTable[pixelValue].length = depth;
        return;
    }
    
    // Traverse left (add '0')
    code[depth] = '0';
    generateCodesHelper(node->left, code, depth + 1);
    
    // Traverse right (add '1')
    code[depth] = '1';
    generateCodesHelper(node->right, code, depth + 1);
}

void Image::generateHuffmanCodes()
{
    // Clear previous codes
    for (int i = 0; i < 256; i++)
    {
        huffmanTable[i].code[0] = '\0';
        huffmanTable[i].length = 0;
    }
    
    char code[256];
    generateCodesHelper(huffmanTree.getRoot(), code, 0);
}

void Image::compressImage()
{
    buildFrequencyTable();
    buildHuffmanTree();
    generateHuffmanCodes();
}

void Image::decompressImage()
{
    // Decompress image using Huffman tree
}

HuffmanCode* Image::getHuffmanTable() const
{
    return (HuffmanCode*)huffmanTable;
}

Tree* Image::getHuffmanTree()
{
    return &huffmanTree;
}

int* Image::getFrequencyTable() const
{
    return (int*)frequency;
}

void Image::clear()
{
    pixelData.clear();
    huffmanTree.clear();
    width = 0;
    height = 0;
    isCompressed = false;
    
    for (int i = 0; i < 256; i++)
    {
        huffmanTable[i].code[0] = '\0';
        huffmanTable[i].length = 0;
        frequency[i] = 0;
    }
}
