#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QWheelEvent>
#include <QGraphicsProxyWidget>
#include <QPixmap>
#include <QImage>
#include <cstring>
#include <cstdio>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , zoomLevel(1.0)
{
    ui->setupUi(this);
    
    // Initialize graphics scene for tree display
    treeScene = new QGraphicsScene(this);
    ui->huffmantreeview->setScene(treeScene);
    ui->huffmantreeview->setRenderHint(QPainter::Antialiasing);
    
    // Enable mouse wheel zoom
    ui->huffmantreeview->setDragMode(QGraphicsView::ScrollHandDrag);
    
    // Connect button signals to slots
    connect(ui->loadfile, &QPushButton::clicked, this, &MainWindow::onLoadFileClicked);
    connect(ui->compress, &QPushButton::clicked, this, &MainWindow::onCompressClicked);
    connect(ui->decompress, &QPushButton::clicked, this, &MainWindow::onDecompressClicked);
    connect(ui->showHuffmanTableandTree, &QPushButton::clicked, this, &MainWindow::onShowHuffmanTableandTreeClicked);
    connect(ui->savecopies, &QPushButton::clicked, this, &MainWindow::onSavecopiesClicked);
    
    setWindowTitle("Image Compression - Huffman Encoding");
    
    // Add context menu for export
    ui->huffmantreeview->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->huffmantreeview, &QWidget::customContextMenuRequested, this, &MainWindow::showTreeContextMenu);
    ui->huffmantablebiew->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->huffmantablebiew, &QWidget::customContextMenuRequested, this, &MainWindow::showTableContextMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
    imageProcessor.clear();
}

void MainWindow::onLoadFileClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open PPM File"), "",
        tr("PPM Images (*.ppm);;All Files (*)"));
    
    if (filePath.isEmpty())
        return;
    
    // Load the image
    bool success = imageProcessor.loadPPM(filePath.toStdString().c_str());
    
    if (success)
    {
        QMessageBox::information(this, "Success",
            QString("Image loaded successfully!\n\n") +
            QString("Dimensions: %1 x %2\n").arg(imageProcessor.getWidth()).arg(imageProcessor.getHeight()) +
            QString("Max Pixel Value: %1").arg(imageProcessor.getMaxPixelValue()));
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to load PPM file!");
    }
}

void MainWindow::onCompressClicked()
{
    if (imageProcessor.getWidth() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please load an image first!");
        return;
    }
    
    // Perform compression
    imageProcessor.compressImage();
    
    // Generate output filename
    const char* originalFile = imageProcessor.getFilename();
    char compressedFile[256];
    
    // Extract filename without extension
    char baseFilename[256];
    strncpy(baseFilename, originalFile, sizeof(baseFilename) - 1);
    baseFilename[sizeof(baseFilename) - 1] = '\0';
    
    // Remove .ppm extension
    size_t pos = strlen(baseFilename);
    if (pos > 4 && strcmp(baseFilename + pos - 4, ".ppm") == 0)
    {
        baseFilename[pos - 4] = '\0';
    }
    
    // Create compressed filename
    snprintf(compressedFile, sizeof(compressedFile), "%s_compressed.huff", baseFilename);
    
    // Save compressed file
    bool success = imageProcessor.saveCompressed(compressedFile);
    
    if (success)
    {
        QMessageBox::information(this, "Success",
            QString("Image compressed successfully!\n\nSaved as: %1").arg(compressedFile));
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to compress image!");
    }
}

void MainWindow::onDecompressClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open Compressed File"), "",
        tr("Huffman Files (*.huff);;All Files (*)"));
    
    if (filePath.isEmpty())
        return;
    
    // Load compressed file
    bool success = imageProcessor.loadCompressed(filePath.toStdString().c_str());
    
    if (success)
    {
        // Generate output filename
        char decompressedFile[256];
        char baseFilename[256];
        strncpy(baseFilename, filePath.toStdString().c_str(), sizeof(baseFilename) - 1);
        baseFilename[sizeof(baseFilename) - 1] = '\0';
        
        // Remove .huff extension
        size_t pos = strlen(baseFilename);
        if (pos > 5 && strcmp(baseFilename + pos - 5, ".huff") == 0)
        {
            baseFilename[pos - 5] = '\0';
        }
        
        // Create decompressed filename
        snprintf(decompressedFile, sizeof(decompressedFile), "%s_decompressed.ppm", baseFilename);
        
        // Save decompressed file
        imageProcessor.savePPM(decompressedFile);
        
        QMessageBox::information(this, "Success",
            QString("Image decompressed successfully!\n\nSaved as: %1").arg(decompressedFile));
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to load compressed file!");
    }
}

