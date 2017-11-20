#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QtGlobal>

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

  QString scalingMethod = ui->input_ScalingMethod->currentText();

  ui->label_TotalColors->setText(QString::number(stepsMaterial*stepsLuminance));

  QImage img;
  if(srcImg)
  {
    img = *srcImg;
    int longSide = qMax(srcImg->width(), srcImg->height());

    if(longSide > maxInputSize)
    {
      float factor = (float(maxInputSize)/float(longSide));
      img = ScaleBilinear(img, factor);
    }
    if(applyGrayscale)
      img = NormalizedGrayscale(img, blackPoint, grayMidpoint, whitePoint);
    if(applyScaling)
    {
      if(scalingMethod=="AVIR")
        img = ScaleAVIR(img, 1.0 / scaleFactor);
      else if(scalingMethod=="DPID")
        img = ScaleDPID(img, 1.0 / scaleFactor, sharpeningCurve);
      else if(scalingMethod=="Bilinear")
        img = ScaleBilinear(img, 1.0 / scaleFactor);
    }
    if(applyAlphaThreshold)
      img = AlphaThreshold(img, alphaThreshold);
    if(applyPosterize)
      img = Posterize(img, stepsLuminance);

    scene->clear();
    QGraphicsPixmapItem* pixmap = scene->addPixmap(QPixmap::fromImage(img));
    pixmap->setScale(scaleFactor);
    ui->preview->setScene(scene);
  }
}

void MainWindow::saveImageAction()
{

}

void MainWindow::loadImageAction()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"", tr("Images (*.png *.xpm *.jpg)"));
  if(srcImg)
    delete srcImg;
  srcImg = new QImage(fileName);
  settingsChanged();
}
