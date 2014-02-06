#ifndef TIMELYACTION_H
#define TIMELYACTION_H

#include <QObject>
#include <QDateTime>

class TimelyAction : public QObject
{
	Q_OBJECT

public:
	TimelyAction(QObject *parent = NULL);
	void setPeriodInSeconds(int value);
	int periodInSeconds() const;

public slots:
	void actShortly();

signals:
	void triggered();

protected:
	void timerEvent(QTimerEvent *event);

private:
	int m_periodInSeconds;
	bool m_doActImmediately;
	int m_timerId;
	QDateTime m_lastTriggered;
};

#endif // TIMELYACTION_H
