#pragma once

#include <QImage>

QImage ScaleAVIR(const QImage& input, float factor);
QImage ScaleBilinear(const QImage& input, float factor);
QImage ScaleDPID(const QImage& input, int pixelFactor, float sharpeningCurve = 0.5f);
QImage AlphaThreshold(const QImage& input, float threshold);
QImage NormalizedGrayscale(const QImage& input, float blackPoint=0.0f, float midPoint=0.5f, float whitePoint=1.0f);
QImage Posterize(const QImage& input, int stepsL, int stepsH);
