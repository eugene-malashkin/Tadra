#include "chartroutine.h"
#include <QFontMetricsF>
#include <float.h>
#include <math.h>
#include "floatroutine.h"

#include <QDebug>

static const int criticalMarkCount = 200;

//******************************************************************************
/*!
\struct FloatRange
*/
//******************************************************************************

FloatRange::FloatRange()
    : min(getNaN())
    , max(getNaN())
{
}

FloatRange::FloatRange(double value)
    : min(value)
    , max(value)
{
}

FloatRange::FloatRange(double aMin, double aMax)
    : min(aMin)
    , max(aMax)
{
}

bool FloatRange::operator == (const FloatRange &right) const
{
    return
        (doubleEquals(min, right.min) || (isNaN(min) && isNaN(right.min))) &&
        (doubleEquals(max, right.max) || (isNaN(max) && isNaN(right.max)));
}

bool FloatRange::operator != (const FloatRange &right) const
{
    return !(operator ==(right));
}

bool FloatRange::isValid() const
{
    return (!isNaN(min)) && (!isNaN(max)) && (min <= max);
}

bool FloatRange::isSingleValue() const
{
    return (!isNaN(min)) && (!isNaN(max)) && (doubleEquals(min, max));
}

double FloatRange::singleValue() const
{
    return (min + max) / 2.0;
}

double FloatRange::length() const
{
    double result = getNaN();
    if (isValid())
    {
        result = max - min;
    }
    return result;
}

bool FloatRange::contains(double value) const
{
    return (!isNaN(value)) && (isValid()) && (value >= min) && (value <= max);
}

bool FloatRange::isSubsetOf(const FloatRange &another) const
{
    bool result = false;
    if (isValid() && another.isValid())
    {
        result = (min >= another.min) && (max <= another.max);
    }
    return result;
}

void FloatRange::append(double value)
{
    if (!isNaN(value))
    {
        if ((isNaN(min)) || (min > value))
        {
            min = value;
        }
        if ((isNaN(max)) || (max < value))
        {
            max = value;
        }
    }
}

void FloatRange::append(const FloatRange &another)
{
    if (another.isValid())
    {
        append(another.min);
        append(another.max);
    }
}

void FloatRange::clear()
{
    min = getNaN();
    max = getNaN();
}

FloatRange& FloatRange::operator << (double value)
{
    append(value);
    return *this;
}

FloatRange& FloatRange::operator << (const FloatRange &another)
{
    append(another);
    return *this;
}

QString FloatRange::toString() const
{
    return QString("[%1, %2]").arg(min).arg(max);
}


//******************************************************************************
/*!
\struct DateTimeRange
*/
//******************************************************************************

DateTimeRange::DateTimeRange()
    : min()
    , max()
{
}

DateTimeRange::DateTimeRange(const QDateTime &value)
    : min(value)
    , max(value)
{
}

DateTimeRange::DateTimeRange(const QDateTime &aMin, const QDateTime &aMax)
    : min(aMin)
    , max(aMax)
{
}

bool DateTimeRange::operator == (const DateTimeRange &right) const
{
    return (min == right.min) && (max == right.max);
}

bool DateTimeRange::operator != (const DateTimeRange &right) const
{
    return !(operator ==(right));
}

bool DateTimeRange::isValid() const
{
    return (min.isValid()) && (max.isValid()) && (min <= max);
}

bool DateTimeRange::isSingleValue() const
{
    return (min.isValid()) && (max.isValid()) && (min == max);
}

qint64 DateTimeRange::secondsSpan() const
{
    qint64 result = 0;
    if (isValid())
    {
        result = min.secsTo(max);
    }
    return result;
}

bool DateTimeRange::contains(const QDateTime &value) const
{
    return (value.isValid()) && (isValid()) && (value >= min) && (value <= max);
}

bool DateTimeRange::isSubsetOf(const DateTimeRange &another) const
{
    bool result = false;
    if (isValid() && another.isValid())
    {
        result = (min >= another.min) && (max <= another.max);
    }
    return result;
}

void DateTimeRange::append(const QDateTime &value)
{
    if (value.isValid())
    {
        if ((!min.isValid()) || (min > value))
        {
            min = value;
        }
        if ((!max.isValid()) || (max < value))
        {
            max = value;
        }
    }
}

