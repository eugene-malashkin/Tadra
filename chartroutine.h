#ifndef CHARTROUTINE_H
#define CHARTROUTINE_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QFont>
#include "numeral.h"

struct FloatRange
{
    double min;
    double max;
    FloatRange();
    FloatRange(double value);
    FloatRange(double aMin, double aMax);
    bool operator == (const FloatRange &right) const;
    bool operator != (const FloatRange &right) const;
    bool isValid() const;
    bool isSingleValue() const;
    double singleValue() const;
    double length() const;
    bool contains(double value) const;
    bool isSubsetOf(const FloatRange &another) const;
    void append(double value);
    void append(const FloatRange &another);
    void clear();
    FloatRange& operator << (double value);
    FloatRange& operator << (const FloatRange &another);
    QString toString() const;
};

struct DateTimeRange
{
    QDateTime min;
    QDateTime max;
    DateTimeRange();
    DateTimeRange(const QDateTime &value);
    DateTimeRange(const QDateTime &aMin, const QDateTime &aMax);
    bool operator == (const DateTimeRange &right) const;
    bool operator != (const DateTimeRange &right) const;
    bool isValid() const;
    bool isSingleValue() const;
    qint64 secondsSpan() const;
    bool contains(const QDateTime &value) const;
    bool isSubsetOf(const DateTimeRange &another) const;
    void append(const QDateTime &value);
    void append(const DateTimeRange &another);
    void clear();
    DateTimeRange& operator << (const QDateTime &value);
    DateTimeRange& operator << (const DateTimeRange &another);
    DateTimeRange intersected(const DateTimeRange &another) const;
    QString toString() const;
};

struct ScreenPoints
{
    double pointAtMin;
    double pointAtMax;
    double minMarkPoint;
    double maxMarkPoint;
    ScreenPoints();
    ScreenPoints(double aPointAtMin, double aPointAtMax);
    ScreenPoints(double aPointAtMin, double aPointAtMax, double aMinMarkPoint, double aMaxMarkPoint);
    ScreenPoints(const ScreenPoints &another);
    bool operator == (const ScreenPoints &right) const;
    bool operator != (const ScreenPoints &right) const;
    bool isValid() const;
};

class Scale
{
public:
    Scale();
    void setOrientation(Qt::Orientation value);
    Qt::Orientation orientation() const;
    void setFont(const QFont &value);
    QFont font() const;
    void setMinimalMarkSpacing(double value);
    double minimalMarkSpacing() const;
    void setScreenPoints(const ScreenPoints &value);
    ScreenPoints screenPoints() const;
    void setLogicRange(const FloatRange &value);
    FloatRange logicRange() const;
    virtual bool isReady() const;
    void setRequestUseExponentialTransformation(bool value);
    bool requestUseExponentialTransformation() const;
    bool canUseExponentialTransformation() const;
    bool useExponentialTransformation() const;
    double screenToLogic(double screen) const;
    double logicToScreen(double logic) const;
    double worstScaleCoef() const;

protected:
    void changed();
    bool isStepStringAvailable(const QString &stepString, double distanceBetween) const;
    virtual void computeParams();

private:
    struct LinearTransformation
    {
        // logic = a*screen + b
        double a;
        double b;
        ScreenPoints screenPoints;
        LinearTransformation();
        bool allow(const ScreenPoints &aScreenPoints, const FloatRange &aLogicRange) const;
        bool initialize(const ScreenPoints &aScreenPoints, const FloatRange &aLogicRange);
        bool isReady() const;
        double screenToLogic(double screen) const;
        double logicToScreen(double logic) const;
        double worstScaleCoef() const;
        bool isSingleValue() const;
    };
    struct ExponentialTransformation
    {
        // logic = b * c^screen
        double b;
        double c;
        FloatRange innerRange;
        ExponentialTransformation();
        bool allow(const ScreenPoints &aScreenPoints, const FloatRange &aLogicRange) const;
        bool initialize(const ScreenPoints &aScreenPoints, const FloatRange &aLogicRange);
        bool isReady() const;
        double screenToLogic(double screen) const;
        double logicToScreen(double logic) const;
        double worstScaleCoef() const;
    };
    LinearTransformation m_linearTransformation;
    Qt::Orientation m_orientation;
    QFont m_font;
    double m_minimalMarkSpacing;
    ExponentialTransformation m_exponentialTransformation;
    ScreenPoints m_screenPoints;
    FloatRange m_logicRange;
    bool m_requestUseExponentialTransformation;
};

struct FloatScaleMark
{
    double value;
    QString text;
    double position;
    FloatScaleMark();
    FloatScaleMark(double aValue, const QString &aText, double aPosition);
    bool isValid() const;
};
typedef QList<FloatScaleMark> FloatScaleMarkList;

class FloatScale : public Scale
{
public:
    FloatScale();
    FloatScaleMarkList markList() const;

protected:
    void computeParams() override;

private:
    double m_step;
    int m_stepPrecision;
    NumeralFormat m_stepNumeralFormat;
    FloatScaleMarkList m_markList;
    int stepPrecision(double step, int tuneDigits) const;
    FloatRange stepRange(double step) const;
    bool isStepAvailable(double step, int tuneDigits) const;
    void findStep(double min, double max);
    void computeStep();
    void computeMarkList();
};

struct DateTimeScaleStep
{
    enum Type {Minute, Hour, Day, Week, Month, Quarter, Year};
    Type type;
    int count;
    DateTimeScaleStep();
    DateTimeScaleStep(Type aType, int aCount);
    DateTimeScaleStep(const DateTimeScaleStep &another);
    bool isIntraday() const;
    int seconds() const;
    QDateTime floorValue(const QDateTime &baseValue) const;
    QDateTime ceilValue(const QDateTime &baseValue) const;
    QDateTime add(const QDateTime &startValue, int intervals) const;
    QString format(const QDateTime &testValue) const;
    QString toString() const;
};
typedef QList<DateTimeScaleStep> DateTimeScaleSteps;

struct DateTimeScaleMark
{
    QDateTime value;
    QString text;
    double position;
    DateTimeScaleMark();
    DateTimeScaleMark(const QDateTime &aValue, const QString &aText, double aPosition);
    bool isValid() const;
};
typedef QList<DateTimeScaleMark> DateTimeScaleMarkList;

class DateTimeScale : public Scale
{
public:
    DateTimeScale();
    void setValues(const QList<QDateTime> &value);
    QList<QDateTime> values() const;
    void setIntradayFlag(bool value);
    bool intradayFlag() const;
    double dateTimeToLogic(const QDateTime &dateTime) const;
    double dateTimeToScreen(const QDateTime &dateTime) const;
    QDateTime logicToDateTime(double logic) const;
    QDateTime screenToDateTime(double screen) const;
    DateTimeRange dateTimeRange() const;
    DateTimeScaleMarkList markList() const;

protected:
    void computeParams() override;

private:
    DateTimeScaleSteps m_possibleSteps;
    QList<QDateTime> m_values;
    bool m_intradayFlag;
    DateTimeScaleStep m_step;
    DateTimeScaleMarkList m_markList;
    void initializePossibleSteps();
    DateTimeRange stepRange(const DateTimeScaleStep &step) const;
    bool isStepAvailable(const DateTimeScaleStep &step) const;
    void computeStep();
    void computeMarkList();
};

#endif // CHARTROUTINE_H
