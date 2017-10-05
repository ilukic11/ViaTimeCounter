#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cprojectlistitem.h"

#include <QThread>
#include <QWebEnginePage>

// ----------------------
// check:
// https://github.com/morphis/qtwebengine-snapshots/blob/master/examples/webenginewidgets/fancybrowser/mainwindow.cpp
template<typename Arg, typename R, typename C>
struct InvokeWrapper {
    R *receiver;
    void (C::*memberFun)(Arg);
    void operator()(Arg result) {
        (receiver->*memberFun)(result);
    }
};

template<typename Arg, typename R, typename C>
InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFun)(Arg))
{
    InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};
    return wrapper;
}
// ----------------------


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_page()
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


void MainWindow::jsCallback(const QVariant& v)
{
    qDebug() << "JS result:";
    qDebug() << v.toString();
}

// ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_lstProjects
QString jsFunction =    "function myFunction()"
                        "{"
                        "var row = document.getElementById(\"ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_lstProjects\");"
                        "if (row != null)"
                        "{"
                        "   var cells = row.getElementsByTagName('option');"
                        "}"
                        "else"
                        "{"
                        "   return 'NullPtr!';"
                        "}"
                        "return cells[0].innerText;"
                        "}"
                        ""
                        "myFunction();"
                        ;

// to parse HTML list, check:
// http://www.qtcentre.org/threads/65044-Get-Html-element-value-with-QWebEngine
void MainWindow::htmlReader(QString html)
{
    qDebug() << "---------------------\nHTML size: " << html.size();
    qDebug() << "HTML:\n" << html << "\n--------------------------------";

    m_page.runJavaScript(jsFunction, invoke(this, &MainWindow::jsCallback));

    auto children = m_page.children();
    for (auto child : children)
    {
        qDebug() << "--------------------\nChild found:" << child;
        qDebug() << "obj Info: ";
        child->dumpObjectInfo();
        for (auto propertyName : child->dynamicPropertyNames())
        {
            qDebug() << "+++ Property name: " << propertyName.toStdString().c_str();
        }
    }
}

void MainWindow::pageLoadFinished(bool b)
{
    qDebug() << "Page content size: " << m_page.contentsSize().toSize();
    m_page.toHtml(invoke(this, &MainWindow::htmlReader));
}

// checkout:
// https://stackoverflow.com/questions/36680604/qwebenginepage-tohtml-returns-an-empty-string
void MainWindow::on_pushButton_clicked()
{
    //    QString url("http://help.websiteos.com/websiteos/example_of_a_simple_html_page.htm");
    QString url("http://tctmweb/viatimroot/default.aspx");

    connect(&m_page, SIGNAL(loadFinished(bool)), this, SLOT(pageLoadFinished(bool)));

    m_page.load(QUrl(url));

    static ulong listIndex = 1;

    QTime time;
    time.start();
    // random sleep
    QThread::sleep(2 * listIndex++);

    QDate date(2017, 8, 13);
    auto item = new CProjectListItem(time, date, "GRP", "Name", "Theme", "Topic", "Comment", ui->listWidget);
    ui->listWidget->addItem(item);
}
