#include "colorroutine.h"
#include <float.h>
#include <math.h>

QColor blendColor(const QColor &c1, const QColor &c2, double alpha)
{
    int r = floor((double)c2.red()*alpha + (double)c1.red()*(1-alpha));
    int g = floor((double)c2.green()*alpha + (double)c1.green()*(1-alpha));
    int b = floor((double)c2.blue()*alpha + (double)c1.blue()*(1-alpha));
    int a = floor((double)c2.alpha()*alpha + (double)c1.alpha()*(1-alpha));
    return QColor(r, g, b, a);
}