void DateTimeRange::append(const DateTimeRange &another)
{
    if (another.isValid())
    {
        append(another.min);
        append(another.max);
    }
}

void DateTimeRange::clear()
{
    min = QDateTime();
    max = QDateTime();
}

DateTimeRange& DateTimeRange::operator << (const QDateTime &value)
{
    append(value);
    return *this;
}

DateTimeRange& DateTimeRange::operator << (const DateTimeRange &another)
{
    append(another);
    return *this;
}

DateTimeRange DateTimeRange::intersected(const DateTimeRange &another) const
{
    DateTimeRange result;
    if ((isValid()) && (another.isValid()))
    {
        DateTimeRange tmp;
        tmp.min = qMax(min, another.min);
        tmp.max = qMin(max, another.max);
        if (tmp.isValid())
        {
            result = tmp;
        }
    }
    return result;
}

QString DateTimeRange::toString() const
{
    return QString("[%1, %2]").arg(min.toString("dd.MM.yyyy hh:mm:ss")).arg(max.toString("dd.MM.yyyy hh:mm:ss"));
}


//******************************************************************************
/*!
\struct ScreenPoints
*/
//******************************************************************************

ScreenPoints::ScreenPoints()
    : pointAtMin(0)
    , pointAtMax(1)
    , minMarkPoint(0)
    , maxMarkPoint(1)
{
}

ScreenPoints::ScreenPoints(double aPointAtMin, double aPointAtMax)
    : pointAtMin(aPointAtMin)
    , pointAtMax(aPointAtMax)
    , minMarkPoint(aPointAtMin)
    , maxMarkPoint(aPointAtMax)
{
}

ScreenPoints::ScreenPoints(double aPointAtMin, double aPointAtMax, double aMinMarkPoint, double aMaxMarkPoint)
    : pointAtMin(aPointAtMin)
    , pointAtMax(aPointAtMax)
    , minMarkPoint(aMinMarkPoint)
    , maxMarkPoint(aMaxMarkPoint)
{

}

ScreenPoints::ScreenPoints(const ScreenPoints &another)
    : pointAtMin(another.pointAtMin)
    , pointAtMax(another.pointAtMax)
    , minMarkPoint(another.minMarkPoint)
    , maxMarkPoint(another.maxMarkPoint)
{
}

bool ScreenPoints::operator == (const ScreenPoints &right) const
{
    return
        doubleEquals(pointAtMin, right.pointAtMin) &&
        doubleEquals(pointAtMax, right.pointAtMax) &&
        doubleEquals(minMarkPoint, right.minMarkPoint) &&
        doubleEquals(maxMarkPoint, right.maxMarkPoint);
}

bool ScreenPoints::operator != (const ScreenPoints &right) const
{
    return !(operator==(right));
}

bool ScreenPoints::isValid() const
{
    const double minimumDelta = 0.1;

    // Помимо валидных значений
    // разницы (pointAtMax - pointAtMin) и (maxMarkPoint - minMarkPoint) должны быть одного знака.
    bool result =
        (abs(pointAtMax - pointAtMin) >= minimumDelta) &&
        (abs(maxMarkPoint - minMarkPoint) >= minimumDelta) &&
        ((pointAtMax - pointAtMin)*(maxMarkPoint - minMarkPoint) > FLT_EPSILON);
    return result;
}


//******************************************************************************
/*!
\class Scale
*/
//******************************************************************************

Scale::Scale()
    : m_linearTransformation()
    , m_exponentialTransformation()
    , m_orientation(Qt::Horizontal)
    , m_font()
    , m_minimalMarkSpacing(1)
    , m_screenPoints()
    , m_logicRange()
    , m_requestUseExponentialTransformation(false)
{

}

void Scale::setOrientation(Qt::Orientation value)
{
    if (m_orientation != value)
    {
        m_orientation = value;
        changed();
    }
}

Qt::Orientation Scale::orientation() const
{
    return m_orientation;
}

void Scale::setFont(const QFont &value)
{
    if (m_font != value)
    {
        m_font = value;
        changed();
    }
}

QFont Scale::font() const
{
    return m_font;
}

void Scale::setMinimalMarkSpacing(double value)
{
    if (m_minimalMarkSpacing != value)
    {
        m_minimalMarkSpacing = value;
        changed();
    }
}

double Scale::minimalMarkSpacing() const
{
    return m_minimalMarkSpacing;
}