void MainWindow::onShowHuffmanTableandTreeClicked()
{
    if (imageProcessor.getWidth() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please load and compress an image first!");
        return;
    }
    
    // Display Huffman table
    displayHuffmanTable();
    
    // Display tree structure
    displayHuffmanTree();
}

void MainWindow::displayHuffmanTable()
{
    // Clear existing table
    ui->huffmantablebiew->setRowCount(0);
    ui->huffmantablebiew->setColumnCount(3);
    
    // Set column headers
    QStringList headers;
    headers << "Pixel Value" << "Frequency" << "Huffman Code";
    ui->huffmantablebiew->setHorizontalHeaderLabels(headers);
    
    // Get frequency table
    int* frequencyTable = imageProcessor.getFrequencyTable();
    HuffmanCode* huffmanTable = imageProcessor.getHuffmanTable();
    
    int row = 0;
    
    // Add rows for each pixel value that exists in the image
    for (int i = 0; i < 256; i++)
    {
        if (frequencyTable[i] > 0)
        {
            ui->huffmantablebiew->insertRow(row);
            
            // Pixel Value
            QTableWidgetItem *pixelItem = new QTableWidgetItem(QString::number(i));
            ui->huffmantablebiew->setItem(row, 0, pixelItem);
            
            // Frequency
            QTableWidgetItem *freqItem = new QTableWidgetItem(QString::number(frequencyTable[i]));
            ui->huffmantablebiew->setItem(row, 1, freqItem);
            
            // Huffman Code
            QString code = QString::fromUtf8(huffmanTable[i].code);
            QTableWidgetItem *codeItem = new QTableWidgetItem(code);
            ui->huffmantablebiew->setItem(row, 2, codeItem);
            
            row++;
        }
    }
    
    // Resize columns to content
    ui->huffmantablebiew->resizeColumnsToContents();
}

int MainWindow::buildSubtreeWidthMap(TreeNode* node, QHash<TreeNode*, int>& widthMap)
{
    if (node == nullptr)
        return 0;

    int leftWidth = buildSubtreeWidthMap(node->left, widthMap);
    int rightWidth = buildSubtreeWidthMap(node->right, widthMap);
    int width = leftWidth + rightWidth;

    if (width == 0)
        width = 1;

    widthMap.insert(node, width);
    return width;
}

int MainWindow::getTreeDepth(TreeNode* node)
{
    if (node == nullptr)
        return 0;

    int leftDepth = getTreeDepth(node->left);
    int rightDepth = getTreeDepth(node->right);
    return 1 + ((leftDepth > rightDepth) ? leftDepth : rightDepth);
}

void MainWindow::drawTreeNode(TreeNode* node, QGraphicsScene* scene, double x, double y,
                              const QHash<TreeNode*, int>& widthMap, double leafSpacing, double levelHeight)
{
    if (node == nullptr)
        return;
    
    // Draw current node
    QGraphicsEllipseItem* circle = scene->addEllipse(x - 15, y - 15, 30, 30);
    circle->setBrush(Qt::yellow);
    circle->setPen(QPen(Qt::black, 2));
    
    // Add frequency text
    if (node->frequency > 0)
    {
        QGraphicsTextItem* text = scene->addText(QString::number(node->frequency));
        text->setDefaultTextColor(Qt::black);

        QRectF textBounds = text->boundingRect();
        text->setX(x - (textBounds.width() / 2.0));
        text->setY(y - (textBounds.height() / 2.0));
    }
    
    // Recursively draw children with subtree-aware spacing
    int leftLeaves = (node->left != nullptr) ? widthMap.value(node->left, 1) : 0;
    int rightLeaves = (node->right != nullptr) ? widthMap.value(node->right, 1) : 0;
    int totalLeaves = leftLeaves + rightLeaves;
    double subtreeWidth = totalLeaves * leafSpacing;

    if (node->left != nullptr)
    {
        double leftSpan = leftLeaves * leafSpacing;
        double leftX = x - (subtreeWidth / 2.0) + (leftSpan / 2.0);
        scene->addLine(x, y + 15, leftX, y + levelHeight - 15, QPen(Qt::black, 2));
        drawTreeNode(node->left, scene, leftX, y + levelHeight, widthMap, leafSpacing, levelHeight);
    }
    
    if (node->right != nullptr)
    {
        double rightSpan = rightLeaves * leafSpacing;
        double rightX = x + (subtreeWidth / 2.0) - (rightSpan / 2.0);
        scene->addLine(x, y + 15, rightX, y + levelHeight - 15, QPen(Qt::black, 2));
        drawTreeNode(node->right, scene, rightX, y + levelHeight, widthMap, leafSpacing, levelHeight);
    }
}

