#include "floatroutine.h"
#include <float.h>
#include <math.h>
#include <QtNumeric>

double getNaN()
{
    return qSNaN();
}

bool isNaN(double x)
{
#if defined(_MSC_VER)
    return (_isnan(x) != 0);
#else
    return (isnan(x) != 0);
#endif
}

bool isZero(double x)
{
    return (fabs(x) < DBL_EPSILON);
}

bool isZeroForDigits(double x, int digits)
{
    double digitsEps = 0.5*pow((double)10.0, (double)-digits);
    if(digitsEps < DBL_EPSILON)
        return (fabs(x) < DBL_EPSILON);
    else
        return (fabs(x) < digitsEps);
}

double sqr(double x)
{
    return x*x;
}

double fround(double x)
{
    return iround(x);
}

int iround(double x)
{
    double a = fabs(x);
    int result = int(a + DBL_EPSILON);
    if (a - result >= 0.5)
    {
        result++;
    }
    if (x < 0)
    {
        result = -result;
    }
    return result;
}

double compareSign(double x1, double x2)
{
    double sign1 = fabs(x1)*DBL_EPSILON;
    double sign2 = fabs(x2)*DBL_EPSILON;
    double sign = (sign1 > sign2)? sign1 : sign2;
    if (sign < DBL_EPSILON)
    {
        sign = DBL_EPSILON;
    }
    if (sign > 1)
    {
        sign = 1;
    }
    return sign;
}

bool doubleEquals(double x1, double x2)
{
    bool result = false;
    bool x1NaN = isNaN(x1);
    bool x2NaN = isNaN(x2);
    if (x1NaN && x2NaN)
    {
        result = true;
    }
    if (!x1NaN && !x2NaN)
    {
        double sign = compareSign(x1, x2);
        result = fabs(x1-x2) < sign;
    }
    return result;
}

bool doubleLess(double x1, double x2)
{
    bool result = false;
    bool x1NaN = isNaN(x1);
    bool x2NaN = isNaN(x2);
    if (x1NaN && x2NaN)
    {
        result = true;
    }
    if (!x1NaN && !x2NaN)
    {
        double sign = compareSign(x1, x2);
        result = x1 - x2 < -sign;
    }
    return result;
}

bool doubleGreater(double x1, double x2)
{
    bool result = false;
    bool x1NaN = isNaN(x1);
    bool x2NaN = isNaN(x2);
    if (x1NaN && x2NaN)
    {
        result = true;
    }
    if (!x1NaN && !x2NaN)
    {
        double sign = compareSign(x1, x2);
        result = x1 - x2 > sign;
    }
    return result;
}

bool doubleLessOrEquals(double x1, double x2)
{
    bool result = false;
    bool x1NaN = isNaN(x1);
    bool x2NaN = isNaN(x2);
    if (x1NaN && x2NaN)
    {
        result = true;
    }
    if (!x1NaN && !x2NaN)
    {
        double sign = compareSign(x1, x2);
        result = x1 - x2 < sign;
    }
    return result;
}

bool doubleGreaterOrEquals(double x1, double x2)
{
    bool result = false;
    bool x1NaN = isNaN(x1);
    bool x2NaN = isNaN(x2);
    if (x1NaN && x2NaN)
    {
        result = true;
    }
    if (!x1NaN && !x2NaN)
    {
        double sign = compareSign(x1, x2);
        result = x1 - x2 > -sign;
    }
    return result;
}