void Scale::setScreenPoints(const ScreenPoints &value)
{
    if (m_screenPoints != value)
    {
        m_screenPoints = value;
        changed();
    }
}

ScreenPoints Scale::screenPoints() const
{
    return m_screenPoints;
}

void Scale::setLogicRange(const FloatRange &value)
{
    if (m_logicRange != value)
    {
        m_logicRange = value;
        changed();
    }
}

FloatRange Scale::logicRange() const
{
    return m_logicRange;
}

bool Scale::isReady() const
{
    if (useExponentialTransformation())
    {
        return m_exponentialTransformation.isReady();
    }
    else
    {
        return m_linearTransformation.isReady();
    }
}

void Scale::setRequestUseExponentialTransformation(bool value)
{
    if (m_requestUseExponentialTransformation != value)
    {
        m_requestUseExponentialTransformation = value;
        changed();
    }
}

bool Scale::requestUseExponentialTransformation() const
{
    return m_requestUseExponentialTransformation;
}

bool Scale::canUseExponentialTransformation() const
{
    return m_exponentialTransformation.allow(screenPoints(), logicRange());
}

bool Scale::useExponentialTransformation() const
{
    return requestUseExponentialTransformation() && canUseExponentialTransformation();
}

double Scale::screenToLogic(double screen) const
{
    if (useExponentialTransformation())
    {
        return m_exponentialTransformation.screenToLogic(screen);
    }
    else
    {
        return m_linearTransformation.screenToLogic(screen);
    }
}

double Scale::logicToScreen(double logic) const
{
    if (useExponentialTransformation())
    {
        return m_exponentialTransformation.logicToScreen(logic);
    }
    else
    {
        return m_linearTransformation.logicToScreen(logic);
    }
}

double Scale::worstScaleCoef() const
{
    if (useExponentialTransformation())
    {
        return m_exponentialTransformation.worstScaleCoef();
    }
    else
    {
        return m_linearTransformation.worstScaleCoef();
    }
}

void Scale::changed()
{
    m_linearTransformation.initialize(screenPoints(), logicRange());
    m_exponentialTransformation.initialize(screenPoints(), logicRange());
    computeParams();
}

bool Scale::isStepStringAvailable(const QString &stepString, double distanceBetween) const
{
    QFontMetricsF fm(font());
    if (orientation() == Qt::Horizontal)
    {
        return (fm.width(stepString) + minimalMarkSpacing()) <= distanceBetween;
    }
    else
    {
        return (fm.height() + minimalMarkSpacing()) <= distanceBetween;
    }
}

void Scale::computeParams()
{

}

Scale::LinearTransformation::LinearTransformation()
    : a(getNaN())
    , b(getNaN())
    , screenPoints()
{

}

bool Scale::LinearTransformation::allow(const ScreenPoints &aScreenPoints, const FloatRange &aLogicRange) const
{
    // Для линейного преобразования доступны любые валидные параметры
    return aScreenPoints.isValid() && aLogicRange.isValid();
}

bool Scale::LinearTransformation::initialize(const ScreenPoints &aScreenPoints, const FloatRange &aLogicRange)
{
    bool result = false;
    if (allow(aScreenPoints, aLogicRange))
    {
        a = (aLogicRange.min - aLogicRange.max) / (aScreenPoints.pointAtMin - aScreenPoints.pointAtMax);
        b = aLogicRange.max - a*aScreenPoints.pointAtMax;
        screenPoints = aScreenPoints;
        result = true;
    }
    return result;
}

bool Scale::LinearTransformation::isReady() const
{
    return (!isNaN(a)) && (!isNaN(b));
}

double Scale::LinearTransformation::screenToLogic(double screen) const
{
    if (!isReady())
    {
        return getNaN();
    }
    else
    {
        return screen*a + b;
    }
}

double Scale::LinearTransformation::logicToScreen(double logic) const
{
    if (!isReady())
    {
        return getNaN();
    }
    else if (isSingleValue())
    {
        return (screenPoints.pointAtMin + screenPoints.pointAtMax) / 2.0;
    }
    else
    {
        return (logic - b) / a;
    }
}

double Scale::LinearTransformation::worstScaleCoef() const
{
    if ((!isReady()) || (isSingleValue()))
    {
        return getNaN();
    }
    else
    {
        return 1.0 / a;
    }
}