void MainWindow::displayHuffmanTree()
{
    // Clear previous scene
    treeScene->clear();
    zoomLevel = 1.0;
    
    TreeNode* root = imageProcessor.getHuffmanTree()->getRoot();
    
    if (root == nullptr)
    {
        QGraphicsTextItem *text = treeScene->addText("No Huffman tree generated. Please load an image and click 'Show HuffmanTable and Tree'.");
        text->setFont(QFont("Arial", 12));
        return;
    }
    
    QHash<TreeNode*, int> widthMap;
    int leafCount = buildSubtreeWidthMap(root, widthMap);
    int treeDepth = getTreeDepth(root);

    const double leafSpacing = 60.0;
    const double levelHeight = 90.0;
    const double margin = 80.0;
    const double sceneWidth = (leafCount * leafSpacing) + (margin * 2.0);
    const double sceneHeight = (treeDepth * levelHeight) + 140.0;

    treeScene->setSceneRect(0, 0, sceneWidth, sceneHeight);

    // Draw the tree
    double rootX = sceneWidth / 2.0;
    drawTreeNode(root, treeScene, rootX, 70.0, widthMap, leafSpacing, levelHeight);
    
    // Add zoom info
    QGraphicsTextItem *infoText = treeScene->addText("Scroll to zoom | Drag to pan | Right-click to export");
    infoText->setX(20);
    infoText->setY(20);
    infoText->setFont(QFont("Arial", 10));
}

void MainWindow::wheelEvent(QWheelEvent* event)
{
    if (ui->huffmantreeview->geometry().contains(event->position().toPoint()))
    {
        double zoomFactor = 1.1;
        if (event->angleDelta().y() > 0)
            zoomLevel *= zoomFactor;
        else
            zoomLevel /= zoomFactor;
        
        ui->huffmantreeview->scale(zoomFactor, zoomFactor);
        event->accept();
    }
    else
    {
        QMainWindow::wheelEvent(event);
    }
}

void MainWindow::showTreeContextMenu(const QPoint& pos)
{
    QMenu menu;
    menu.addAction("Export Tree as PNG", this, &MainWindow::exportTreeAsPNG);
    menu.addAction("Export Tree as SVG", this, &MainWindow::exportTreeAsSVG);
    menu.exec(ui->huffmantreeview->mapToGlobal(pos));
}

void MainWindow::showTableContextMenu(const QPoint& pos)
{
    QMenu menu;
    menu.addAction("Export Table as CSV", this, &MainWindow::exportTableAsCSV);
    menu.addAction("Export Table as Text", this, &MainWindow::exportTableAsText);
    menu.exec(ui->huffmantablebiew->mapToGlobal(pos));
}

void MainWindow::exportTreeAsPNG()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Tree as PNG"), "", tr("PNG Image (*.png)"));

    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".png", Qt::CaseInsensitive))
        fileName += ".png";

    // Compute full bounds of all items so nothing is cropped
    QRectF bounds = treeScene->itemsBoundingRect();
    if (bounds.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "No tree to export. Generate the Huffman tree first.");
        return;
    }

    // Add padding around the drawing
    const qreal pad = 40.0;
    bounds.adjust(-pad, -pad, pad, pad);

    // Render at higher resolution (3x scale factor for crispness)
    const qreal scale = 3.0;
    QSize imgSize(qCeil(bounds.width() * scale), qCeil(bounds.height() * scale));

    QImage image(imgSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Map scene bounds directly into image bounds
    treeScene->render(&painter, QRectF(0, 0, imgSize.width(), imgSize.height()), bounds);

    if (image.save(fileName))
        QMessageBox::information(this, "Success", "Tree exported as high-quality PNG!");
    else
        QMessageBox::critical(this, "Error", "Failed to export tree!");
}

