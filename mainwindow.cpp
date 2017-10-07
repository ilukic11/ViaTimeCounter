#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cprojectlistitem.h"

#include <QThread>
#include <QWebEnginePage>
#include <QAuthenticator>

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


// ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_lstProjects
const QString jsTemplate = QStringLiteral("function myFunction()"
                                            "{"
                                            "   var row = document.getElementById(\"%1\");"
                                            "   if (row != null)"
                                            "   {"
                                            "       var cells = row.getElementsByTagName('option');"
                                            "       var elements = [];"
                                            "       for (var val of cells)"
                                            "       {"
                                            "           console.log(val.name + \" : \" + val.value);"
                                            "           elements.push(val.innerText);"
                                            "       }"
                                            "       return elements;"
                                            "   }"
                                            "   else"
                                            "   {"
                                            "       return 'NullPtr!';"
                                            "   }"
                                            "}"
                                            ""
                                            "myFunction();"
                                         );



// "   var row = document.getElementById(\"ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_lstProjects\");"

const QMap<QString, QString> MainWindow::s_lists = {
    {"costlist", "ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_drpCostList"},
    {"project", "ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_lstProjects"},
    {"subProject", "ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_lstSubProjects"},
    {"activity", "ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_lstActivities"},
};

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

QString MainWindow::generateJsFunction(const QString & element)
{
    qDebug() << "=======\n" << QString(jsTemplate).arg(element);
    return QString(jsTemplate).arg(element);
}

void MainWindow::jsCallbackCostlist(const QVariant &v)
{
    QStringList result = v.toStringList();
    qDebug() << "JS Costlist result size:";
    qDebug() << result.size();
    qDebug() << "Values:";
    qDebug() << result;

    // set combo box list
    ui->m_costlistCombo->clear();
    ui->m_costlistCombo->addItems(result);
}

void MainWindow::jsCallbackProject(const QVariant &v)
{
    QStringList result = v.toStringList();
    qDebug() << "JS Projects result size:";
    qDebug() << result.size();
    qDebug() << "Values:";
    qDebug() << result;

    // set combo box list
    ui->m_projectsCombo->clear();
    ui->m_projectsCombo->addItems(result);
}

void MainWindow::jsCallbackSubProject(const QVariant &v)
{
    QStringList result = v.toStringList();
    qDebug() << "JS SubProjects result size:";
    qDebug() << result.size();
    qDebug() << "Values:";
    qDebug() << result;

    // set combo box list
    ui->m_subProjectsCombo->clear();
    ui->m_subProjectsCombo->addItems(result);
}

void MainWindow::jsCallbackActivity(const QVariant &v)
{
    QStringList result = v.toStringList();
    qDebug() << "JS Activity result size:";
    qDebug() << result.size();
    qDebug() << "Values:";
    qDebug() << result;

    // set combo box list
    ui->m_activitiesCombo->clear();
    ui->m_activitiesCombo->addItems(result);
}


//QString jsFunction =    "document.title"
//                        ;

// to parse HTML list, check:
// http://www.qtcentre.org/threads/65044-Get-Html-element-value-with-QWebEngine
void MainWindow::htmlReader(QString html)
{
    qDebug() << "---------------------\nHTML size: " << html.size();
    qDebug() << "HTML:\n" << html << "\n--------------------------------";

    m_page.runJavaScript(generateJsFunction(s_lists["project"]), invoke(this, &MainWindow::jsCallbackProject));

    // should cascade to another web page call
    // but purely for test purposes of parsing elements
    // The chaining mechanism will later on include selection
    // and triggering of sending another request and parsing results
    m_page.runJavaScript(generateJsFunction(s_lists["subProject"]), invoke(this, &MainWindow::jsCallbackSubProject));
    m_page.runJavaScript(generateJsFunction(s_lists["activity"]), invoke(this, &MainWindow::jsCallbackActivity));
    m_page.runJavaScript(generateJsFunction(s_lists["costlist"]), invoke(this, &MainWindow::jsCallbackCostlist));


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

void MainWindow::slotAuthentication(const QUrl &requestUrl, QAuthenticator *authenticator)
{
    authenticator->setUser(ui->m_username->text());
    authenticator->setPassword(ui->m_password->text());
}

// checkout:
// https://stackoverflow.com/questions/36680604/qwebenginepage-tohtml-returns-an-empty-string
void MainWindow::on_pushButton_clicked()
{
//    QString url("http://help.websiteos.com/websiteos/example_of_a_simple_html_page.htm");
//    QString url("http://tctmweb/viatimroot/default.aspx");
//    QString url("file:///home/ilukic/projects/web/tctmweb/viatimroot/default.aspx.html");
    QString url("file:///home/ilukic/projects/web/Home%20-%20ViaTim/1/Home%20-%20ViaTim.html");

    connect(&m_page, SIGNAL(loadFinished(bool)), this, SLOT(pageLoadFinished(bool)));
    // authentication purposes
    connect(&m_page, SIGNAL(authenticationRequired(QUrl,QAuthenticator*)), this, SLOT(slotAuthentication(QUrl, QAuthenticator*)));

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