bool Scale::LinearTransformation::isSingleValue() const
{
    return isZero(a);
}

Scale::ExponentialTransformation::ExponentialTransformation()
    : b(getNaN())
    , c(getNaN())
    , innerRange()
{
}

bool Scale::ExponentialTransformation::allow(const ScreenPoints &aScreenPoints, const FloatRange &aLogicRange) const
{
    return (aScreenPoints.isValid() && (aLogicRange.isValid()) && (aLogicRange.min >= 0));
}

bool Scale::ExponentialTransformation::initialize(const ScreenPoints &aScreenPoints, const FloatRange &aLogicRange)
{
    const double exponentialScaleMaxRatio = 40.0;

    bool result = false;
    if (allow(aScreenPoints, aLogicRange))
    {
        // Внутренний диапазон - минимльное значение не должно быть меньше максимального в exponentialScaleMaxRatio раз
        innerRange.max = aLogicRange.max;
        innerRange.min = qMax(aLogicRange.min, aLogicRange.max / exponentialScaleMaxRatio);
        // Рассчет параметров на основе внутреннего диапазона
        c = exp(log(innerRange.max/innerRange.min)/(aScreenPoints.pointAtMax - aScreenPoints.pointAtMin));
        b = innerRange.min/pow(c, aScreenPoints.pointAtMin);
        result = true;
    }
    return result;
}

bool Scale::ExponentialTransformation::isReady() const
{
    return (!isNaN(b)) && (!isNaN(c));
}

double Scale::ExponentialTransformation::screenToLogic(double screen) const
{
    if (!isReady())
    {
        return getNaN();
    }
    else
    {
        return b * pow(c, screen);
    }
}

double Scale::ExponentialTransformation::logicToScreen(double logic) const
{
    if (!isReady())
    {
        return getNaN();
    }
    else
    {
        // Значение, которое гарантировано не завалит систему
        double correctLogic = qMax(logic, innerRange.min);
        // Расчет на основе этого значения
        return (log(correctLogic) - log(b)) / log(c);
    }
}

double Scale::ExponentialTransformation::worstScaleCoef() const
{
    if (!isReady())
    {
        return getNaN();
    }
    else
    {
        return 1 / (log(c) * innerRange.max);
    }
}


//******************************************************************************
/*!
\struct FloatScaleMark
*/
//******************************************************************************

FloatScaleMark::FloatScaleMark()
    : value(getNaN())
    , text()
    , position(getNaN())
{

}

FloatScaleMark::FloatScaleMark(double aValue, const QString &aText, double aPosition)
    : value(aValue)
    , text(aText)
    , position(aPosition)
{

}

bool FloatScaleMark::isValid() const
{
    return (!isNaN(value)) && (!text.isEmpty()) && (!isNaN(position));
}


//******************************************************************************
/*!
\class FloatScale
*/
//******************************************************************************

FloatScale::FloatScale()
    : Scale()
    , m_step(0)
    , m_stepPrecision(0)
    , m_stepNumeralFormat()
    , m_markList()
{

}

FloatScaleMarkList FloatScale::markList() const
{
    return m_markList;
}

void FloatScale::computeParams()
{
    computeStep();
    computeMarkList();
}

int FloatScale::stepPrecision(double step, int tuneDigits) const
{
    return tuneDigits - floor(log(step) / log(10.0));
}

FloatRange FloatScale::stepRange(double step) const
{
    FloatRange result;
    result << step * ceil(screenToLogic(screenPoints().minMarkPoint) / step);
    result << step * floor(screenToLogic(screenPoints().maxMarkPoint) / step);
    return result;
}

bool FloatScale::isStepAvailable(double step, int tuneDigits) const
{
    Numeral numeral(NumeralFormat(false, true, stepPrecision(step, tuneDigits), false, false));
    FloatRange range = stepRange(step);
    double distanceBetween = step * abs(worstScaleCoef());
    return
            isStepStringAvailable(numeral.toString(range.min), distanceBetween) &&
            isStepStringAvailable(numeral.toString(range.max), distanceBetween);
}

