#include "mainwindow.h"

#include <QAction>
#include <QActionGroup>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QStyle>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Qt Image Editor");
    resize(1024, 720);

    setupCentralWorkspace();
    createActions();
    createMenus();
    createToolBars();
    updateActionStates();

    statusBar()->showMessage("就緒 — 請開啟一張圖片開始編輯");
}

void MainWindow::setupCentralWorkspace()
{
    graphicsScene = new QGraphicsScene(this);
    graphicsView = new QGraphicsView(graphicsScene, this);

    // 畫布基礎設定：抗鋸齒與深灰色背景，凸顯圖片邊界
    graphicsView->setRenderHint(QPainter::Antialiasing, true);
    graphicsView->setRenderHint(QPainter::SmoothPixmapTransform, true);
    graphicsView->setBackgroundBrush(QBrush(QColor(45, 45, 48)));
    graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    setCentralWidget(graphicsView);
}

void MainWindow::createActions()
{
    // --- File 類別 ---
    openAct = new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton), "開啟(&O)...", this);
    openAct->setShortcut(QKeySequence::Open); // Ctrl+O
    openAct->setStatusTip("從本機開啟影像檔案");
    connect(openAct, &QAction::triggered, this, &MainWindow::openImage);

    saveAct = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "儲存(&S)...", this);
    saveAct->setShortcut(QKeySequence::Save); // Ctrl+S
    saveAct->setStatusTip("將目前影像另存新檔");
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveImage);

    exitAct = new QAction(style()->standardIcon(QStyle::SP_DialogCloseButton), "離開(&X)", this);
    exitAct->setShortcut(QKeySequence::Quit); // Ctrl+Q
    exitAct->setStatusTip("關閉應用程式");
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    // --- Edit / 工具模式（使用 QActionGroup 確保單選互斥） ---
    panToolAct = new QAction(style()->standardIcon(QStyle::SP_ArrowRight), "檢視/移動(&V)", this);
    panToolAct->setShortcut(QKeySequence(Qt::Key_V));
    panToolAct->setCheckable(true);
    panToolAct->setChecked(true);
    panToolAct->setStatusTip("切換至畫布移動模式");

    brushToolAct = new QAction(style()->standardIcon(QStyle::SP_FileDialogDetailedView), "畫筆(&B)", this);
    brushToolAct->setShortcut(QKeySequence(Qt::Key_B));
    brushToolAct->setCheckable(true);
    brushToolAct->setStatusTip("切換至畫筆繪圖模式（待實作）");

    cropToolAct = new QAction(style()->standardIcon(QStyle::SP_TitleBarMaxButton), "裁切(&C)", this);
    cropToolAct->setShortcut(QKeySequence(Qt::Key_C));
    cropToolAct->setCheckable(true);
    cropToolAct->setStatusTip("切換至影像裁切模式（待實作）");

    toolGroup = new QActionGroup(this);
    toolGroup->addAction(panToolAct);
    toolGroup->addAction(brushToolAct);
    toolGroup->addAction(cropToolAct);
    toolGroup->setExclusive(true);
    connect(toolGroup, &QActionGroup::triggered, this, &MainWindow::onToolSelected);

    // --- Image 類別 ---
    flipHAct = new QAction(style()->standardIcon(QStyle::SP_ArrowLeft), "水平翻轉(&H)", this);
    flipHAct->setShortcut(QKeySequence("Ctrl+Shift+H"));
    flipHAct->setStatusTip("左右鏡像翻轉影像（待實作）");
    connect(flipHAct, &QAction::triggered, this, &MainWindow::onFlipHorizontal);

    flipVAct = new QAction(style()->standardIcon(QStyle::SP_ArrowUp), "垂直翻轉(&F)", this);
    flipVAct->setShortcut(QKeySequence("Ctrl+Shift+V"));
    flipVAct->setStatusTip("上下鏡像翻轉影像（待實作）");
    connect(flipVAct, &QAction::triggered, this, &MainWindow::onFlipVertical);
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("檔案(&F)");
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    QMenu *editMenu = menuBar()->addMenu("編輯(&E)");
    editMenu->addAction(panToolAct);
    editMenu->addAction(brushToolAct);

    QMenu *imageMenu = menuBar()->addMenu("影像(&I)");
    imageMenu->addAction(cropToolAct);
    imageMenu->addSeparator();
    imageMenu->addAction(flipHAct);
    imageMenu->addAction(flipVAct);
}

void MainWindow::createToolBars()
{
    QToolBar *mainToolBar = addToolBar("主要工具列");
    mainToolBar->setMovable(false);
    mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    mainToolBar->addAction(openAct);
    mainToolBar->addAction(saveAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(panToolAct);
    mainToolBar->addAction(brushToolAct);
    mainToolBar->addAction(cropToolAct);
    mainToolBar->addSeparator();
    mainToolBar->addAction(flipHAct);
    mainToolBar->addAction(flipVAct);
}

void MainWindow::updateActionStates()
{
    bool hasImage = !currentPixmap.isNull();
    saveAct->setEnabled(hasImage);
    brushToolAct->setEnabled(hasImage);
    cropToolAct->setEnabled(hasImage);
    flipHAct->setEnabled(hasImage);
    flipVAct->setEnabled(hasImage);
}

void MainWindow::openImage()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "開啟影像", QString(),
        "影像檔案 (*.png *.jpg *.jpeg *.bmp);;所有檔案 (*.*)");

    if (filePath.isEmpty())
        return;

    QPixmap loaded(filePath);
    if (loaded.isNull()) {
        QMessageBox::warning(this, "開啟失敗", "無法讀取所選的影像檔案。");
        return;
    }

    currentPixmap = loaded;
    graphicsScene->clear();
    pixmapItem = graphicsScene->addPixmap(currentPixmap);
    graphicsScene->setSceneRect(currentPixmap.rect());
    graphicsView->fitInView(pixmapItem, Qt::KeepAspectRatio);

    updateActionStates();
    statusBar()->showMessage(
        QString("已載入：%1 (%2 x %3)")
            .arg(filePath)
            .arg(currentPixmap.width())
            .arg(currentPixmap.height()));
}

void MainWindow::saveImage()
{
    if (currentPixmap.isNull())
        return;

    QString filePath = QFileDialog::getSaveFileName(
        this, "儲存影像", "output.png",
        "PNG 影像 (*.png);;JPEG 影像 (*.jpg *.jpeg);;BMP 影像 (*.bmp)");

    if (filePath.isEmpty())
        return;

    if (!currentPixmap.save(filePath)) {
        QMessageBox::warning(this, "儲存失敗", "無法將影像寫入指定路徑。");
        return;
    }

    statusBar()->showMessage("已儲存至：" + filePath, 4000);
}

void MainWindow::onToolSelected(QAction *action)
{
    if (action == panToolAct) {
        graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        statusBar()->showMessage("目前模式：檢視 / 拖曳畫布");
    } else if (action == brushToolAct) {
        graphicsView->setDragMode(QGraphicsView::NoDrag);
        statusBar()->showMessage("目前模式：畫筆（下一階段實作）");
    } else if (action == cropToolAct) {
        graphicsView->setDragMode(QGraphicsView::NoDrag);
        statusBar()->showMessage("目前模式：裁切（下一階段實作）");
    }
}

void MainWindow::onFlipHorizontal()
{
    statusBar()->showMessage("觸發：水平翻轉（下一階段實作）");
}

void MainWindow::onFlipVertical()
{
    statusBar()->showMessage("觸發：垂直翻轉（下一階段實作）");
}