#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEnginePage>

struct SElementId;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

protected:
    void htmlReader(QString html);

    static QString generateJsSelectFunction(const SElementId & elem, const int index);
    static QString generateJsFunction(const SElementId & elem);

    // callback methods, later on to be merged into one
    // and pass caller element to be able to determine
    // the target element for update
    void jsCallbackCostlist(const QVariant &v);
    void jsCallbackProject(const QVariant &v);
    void jsCallbackSubProject(const QVariant &v);
    void jsCallbackActivity(const QVariant &v);


public slots:
    void pageLoadFinished(bool b);

    void slotAuthentication(const QUrl &requestUrl, QAuthenticator *authenticator);

    void slotComboIndexChangedCosts(int index);
    void slotComboIndexChangedProjects(int index);
    void slotComboIndexChangedSubProjects(int index);

    void slotComboIndexChanged(const QString & text);
    void slotComboIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    QWebEnginePage m_page;

    static const QMap<QString, SElementId> s_lists;
    static QString s_username;
    static QString s_password;
};

#endif // MAINWINDOW_H