void FloatScale::findStep(double min, double max)
{
    m_step = 0;
    if (max-min > FLT_EPSILON)
    {
        int p = ceil(log(max-min) / log(10.0));
        double delta = pow(10.0, p);
        int tuneDigits = 0;
        double nextDelta = pow(10.0, p-1);
        double prevDelta = delta;
        int prevTuneDigits = tuneDigits;
        while (isStepAvailable(delta, tuneDigits) && (delta > FLT_EPSILON))
        {
            prevDelta = delta;
            prevTuneDigits = tuneDigits;
            if (delta > nextDelta * 5.5)
            {
                delta = nextDelta * 5;
                tuneDigits = 0;
            }
            else if (delta > nextDelta * 2.6)
            {
                delta = nextDelta * 2.5;
                tuneDigits = 1;
            }
            else if (delta > nextDelta * 2.02)
            {
                delta = nextDelta * 2;
                tuneDigits = 0;
            }
            else
            {
                p = p - 1;
                delta = pow(10.0, p);
                tuneDigits = 0;
                nextDelta = pow(10.0, p-1);
            }
        }
        m_step = prevDelta;
        m_stepPrecision = stepPrecision(prevDelta, prevTuneDigits);
    }
}

void FloatScale::computeStep()
{
    if (!logicRange().isValid())
    {
        m_step = 0;
        m_stepNumeralFormat.clear();
    }
    else if (!logicRange().isSingleValue())
    {
        findStep(logicRange().min, logicRange().max);
        m_stepNumeralFormat = NumeralFormat(false, true, m_stepPrecision, false, false);
    }
}

void FloatScale::computeMarkList()
{
    m_markList.clear();
    if (!isReady())
    {
        return;
    }
    if ((logicRange().isSingleValue()) || (m_step <= FLT_EPSILON*2.0))
    {
        // Единственная метка на шкале
        double value = logicRange().singleValue();
        m_markList << FloatScaleMark(value, Numeral::format(value), logicToScreen(value));
    }
    else
    {
        // Диапазон значений на шкале
        FloatRange range = stepRange(m_step);
        for (double value = range.min; ((value <= range.max) && (m_markList.count() < criticalMarkCount)); value += m_step)
        {
            m_markList << FloatScaleMark(value, Numeral::format(value, m_stepNumeralFormat), logicToScreen(value));
        }
    }
}


//******************************************************************************
/*!
\struct DateTimeScaleStep
*/
//******************************************************************************

DateTimeScaleStep::DateTimeScaleStep()
    : type(Minute)
    , count(1)
{
}

DateTimeScaleStep::DateTimeScaleStep(Type aType, int aCount)
    : type(aType)
    , count(aCount)
{
}

DateTimeScaleStep::DateTimeScaleStep(const DateTimeScaleStep &another)
    : type(another.type)
    , count(another.count)
{
}

bool DateTimeScaleStep::isIntraday() const
{
    return ((type == Minute) || (type == Hour));
}

int DateTimeScaleStep::seconds() const
{
    int result = 0;
    if (isIntraday())
    {
        switch (type)
        {
        case Minute:
            result = count*60;
            break;
        case Hour:
            result = count*60*60;
            break;
        };
    }
    else
    {
        QDateTime current = QDateTime::currentDateTime();
        QDateTime next = add(current, 1);
        result = current.secsTo(next);
    }
    return result;
}

QDateTime DateTimeScaleStep::floorValue(const QDateTime &baseValue) const
{
    QDateTime result;
    if (isIntraday())
    {
        int secsTo = QTime(0, 0).secsTo(baseValue.time());
        int interval = seconds();
        int intervals = secsTo / interval;
        result = QDateTime(baseValue.date(), QTime(0, 0).addSecs(intervals*interval));
    }
    else if ((type == Day) || (type == Week))
    {
        QDate anchor(2007, 01, 01); // понедельник
        int days = anchor.daysTo(baseValue.date());
        int interval = (type == Week)? (count*7) : count;
        int intervals = days / interval;
        if ((days % interval) < 0)
        {
            intervals--;
        }
        result = QDateTime(anchor.addDays(interval*intervals));
    }
    else if (type == Month)
    {
        int months = baseValue.date().month() - 1;
        int intervals = months / count;
        result = QDateTime(QDate(baseValue.date().year(), intervals*count + 1, 1));
    }
    else if (type == Quarter)
    {
        int months = baseValue.date().month() - 1;
        int intervals = months / count / 3;
        result = QDateTime(QDate(baseValue.date().year(), intervals*count*3 + 1, 1));
    }
    else if (type == Year)
    {
        int years = baseValue.date().year();
        int intervals = years / count;
        result = QDateTime(QDate(intervals*count, 1, 1));
    }
    return result;
}