void MainWindow::exportTreeAsSVG()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Tree as SVG"), "", tr("SVG Image (*.svg)"));

    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".svg", Qt::CaseInsensitive))
        fileName += ".svg";

    QRectF bounds = treeScene->itemsBoundingRect();
    if (bounds.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "No tree to export. Generate the Huffman tree first.");
        return;
    }

    const qreal pad = 20.0;
    bounds.adjust(-pad, -pad, pad, pad);

    QSvgGenerator generator;
    generator.setFileName(fileName);
    generator.setViewBox(bounds.toRect());
    generator.setSize(bounds.size().toSize());
    generator.setTitle("Huffman Tree");
    generator.setDescription("Exported Huffman tree visualization");

    QPainter painter(&generator);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    treeScene->render(&painter, QRectF(), bounds);
    painter.end();

    QMessageBox::information(this, "Success", "Tree exported as SVG!");
}

void MainWindow::exportTableAsCSV()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Table as CSV"), "", tr("CSV File (*.csv)"));
    
    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith(".csv", Qt::CaseInsensitive))
            fileName += ".csv";

        std::ofstream file(fileName.toStdString());
        
        if (file.is_open())
        {
            // Write headers
            file << "Pixel Value,Frequency,Huffman Code\n";
            
            // Write data
            for (int row = 0; row < ui->huffmantablebiew->rowCount(); row++)
            {
                for (int col = 0; col < 3; col++)
                {
                    QTableWidgetItem* item = ui->huffmantablebiew->item(row, col);
                    file << (item ? item->text().toStdString() : "");
                    if (col < 2) file << ",";
                }
                file << "\n";
            }
            
            file.close();
            QMessageBox::information(this, "Success", "Table exported as CSV!");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Failed to open file for CSV export!");
        }
    }
}

void MainWindow::exportTableAsText()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Table as Text"), "", tr("Text File (*.txt)"));
    
    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith(".txt", Qt::CaseInsensitive))
            fileName += ".txt";

        std::ofstream file(fileName.toStdString());
        
        if (file.is_open())
        {
            // Write headers
            file << "Pixel Value | Frequency | Huffman Code\n";
            file << "============================================\n";
            
            // Write data
            for (int row = 0; row < ui->huffmantablebiew->rowCount(); row++)
            {
                QTableWidgetItem* pixelItem = ui->huffmantablebiew->item(row, 0);
                QTableWidgetItem* freqItem = ui->huffmantablebiew->item(row, 1);
                QTableWidgetItem* codeItem = ui->huffmantablebiew->item(row, 2);

                QString line = QString("%1 | %2 | %3\n")
                    .arg(pixelItem ? pixelItem->text() : "", 15)
                    .arg(freqItem ? freqItem->text() : "", 15)
                    .arg(codeItem ? codeItem->text() : "", 30);
                file << line.toStdString();
            }
            
            file.close();
            QMessageBox::information(this, "Success", "Table exported as Text!");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Failed to open file for text export!");
        }
    }
}

