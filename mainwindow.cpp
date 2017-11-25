#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QtGlobal>
#include <QtDebug>
#include <QDropEvent>
#include <QMimeData>

#include "filters.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  scene = new QGraphicsScene(this);
  ui->preview->setScene(scene);

  this->settingsChanged();
}

MainWindow::~MainWindow()
{
  delete scene;
  delete ui;
  delete srcImg;
}

void MainWindow::settingsChanged()
{
  if(blockSlots == true)
    return;

  int stepsLuminance = ui->input_LuminanceSteps->value();
  int stepsMaterial  = ui->input_MaterialSteps->value();
  int scaleFactor    = ui->input_DownscaleFactor->value();
  int maxInputSize   = ui->input_MaxInputSize->value();
  float alphaThreshold = ui->input_AlphaThreshold->value()/100.0;

  float blackPoint   = ui->input_BlackPoint->value();
  float grayMidpoint = ui->input_GrayPoint->value();
  float whitePoint   = ui->input_WhitePoint->value();

  float sharpeningCurve = ui->input_SharpeningCurve->value();

  bool applyAlphaThreshold = ui->settings_AlphaThreshold->isChecked();
  bool applyScaling        = ui->settings_Downscale->isChecked();
  bool applyGrayscale      = ui->settings_NormalizeLuminance->isChecked();
  bool applyPosterize      = ui->settings_Posterize->isChecked();
  bool limitInput          = ui->settings_LimitInputSize->isChecked();

  if(!applyScaling)
    scaleFactor = 1;

  QString scalingMethod = ui->input_ScalingMethod->currentText();

  ui->label_TotalColors->setText(QString::number(stepsMaterial*stepsLuminance));

  QImage img;
  if(srcImg)
  {
    img = *srcImg;
    int longSide = qMax(srcImg->width(), srcImg->height());

    if(limitInput && longSide > maxInputSize)
    {
      float factor = (float(maxInputSize)/float(longSide));
      img = ScaleBilinear(img, factor);
    }
    if(applyScaling)
    {
      if(scalingMethod=="AVIR")
        img = ScaleAVIR(img, 1.0 / scaleFactor);
      else if(scalingMethod=="DPID")
        img = ScaleDPID(img, scaleFactor, sharpeningCurve);
      else if(scalingMethod=="Bilinear")
        img = ScaleBilinear(img, 1.0 / scaleFactor);
    }
    if(applyGrayscale)
      img = NormalizedGrayscale(img, blackPoint, grayMidpoint, whitePoint);
    if(applyAlphaThreshold)
      img = AlphaThreshold(img, alphaThreshold);
    if(applyPosterize)
      img = Posterize(img, stepsLuminance, stepsMaterial);

    scene->clear();
    QGraphicsPixmapItem* pixmap = scene->addPixmap(QPixmap::fromImage(img));
    QGraphicsRectItem* rect = scene->addRect(pixmap->boundingRect().adjusted(-2, -2, 2, 2), Qt::SolidLine, Qt::NoBrush);
    pixmap->setScale(scaleFactor*2);
    rect->setScale(scaleFactor*2);
    ui->preview->setScene(scene);
  }
}

void MainWindow::saveImageAction()
{

}

void MainWindow::loadImageAction()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"", tr("Images (*.png *.xpm *.jpg)"));

  if(!fileName.isEmpty())
  {
    if(srcImg)
        delete srcImg;
    srcImg = new QImage(fileName);
    settingsChanged();
  }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
  event->accept();
}

void MainWindow::dropEvent(QDropEvent* event)
{
  QUrl fileName = event->mimeData()->urls()[0];
  if(!fileName.isEmpty())
  {
    if(srcImg)
        delete srcImg;
    srcImg = new QImage(fileName.toLocalFile());
    settingsChanged();
  }

  event->setDropAction(Qt::CopyAction);
  event->accept();
}


