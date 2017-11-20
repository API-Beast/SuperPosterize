#include "filters.h"
#include "avir.h"
#include <QtMath>

QImage ScaleAVIR(const QImage& input, float factor)
{
  QImage converted;
  if(input.format() != QImage::Format_ARGB32)
    converted = input.convertToFormat(QImage::Format_ARGB32);
  else
    converted = input;

  uchar* src = converted.bits();
  avir::CImageResizer<> ImageResizer(8, 0);

  int w = input.width();
  int h = input.height();
  int ow = w*factor;
  int oh = h*factor;

  QImage retVal(ow, oh, QImage::Format_ARGB32);
  uchar* out = retVal.bits();

  avir::CImageResizerVars vars;
  vars.UseSRGBGamma = true;
  ImageResizer.resizeImage(src, w, h, 0,
                           out, ow, oh, 4, 0, &vars);
  return retVal;
}

QImage ScaleBilinear(const QImage& input, float factor)
{
  return input.scaled(input.width()*factor, input.height()*factor);
}

namespace
{
  struct KernelValue
  {
    uchar r;
    uchar g;
    uchar b;
    uchar a;
    float w = 0.0f;

    KernelValue& operator=(QRgb val)
    {
      r = qRed(val);
      g = qGreen(val);
      b = qBlue(val);
      a = qAlpha(val);
    }

    float distance(QRgb val)
    {
      return qSqrt( (qRed(val)-r) + (qGreen(val)-g) + (qBlue(val)-b) + (qAlpha(val)-a) ) / 255.0f;
    }
  };

  QRgb dpidKernel(const QImage& src, QRgb refColor, int startX, int startY, int width, int height, float sharpeningCurve)
  {
    KernelValue* values = new KernelValue[width*height];
    float cumWeight = 0.0;

    for(int y = startY; y < startY + height; y++)
    for(int x = startX; x < startX + width; x++)
    {
      values[y*width+x]   = src.pixel(x, y);
      values[y*width+x].w = qPow(values[y*width+x].distance(refColor), sharpeningCurve);
      cumWeight += values[y*width+x].w;
    }

    int red   = 0;
    int blue  = 0;
    int green = 0;
    int alpha = 0;

    for(int y = 0; y < height; y++)
    for(int x = 0; x < width; x++)
    {
      KernelValue& val = values[y*width+x];
      red   += val.r * val.w;
      green += val.g * val.w;
      blue  += val.b * val.w;
      alpha += val.a * val.w;
    }
    delete[] values;

    red   /= cumWeight;
    green /= cumWeight;
    blue  /= cumWeight;
    alpha /= cumWeight;

    return qRgba(red, green, blue, alpha);
  }
}

QImage ScaleDPID(const QImage& src, float factor, float sharpeningCurve)
{
  // Implementation of "Rapid, Detail-Preserving Image Downscaling"-Research paper by Nicolas Weber et al from 2016
  QImage reference = ScaleAVIR(src, factor);
  int pixelFactor = 1 / factor;

  QImage retVal(reference.width(), reference.height(), QImage::Format_ARGB32);
  for(int y = 0; y < retVal.height(); ++y)
  {
    QRgb* line = (QRgb*)retVal.scanLine(y);
    QRgb* refLine = (QRgb*)retVal.scanLine(y);
    for(int x = 0; x < retVal.width(); ++x)
      line[x] = dpidKernel(src, refLine[x], x*pixelFactor, y*pixelFactor, pixelFactor, pixelFactor, sharpeningCurve);
  }

  return retVal;
}

QImage AlphaThreshold(const QImage& input, float threshold)
{
  QImage retVal(input.convertToFormat(QImage::Format_ARGB32));
  for(int y = 0; y < retVal.height(); ++y)
  {
    uchar* line = retVal.scanLine(y);
    for(int x = 0; x < retVal.width(); ++x)
    {
      float alpha = line[x*4+3] / 255.0;
      if(alpha > threshold)
        line[x*4+3] = 255;
      else
      {
        line[x*4+0] = 0;
        line[x*4+1] = 0;
        line[x*4+2] = 0;
        line[x*4+3] = 0;
      }
    }
  }
  return retVal;
}

namespace
{
  float getLuminance(QRgb val)
  {
    return 0.2126 * (qRed(val)/255.0) + 0.7152 * (qGreen(val)/255.0) + 0.0722 * (qBlue(val)/255.0);
  }

  template<typename T>
  T interpolateLinear(T start, T end, float factor)
  {
    return start + (end - start) * factor;
  };

  template<typename T>
  T interpolateLinear(T start, T end, float startT, float endT, float time)
  {
    return interpolateLinear(start, end, (time-startT)/(endT-startT));
  };


  float applyMapping(float value, float start, float mid, float end, float midp=0.5f)
  {
    if(value < start)
      return 0.0f;
    if(value > end)
      return 1.0f;

    if(value < mid)
      return interpolateLinear(0.0f, midp, start, mid, value);
    else
      return interpolateLinear(midp, 1.0f, mid, end, value);
  }

  float barycentricInterpolation(float px, float py, float x1, float y1, float v1, float x2, float y2, float v2, float x3, float y3, float v3)
  {
    float div = ((y2-y3)*(x1-x3) + (x3-x2)*(y1-y3));
    float w1 = ((y2-y3)*(px-x3) + (x3-x2)*(py-y3)) / div;
    float w2 = ((y3-y1)*(px-x3) + (x1-x3)*(py-y3)) / div;
    float w3 = 1 - w1 - w2;
    return v1*w1 + v2*w2 + v3*w3;
  }
}

QImage NormalizedGrayscale(const QImage& input, float blackPoint, float midPoint, float whitePoint)
{
  QImage retVal(input.convertToFormat(QImage::Format_ARGB32));
  QList<float> bucketL;
  bucketL.reserve(input.width()*input.height());

  for(int y = 0; y < retVal.height(); ++y)
  {
    QRgb* line = (QRgb*)retVal.scanLine(y);
    for(int x = 0; x < retVal.width(); ++x)
    {
      const QRgb& pixel = line[x];
      if(qAlpha(pixel) > 64)
        bucketL.push_back(getLuminance(pixel));
    }
  }

  qSort(bucketL);
  float minL = bucketL[bucketL.length()*blackPoint];
  float medianL = bucketL[bucketL.length()*midPoint];
  float maxL = bucketL[(bucketL.length()-1)*whitePoint];

  for(int y = 0; y < retVal.height(); ++y)
  {
    uchar* line = retVal.scanLine(y);
    for(int x = 0; x < retVal.width(); ++x)
    {
      QRgb* pixel = (QRgb*)&line[x*4];
      float luminance = getLuminance(*pixel);
      float normalized = applyMapping(luminance, minL, medianL, maxL);
      uchar byteV = normalized*255;
      line[x*4+0] = byteV;
      line[x*4+1] = byteV;
      line[x*4+2] = byteV;
    }
  }
  return retVal;
}

QImage Posterize(const QImage& input, int steps)
{
  int stepSize = 255 / steps;
  QImage retVal(input.convertToFormat(QImage::Format_ARGB32));
  for(int y = 0; y < retVal.height(); ++y)
  {
    uchar* line = retVal.scanLine(y);
    for(int x = 0; x < retVal.width(); ++x)
    {
      uchar val = qMax(qMax(line[x*4+0], line[x*4+1]), line[x*4+2]);
      uchar byteV = val/stepSize*stepSize;
      line[x*4+0] = byteV;
      line[x*4+1] = byteV;
      line[x*4+2] = byteV;
    }
  }
  return retVal;
}
