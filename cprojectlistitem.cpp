#include "cprojectlistitem.h"

#define SEC_PER_MIN (60)
#define SEC_PER_HOU (60 * SEC_PER_MIN)
#define TIME_ELAPSED(elapsed)                                       \
    (QString("%1:%2:%3")                                            \
    .arg(elapsed / SEC_PER_HOU, 2, 10, QChar('0'))                  \
    .arg((elapsed % SEC_PER_HOU) / SEC_PER_MIN, 2, 10, QChar('0'))  \
    .arg((elapsed % SEC_PER_MIN), 2, 10, QChar('0'))                \
    )

CProjectTableItem::CProjectTableItem(int secOff, QDate date, QString projGrp, QString projName, QString projTheme, QString projTopic, QString comment, QTableWidget *parent) :
    QTableWidgetItem(""),
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

QTime CProjectTableItem::getTime() const
{
//    return m_time.addSecs(m_secOff).toString("HH:mm:ss");
    return m_time;
}

void CProjectTableItem::setTime(const QTime &time)
{
    m_time = time;
}

QDate CProjectTableItem::getDate() const
{
    return m_date;
}

void CProjectTableItem::setDate(const QDate &date)
{
    m_date = date;
}

QString CProjectTableItem::getGroup() const
{
    return m_group;
}

void CProjectTableItem::setGroup(const QString &group)
{
    m_group = group;
}

QString CProjectTableItem::getName() const
{
    return m_name;
}

void CProjectTableItem::setName(const QString &name)
{
    m_name = name;
}

QString CProjectTableItem::getTheme() const
{
    return m_theme;
}

void CProjectTableItem::setTheme(const QString &theme)
{
    m_theme = theme;
}

QString CProjectTableItem::getTopic() const
{
    return m_topic;
}

void CProjectTableItem::setTopic(const QString &topic)
{
    m_topic = topic;
}

QString CProjectTableItem::getComment() const
{
    return m_comment;
}

void CProjectTableItem::setComment(const QString &comment)
{
    m_comment = comment;
}

QString CProjectTableItem::getUpdatedTitle() const
{
    return QString(m_comment + " >> (" + TIME_ELAPSED(m_secCnt) + ")");
}

int CProjectTableItem::getSecCnt() const
{
    return m_secCnt;
}

void CProjectTableItem::setSecCnt(int secCnt)
{
    m_secCnt = secCnt;
}

void CProjectTableItem::updateTitle()
{
    setText(getUpdatedTitle());
}
