#include "cprojectlistitem.h"

#define SEC_PER_MIN (60)
#define SEC_PER_HOU (60 * SEC_PER_MIN)
#define TIME_ELAPSED(elapsed)                                       \
    (QString("%1:%2:%3")                                            \
    .arg(elapsed / SEC_PER_HOU, 2, 10, QChar('0'))                  \
    .arg((elapsed % SEC_PER_HOU) / SEC_PER_MIN, 2, 10, QChar('0'))  \
    .arg((elapsed % SEC_PER_MIN), 2, 10, QChar('0'))                \
    )

CProjectListItem::CProjectListItem(int secOff, QDate date, QString projGrp, QString projName, QString projTheme, QString projTopic, QString comment, QListWidget* parent) :
    QListWidgetItem("", parent),
    m_secCnt(secOff),
    m_date(date),
    m_group(projGrp),
    m_name(projName),
    m_theme(projTheme),
    m_topic(projTopic),
    m_comment(comment)
{
    // account for offset when using elapsed time
    setText(getUpdatedTitle());
}

QTime CProjectListItem::getTime() const
{
//    return m_time.addSecs(m_secOff).toString("HH:mm:ss");
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

QString CProjectListItem::getUpdatedTitle() const
{
    return QString(m_comment + " >> (" + TIME_ELAPSED(m_secCnt) + ")");
}

int CProjectListItem::getSecCnt() const
{
    return m_secCnt;
}

void CProjectListItem::setSecCnt(int secCnt)
{
    m_secCnt = secCnt;
}

void CProjectListItem::updateTitle()
{
    setText(getUpdatedTitle());
}
