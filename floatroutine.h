#ifndef FLOATROUTINE_H
#define FLOATROUTINE_H

double getNaN();
bool isNaN(double x);
bool isZero(double x);
double sqr(double x);
double fround(double x);
int iround(double x);
bool doubleEquals(double x1, double x2);
bool doubleLess(double x1, double x2);
bool doubleGreater(double x1, double x2);
bool doubleLessOrEquals(double x1, double x2);
bool doubleGreaterOrEquals(double x1, double x2);

#endif // FLOATROUTINE_H
