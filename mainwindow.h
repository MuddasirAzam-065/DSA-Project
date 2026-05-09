#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QPainter>
#include <QSvgGenerator>
#include <QHash>
#include "image.h"
#include "tree.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
protected:
    void wheelEvent(QWheelEvent* event) override;

private slots:
    // Button click handlers
    void onLoadFileClicked();
    void onCompressClicked();
    void onDecompressClicked();
    void onShowHuffmanTableandTreeClicked();
    void onSavecopiesClicked();
    
    // Tree export
    void exportTreeAsPNG();
    void exportTreeAsSVG();
    void showTreeContextMenu(const QPoint& pos);
    
    // Table export
    void exportTableAsCSV();
    void exportTableAsText();
    void showTableContextMenu(const QPoint& pos);

private:
    // Helper functions
    void displayHuffmanTable();
    void displayHuffmanTree();
    int buildSubtreeWidthMap(TreeNode* node, QHash<TreeNode*, int>& widthMap);
    int getTreeDepth(TreeNode* node);
    void drawTreeNode(TreeNode* node, QGraphicsScene* scene, double x, double y,
                      const QHash<TreeNode*, int>& widthMap, double leafSpacing, double levelHeight);

    Ui::MainWindow *ui;
    Image imageProcessor;
    QGraphicsScene *treeScene;
    double zoomLevel;
};

#endif // MAINWINDOW_H
