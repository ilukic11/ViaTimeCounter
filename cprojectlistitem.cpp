#include "cprojectlistitem.h"

#define MS_PER_MIN (60 * 1000)
#define MS_PER_HOU (60 * MS_PER_MIN)
//#define TIME_ELAPSED(time)                                                \
//    (QString("%1:%2:%3")                                                  \
//    .arg(time.elapsed() / MS_PER_HOU, 2, 10, QChar('0'))                  \
//    .arg((time.elapsed() %  MS_PER_HOU) / MS_PER_MIN, 2, 10, QChar('0'))  \
//    .arg((time.elapsed() %  MS_PER_MIN) / 1000, 2, 10, QChar('0'))        \
//    )

CProjectListItem::CProjectListItem(int secOff, QDate date, QString projGrp, QString projName, QString projTheme, QString projTopic, QString comment, QListWidget* parent) :
    QListWidgetItem("", parent),
    m_secOff(secOff),
    m_date(date),
    m_group(projGrp),
    m_name(projName),
    m_theme(projTheme),
    m_topic(projTopic),
    m_comment(comment)
{
    // 1s
    m_cntTimer.setInterval(1000);
    // connect timer
    connnect(&m_cntTimer, &QTimer::timeout, this, &CProjectListItem::secCnt);
    // call start
    QTimer::singleShot(0,
                       [this] { m_cntTimer.start(); });

    // account for offset when using elapsed time
    this->setText(getUpdatedTitle());
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
    return QString(m_comment + " >> (" + /*TIME_ELAPSED(time) +*/ ")");
}

void CProjectListItem::secCnt()
{
    qDebug() << "1 sec";
}