QDateTime DateTimeScaleStep::ceilValue(const QDateTime &baseValue) const
{
    QDateTime result = floorValue(baseValue);
    if (result < baseValue)
    {
        result = add(result, 1);
    }
    return result;
}

QDateTime DateTimeScaleStep::add(const QDateTime &startValue, int intervals) const
{
    QDateTime result;
    if (isIntraday())
    {
        result = startValue.addSecs(seconds()*intervals);
    }
    else if ((type == Day) || (type == Week))
    {
        int interval = (type == Week)? (count*7) : count;
        result = startValue.addDays(interval*intervals);
    }
    else if (type == Month)
    {
        result = startValue.addMonths(count*intervals);
    }
    else if (type == Quarter)
    {
        result = startValue.addMonths(count*intervals*3);
    }
    else if (type == Year)
    {
        result = startValue.addYears(count*intervals);
    }
    return result;
}

QString DateTimeScaleStep::format(const QDateTime &testValue) const
{
    QString result = "hh:mm dd MMM yyyyy";
    bool isCurrentDate = (testValue.date() == QDate::currentDate());
    bool isCurrentYear = (testValue.date().year() == QDate::currentDate().year());
    if ((type == Minute) || (type == Hour))
    {
        if (isCurrentDate)
        {
            return "hh:mm";
        }
        else if (isCurrentYear)
        {
            return "hh:mm dd MMM";
        }
        else
        {
            return "hh:mm dd MMM yyyy";
        }
    }
    else if ((type == Day) || (type == Week))
    {
        if (isCurrentYear)
        {
            return "dd MMM";
        }
        else
        {
            return "dd MMM yyyy";
        }
    }
    else if ((type == Month) || (type == Quarter))
    {
        if (isCurrentYear)
        {
            return "MMM";
        }
        else
        {
            return "MMM yyyy";
        }
    }
    else if (type == Year)
    {
        return "yyyy";
    }
    return result;
}

QString DateTimeScaleStep::toString() const
{
    QString typeSt;
    switch (type)
    {
    case Minute:
        typeSt = "Minute";
        break;
    case Hour:
        typeSt = "Hour";
        break;
    case Day:
        typeSt = "Day";
        break;
    case Week:
        typeSt = "Week";
        break;
    case Month:
        typeSt = "Month";
        break;
    case Quarter:
        typeSt = "Quarter";
        break;
    case Year:
        typeSt = "Year";
        break;
    };
    return QString("%1:%2").arg(typeSt).arg(count);
}


//******************************************************************************
/*!
\struct DateTimeScaleMark
*/
//******************************************************************************

DateTimeScaleMark::DateTimeScaleMark()
    : value()
    , text()
    , position(getNaN())
{

}

DateTimeScaleMark::DateTimeScaleMark(const QDateTime &aValue, const QString &aText, double aPosition)
    : value(aValue)
    , text(aText)
    , position(aPosition)
{

}

bool DateTimeScaleMark::isValid() const
{
    return (value.isValid()) && (!text.isEmpty()) && (!isNaN(position));
}


//******************************************************************************
/*!
\class DateTimeScale
*/
//******************************************************************************

DateTimeScale::DateTimeScale()
    : Scale()
    , m_possibleSteps()
    , m_values()
    , m_intradayFlag(true)
    , m_step()
    , m_markList()
{
    initializePossibleSteps();
}

void DateTimeScale::setValues(const QList<QDateTime> &value)
{
    m_values = value;
    qSort(m_values);
    setLogicRange(FloatRange(0, m_values.count()-1));
    changed();
}

QList<QDateTime> DateTimeScale::values() const
{
    return m_values;
}

void DateTimeScale::setIntradayFlag(bool value)
{
    if (m_intradayFlag != value)
    {
        m_intradayFlag = value;
        changed();
    }
}

bool DateTimeScale::intradayFlag() const
{
    return m_intradayFlag;
}

double DateTimeScale::dateTimeToLogic(const QDateTime &dateTime) const
{
    double result = getNaN();
    if (!m_values.isEmpty())
    {
        QList<QDateTime>::const_iterator iter = qLowerBound(m_values.constBegin(), m_values.constEnd(), dateTime);
        return qBound(0, iter - m_values.constBegin(), m_values.count()-1);
    }
    return result;
}

