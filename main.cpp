#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include "filters.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QCommandLineParser parser;
  parser.addOption({{"b", "batch"}, "Batch processing mode"});
  parser.addOption({{"a", "alpha-threshold"}, "Alpha threshold"});
  parser.addPositionalArgument("files", "The files to process", "[files...]");
  parser.addHelpOption();
  parser.process(a);

  if(!parser.optionNames().contains("batch"))
  {
    MainWindow w;
    w.show();
    return a.exec();
  }
  else
  {
    bool doAlphaThreshold = parser.optionNames().contains("alpha-threshold");
    float alphaThreshold = parser.value("alpha-threshold").toFloat();

    QStringList files = parser.positionalArguments();
    for(const QString& file: files)
    {
      QImage image(file);
      if(doAlphaThreshold)
        image = AlphaThreshold(image, alphaThreshold);
      image.save(file);
    }

    return 0;
  }
}
