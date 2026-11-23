#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>

class QAction;
class QActionGroup;
class QGraphicsScene;
class QGraphicsView;
class QGraphicsPixmapItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void openImage();
    void saveImage();
    void onToolSelected(QAction *action);
    void onFlipHorizontal();
    void onFlipVertical();

private:
    void setupCentralWorkspace();
    void createActions();
    void createMenus();
    void createToolBars();
    void updateActionStates();

    // 中央影像工作區
    QGraphicsView *graphicsView = nullptr;
    QGraphicsScene *graphicsScene = nullptr;
    QGraphicsPixmapItem *pixmapItem = nullptr;
    QPixmap currentPixmap;

    // File Actions
    QAction *openAct = nullptr;
    QAction *saveAct = nullptr;
    QAction *exitAct = nullptr;

    // Edit / Tool Actions (互斥模式：檢視 / 畫筆 / 裁切)
    QActionGroup *toolGroup = nullptr;
    QAction *panToolAct = nullptr;
    QAction *brushToolAct = nullptr;
    QAction *cropToolAct = nullptr;

    // Image Actions
    QAction *flipHAct = nullptr;
    QAction *flipVAct = nullptr;
};

#endif // MAINWINDOW_H