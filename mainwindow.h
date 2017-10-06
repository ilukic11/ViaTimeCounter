#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEnginePage>

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

    void jsCallback(const QVariant& v);


public slots:
    void pageLoadFinished(bool b);

    void slotAuthentication(const QUrl &requestUrl, QAuthenticator *authenticator);

private:
    Ui::MainWindow *ui;
    QWebEnginePage m_page;
};

#endif // MAINWINDOW_H
