#ifndef CLISTITEM_H
#define CLISTITEM_H

#include <QListWidgetItem>
#include <QTime>
#include <QDate>
#include <QTimer>

class CProjectListItem : public QListWidgetItem
{
public:
    CProjectListItem(int secOff, QDate date, QString projGrp, QString projName, QString projTheme, QString projTopic, QString comment, QListWidget* parent);

    QTime getTime() const;
    void setTime(const QTime &time);

    QDate getDate() const;
    void setDate(const QDate &date);

    QString getGroup() const;
    void setGroup(const QString &group);

    QString getName() const;
    void setName(const QString &name);

    QString getTheme() const;
    void setTheme(const QString &theme);

    QString getTopic() const;
    void setTopic(const QString &topic);

    QString getComment() const;
    void setComment(const QString &comment);

protected slots:
    void secCnt();

private:
    QString getUpdatedTitle() const;



    int m_secOff;
    QTime m_time;
    QDate m_date;
    QString m_group;
    QString m_name;
    QString m_theme;
    QString m_topic;
    QString m_comment;

    QTimer m_cntTimer;
};

#endif // CLISTITEM_H
