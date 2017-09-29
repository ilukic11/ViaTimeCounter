#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cprojectlistitem.h"

#include <QThread>
#include <QWebEnginePage>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->dateEdit->setDate(QDate::currentDate());
}

void htmlReader(QString html)
{
    qDebug() << "HTML size: " << html.size();
    qDebug() << "HTML:\n" << html;
}

void MainWindow::on_pushButton_clicked()
{
    QString url("http://http://www.play-hookey.com/");
    QWebEnginePage page;
    page.load(QUrl(url));

    qDebug() << "Page content size: " << page.contentsSize().toSize();
    page.toHtml(htmlReader);

    static ulong listIndex = 1;

    QTime time;
    time.start();
    // random sleep
    QThread::sleep(2 * listIndex++);

    QDate date(2017, 8, 13);
    auto item = new CProjectListItem(time, date, "GRP", "Name", "Theme", "Topic", "Comment", ui->listWidget);
    ui->listWidget->addItem(item);
}
