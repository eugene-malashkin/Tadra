#include "timelyaction.h"
#include <QTimerEvent>

TimelyAction::TimelyAction(QObject *parent)
    :QObject(parent)
    ,m_periodInSeconds(0)
    ,m_doActImmediately(false)
    ,m_timerId(0)
    ,m_lastTriggered()
{
    m_timerId = startTimer(50);
}

void TimelyAction::setPeriodInSeconds(int value)
{
	m_periodInSeconds = value;
}

int TimelyAction::periodInSeconds() const
{
	return m_periodInSeconds;
}

void TimelyAction::actShortly()
{
	m_doActImmediately = true;
}

void TimelyAction::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_timerId)
	{
		QDateTime current = QDateTime::currentDateTime();
		if (
			(m_doActImmediately) || 
			(!m_lastTriggered.isValid()) || 
			((m_periodInSeconds > 0) && (m_lastTriggered.addSecs(m_periodInSeconds) <= current))
			)
		{
			m_lastTriggered = current;
			m_doActImmediately = false;
			emit triggered();
		}
	}
}