void MainWindow::onSavecopiesClicked()
{
    if (imageProcessor.getWidth() == 0)
    {
        QMessageBox::warning(this, "Warning", "Please load an image first!");
        return;
    }
    
    int operationsCount = 0;
    
    // RGB to Grayscale
    if (ui->rgbtograyscale->isChecked())
    {
        Image tempImage = imageProcessor;
        tempImage.rgbToGrayscale();
        
        const char* originalFile = imageProcessor.getFilename();
        char outputFile[256];
        char baseFilename[256];
        strncpy(baseFilename, originalFile, sizeof(baseFilename) - 1);
        baseFilename[sizeof(baseFilename) - 1] = '\0';
        
        size_t pos = strlen(baseFilename);
        if (pos > 4 && strcmp(baseFilename + pos - 4, ".ppm") == 0)
        {
            baseFilename[pos - 4] = '\0';
        }
        
        snprintf(outputFile, sizeof(outputFile), "%s_grayscale.ppm", baseFilename);
        tempImage.savePPM(outputFile);
        operationsCount++;
    }
    
    // Edge Detection
    if (ui->edgedetection->isChecked())
    {
        try
        {
            Image tempImage = imageProcessor;
            tempImage.edgeDetection();
            
            char outputFile[256];
            char baseFilename[256];
            strncpy(baseFilename, imageProcessor.getFilename(), sizeof(baseFilename) - 1);
            baseFilename[sizeof(baseFilename) - 1] = '\0';
            
            size_t pos = strlen(baseFilename);
            if (pos > 4 && strcmp(baseFilename + pos - 4, ".ppm") == 0)
            {
                baseFilename[pos - 4] = '\0';
            }
            
            snprintf(outputFile, sizeof(outputFile), "%s_edge_detected.ppm", baseFilename);
            tempImage.savePPM(outputFile);
            operationsCount++;
        }
        catch (...)
        {
            QMessageBox::warning(this, "Warning", "Edge Detection failed!");
        }
    }
    
    // Image Sharpening
    if (ui->imagesharpening->isChecked())
    {
        try
        {
            Image tempImage = imageProcessor;
            tempImage.imageSharpening();
            
            char outputFile[256];
            char baseFilename[256];
            strncpy(baseFilename, imageProcessor.getFilename(), sizeof(baseFilename) - 1);
            baseFilename[sizeof(baseFilename) - 1] = '\0';
            
            size_t pos = strlen(baseFilename);
            if (pos > 4 && strcmp(baseFilename + pos - 4, ".ppm") == 0)
            {
                baseFilename[pos - 4] = '\0';
            }
            
            snprintf(outputFile, sizeof(outputFile), "%s_sharpened.ppm", baseFilename);
            tempImage.savePPM(outputFile);
            operationsCount++;
        }
        catch (...)
        {
            QMessageBox::warning(this, "Warning", "Image Sharpening failed!");
        }
    }
    
    // Image Scaling
    if (ui->imagescaling->isChecked())
    {
        try
        {
            Image tempImage = imageProcessor;
            int newWidth = tempImage.getWidth() / 2;
            int newHeight = tempImage.getHeight() / 2;
            tempImage.imageScaling(newWidth, newHeight);
            
            char outputFile[256];
            char baseFilename[256];
            strncpy(baseFilename, imageProcessor.getFilename(), sizeof(baseFilename) - 1);
            baseFilename[sizeof(baseFilename) - 1] = '\0';
            
            size_t pos = strlen(baseFilename);
            if (pos > 4 && strcmp(baseFilename + pos - 4, ".ppm") == 0)
            {
                baseFilename[pos - 4] = '\0';
            }
            
            snprintf(outputFile, sizeof(outputFile), "%s_scaled.ppm", baseFilename);
            tempImage.savePPM(outputFile);
            operationsCount++;
        }
        catch (...)
        {
            QMessageBox::warning(this, "Warning", "Image Scaling failed!");
        }
    }
    
    // Invert Colors
    if (ui->invert->isChecked())
    {
        Image tempImage = imageProcessor;
        tempImage.invertImage();
        
        char outputFile[256];
        char baseFilename[256];
        strncpy(baseFilename, imageProcessor.getFilename(), sizeof(baseFilename) - 1);
        baseFilename[sizeof(baseFilename) - 1] = '\0';
        
        size_t pos = strlen(baseFilename);
        if (pos > 4 && strcmp(baseFilename + pos - 4, ".ppm") == 0)
        {
            baseFilename[pos - 4] = '\0';
        }
        
        snprintf(outputFile, sizeof(outputFile), "%s_inverted.ppm", baseFilename);
        tempImage.savePPM(outputFile);
        operationsCount++;
    }
    
    if (operationsCount == 0)
    {
        QMessageBox::warning(this, "Warning", "Please select at least one operation!");
        return;
    }
    
    QMessageBox::information(this, "Success",
        QString("Saved %1 image copy(ies) with selected operations!").arg(operationsCount));
}
