#include "cprojectlistitem.h"

#define MS_PER_MIN (60 * 1000)
#define MS_PER_HOU (60 * MS_PER_MIN)
#define TIME_ELAPSED(time)                                                \
    (QString("%1:%2:%3")                                                  \
    .arg(time.elapsed() / MS_PER_HOU, 2, 10, QChar('0'))                  \
    .arg((time.elapsed() %  MS_PER_HOU) / MS_PER_MIN, 2, 10, QChar('0'))  \
    .arg((time.elapsed() %  MS_PER_MIN) / 1000, 2, 10, QChar('0'))        \
    )

CProjectListItem::CProjectListItem(QTime time, QDate date, QString projGrp, QString projName, QString projTheme, QString projTopic, QString comment, QListWidget* parent) :
    QListWidgetItem(getUpdatedTitle(time, projName, projTheme, projTopic), parent),
    m_time(time),
    m_date(date),
    m_group(projGrp),
    m_name(projName),
    m_theme(projTheme),
    m_topic(projTopic),
    m_comment(comment)
{
}

QString CProjectListItem::getUpdatedTitle() const
{
    return getUpdatedTitle(m_time, m_name, m_theme, m_topic);
}


QTime CProjectListItem::getTime() const
{
    return m_time;
}

void CProjectListItem::setTime(const QTime &time)
{
    m_time = time;
}

QDate CProjectListItem::getDate() const
{
    return m_date;
}

void CProjectListItem::setDate(const QDate &date)
{
    m_date = date;
}

QString CProjectListItem::getGroup() const
{
    return m_group;
}

void CProjectListItem::setGroup(const QString &group)
{
    m_group = group;
}

QString CProjectListItem::getName() const
{
    return m_name;
}

void CProjectListItem::setName(const QString &name)
{
    m_name = name;
}

QString CProjectListItem::getTheme() const
{
    return m_theme;
}

void CProjectListItem::setTheme(const QString &theme)
{
    m_theme = theme;
}

QString CProjectListItem::getTopic() const
{
    return m_topic;
}

void CProjectListItem::setTopic(const QString &topic)
{
    m_topic = topic;
}

QString CProjectListItem::getComment() const
{
    return m_comment;
}

void CProjectListItem::setComment(const QString &comment)
{
    m_comment = comment;
}

QString CProjectListItem::getUpdatedTitle(QTime time, QString projName, QString projTheme, QString projTopic) const
{
    return QString(projName + " > " + projTheme + " > " + projTopic + " (" + TIME_ELAPSED(time) + ")");
}
