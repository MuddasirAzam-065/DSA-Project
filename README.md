# Image Compression System (Huffman Encoding)

## 📌 Project Overview
This project is a C++ desktop application that compresses image files using **Huffman Encoding**, a lossless data compression algorithm. The system reduces image size by assigning shorter binary codes to frequently occurring pixel values and longer codes to less frequent ones.

The application works on **PPM image format (P3 and P6)** and provides a graphical interface built using **Qt**, including Huffman tree visualization and frequency tables.

---

## 🎯 Features
- Huffman Encoding & Decoding for images
- Lossless image compression
- Support for PPM formats (ASCII P3 and Binary P6)
- Huffman Tree visualization
- Frequency table display
- Image processing operations:
  - RGB to Grayscale
  - Edge Detection
  - Sharpening
  - Scaling
  - Invert Colors
- GUI-based interaction using Qt

---

## 🧠 Algorithm Used
### Huffman Encoding
The algorithm follows these steps:
1. Count frequency of each pixel value
2. Build a priority queue (min-heap)
3. Construct Huffman Tree by merging lowest frequency nodes
4. Assign binary codes (left = 0, right = 1)
5. Encode image using generated codes

---

## 🏗️ System Architecture

The system follows a layered architecture:

- **Presentation Layer:** Qt GUI (MainWindow)
- **Logic Layer:** Image processing & Huffman encoding
- **Data Structure Layer:** Tree, Queue, Matrix

### Modules
| Module | File | Responsibility |
|--------|------|----------------|
| Main Window | `mainwindow.h/.cpp` | GUI and user interaction |
| Image | `image.h/.cpp` | Compression, processing, frequency analysis |
| Tree | `tree.h/.cpp` | Huffman tree construction |
| Queue | `queue.h/.cpp` | Priority queue implementation |
| Matrix | `matrix.h/.cpp` | Pixel storage |

---

## 🔄 Data Flow
1. Load PPM image (`loadPPM()`)
2. Build frequency table
3. Construct Huffman Tree
4. Generate binary codes
5. Encode image data
6. Save compressed output

<img width="478" height="1111" alt="flow" src="https://github.com/user-attachments/assets/06c77bbe-8095-4660-800a-0cf6c76ac911" />


---

## 🖥️ GUI Layout

### Interface Overview
- Top Panel: Load, Compress, Decompress, View Tree
- Middle Panel: Huffman Table + Visualization
- Bottom Panel: Image Processing Tools

📷 **Screenshot - Main Interface**