double DateTimeScale::dateTimeToScreen(const QDateTime &dateTime) const
{
    return logicToScreen(dateTimeToLogic(dateTime));
}

QDateTime DateTimeScale::logicToDateTime(double logic) const
{
    QDateTime result;
    if (!m_values.isEmpty())
    {
        return m_values[qBound(0, qRound(logic), m_values.count()-1)];
    }
    return result;
}

QDateTime DateTimeScale::screenToDateTime(double screen) const
{
    return logicToDateTime(screenToLogic(screen));
}

DateTimeRange DateTimeScale::dateTimeRange() const
{
    DateTimeRange result;
    if (!m_values.isEmpty())
    {
        result << m_values.first();
        result << m_values.last();
    }
    return result;
}

DateTimeScaleMarkList DateTimeScale::markList() const
{
    return m_markList;
}

void DateTimeScale::computeParams()
{
    computeStep();
    computeMarkList();
}

void DateTimeScale::initializePossibleSteps()
{
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Minute, 1));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Minute, 2));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Minute, 4));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Minute, 5));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Minute, 10));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Minute, 15));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Minute, 20));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Minute, 30));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Hour, 1));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Hour, 2));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Hour, 3));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Hour, 4));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Hour, 6));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Hour, 12));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Day, 1));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Week, 1));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Week, 2));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Month, 1));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Month, 2));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Quarter, 1));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Quarter, 2));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Year, 1));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Year, 2));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Year, 4));
    m_possibleSteps.append(DateTimeScaleStep(DateTimeScaleStep::Year, 8));
}

DateTimeRange DateTimeScale::stepRange(const DateTimeScaleStep &step) const
{
    DateTimeRange r = dateTimeRange();
    return DateTimeRange(step.ceilValue(r.min), step.floorValue(r.max));
}

bool DateTimeScale::isStepAvailable(const DateTimeScaleStep &step) const
{
    if ((step.isIntraday()) && (!intradayFlag()))
    {
        return false;
    }

    DateTimeRange range = dateTimeRange();
    double markCount = double(range.secondsSpan()) / double(step.seconds());
    double logicLength = double(values().count()) / markCount;
    double distanceBetween = logicLength * worstScaleCoef();
    return
            isStepStringAvailable(step.format(range.min), distanceBetween) &&
            isStepStringAvailable(step.format(range.max), distanceBetween);
}

void DateTimeScale::computeStep()
{
    // Шаг по умолчанию - максимальный
    if (intradayFlag())
    {
        m_step = DateTimeScaleStep(DateTimeScaleStep::Minute, 1);
    }
    else
    {
        m_step = DateTimeScaleStep(DateTimeScaleStep::Day, 1);
    }

    DateTimeRange r = dateTimeRange();
    if (r.secondsSpan() > 0)
    {
        // Есть диапазон значений. Подбор шага.
        foreach (const DateTimeScaleStep &step, m_possibleSteps)
        {
            if (isStepAvailable(step))
            {
                m_step = step;
                break;
            }
        }
    }
}

void DateTimeScale::computeMarkList()
{
    m_markList.clear();

    DateTimeRange r = dateTimeRange();
    if (!r.isValid())
    {
        return;
    }

    if (r.isSingleValue())
    {
        QString format = m_step.format(r.min);
        m_markList << DateTimeScaleMark(r.min, r.min.toString(format), dateTimeToScreen(r.min));
    }
    else
    {
        DateTimeRange range = stepRange(m_step);
        QString format = qMax(m_step.format(range.min), m_step.format(range.max));
        QDateTime currentDateTime = range.max;
        int intervals = 0;
        QFontMetricsF fm(font());
        while (currentDateTime >= range.min)
        {
            DateTimeScaleMark currentMark(currentDateTime, currentDateTime.toString(format), dateTimeToScreen(currentDateTime));
            double textSize = (orientation() == Qt::Horizontal) ? fm.width(currentMark.text) : fm.height();
            if ((m_markList.isEmpty()) || (m_markList.first().position >= currentMark.position + textSize))
            {
                m_markList.insert(0, currentMark);
            }
            intervals--;
            currentDateTime = m_step.add(range.max, intervals);
        }
    }
}
