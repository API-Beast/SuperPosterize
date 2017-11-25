#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QGraphicsScene;
class QImage;

namespace avir
{
  struct CImageResizerParams;
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void settingsChanged();
  void saveImageAction();
  void loadImageAction();

protected:
  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent* event);

private:
  Ui::MainWindow *ui = nullptr;
  QGraphicsScene *scene = nullptr;
  QImage *srcImg = nullptr;
  avir::CImageResizerParams *avirParams;
  bool blockSlots = false;
};

#endif // MAINWINDOW_H
