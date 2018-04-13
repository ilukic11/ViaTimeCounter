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
// "   var row = document.getElementById(\"ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_lstProjects\");"
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

// project::select
// <option selected="selected" value="49adc4ee-8f9f-4e3f-84da-92c49baf09a8">AMOS Adaptations CT</option>
// onchange="javascript:setTimeout('__doPostBack(\'ctl00$m$g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d$ctl00$lstProjects\',\'\')', 0)"

// subProject::select
// <option selected="selected" value="f3c79609-8cf7-4668-b9c6-53ac8300a716">Actelion-114014</option>
// onchange="javascript:setTimeout('__doPostBack(\'ctl00$m$g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d$ctl00$lstSubProjects\',\'\')', 0)"

// activity::select
// <option selected="selected" value="33ee5550-8cd0-42b3-bb5d-07f32b42d2c7">Build Management                                  </option>
// onchange="javascript:setTimeout('__doPostBack(\'ctl00$m$g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d$ctl00$lstActivities\',\'\')', 0)"
const QString jsSelectTemplate = QStringLiteral("function selectOption()"
                                                "{"
                                                "   document.getElementById(\"%1\").selectedIndex = %2;"
                                                "   setTimeout(\'__doPostBack(\\\'%3\\\',\\\'\\\')\', 0);"
                                                "}"
                                                "selectOption();"
                                               );


// submit
// javascript:WebForm_DoPostBackWithOptions(new WebForm_PostBackOptions("ctl00$m$g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d$ctl00$btnAddTimeRecord", "", true, "", "", false, false))
const QString jsSubmitTemplate = QStringLiteral("function submit()"
                                                "{"
                                                "   WebForm_DoPostBackWithOptions(new WebForm_PostBackOptions(\"ctl00$m$g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d$ctl00$btnAddTimeRecord\", \"\", true, \"\", \"\", false, false))"
                                                "}"
                                                "submit();"
                                               );



typedef struct SElementId
{
    // default constructor needed for map
    SElementId() {}

    SElementId(const char * id, const char * selectId) {m_listId = id;m_listSelectId = selectId;}
    QString m_listId;
    QString m_listSelectId;
} SElementId;

const QMap<QString, SElementId> MainWindow::s_lists = {
    {"costlist", SElementId("ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_drpCostList", "")},
    {"project", SElementId("ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_lstProjects", "ctl00$m$g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d$ctl00$lstProjects")},
    {"subProject", SElementId("ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_lstSubProjects", "ctl00$m$g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d$ctl00$lstSubProjects")},
    {"activity", SElementId("ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_lstActivities", "ctl00$m$g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d$ctl00$lstActivities")},
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_page()
{
    ui->setupUi(this);

    connect(&m_page, SIGNAL(loadFinished(bool)), this, SLOT(pageLoadFinished(bool)));
    // authentication purposes
    connect(&m_page, SIGNAL(authenticationRequired(QUrl,QAuthenticator*)), this, SLOT(slotAuthentication(QUrl, QAuthenticator*)));

    // send request when combo box selection changes
//    connect(ui->m_costlistCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(slotComboIndexChanged(const QString &)));
//    connect(ui->m_projectsCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(slotComboIndexChanged(const QString &)));

    // currentIndexChanged is called on any event (user/programmatically) which causes infinite loop
    // highlight is called on user interaction each time mouse crosses over an item in a list
    // activated is called when user clicks on highlighted item
    connect(ui->m_costlistCombo, SIGNAL(activated(int)), this, SLOT(slotComboIndexChangedCosts(int)));
    connect(ui->m_projectsCombo, SIGNAL(activated(int)), this, SLOT(slotComboIndexChangedProjects(int)));
    connect(ui->m_subProjectsCombo, SIGNAL(activated(int)), this, SLOT(slotComboIndexChangedSubProjects(int)));
    connect(ui->m_activitiesCombo, SIGNAL(activated(int)), this, SLOT(slotComboIndexChangedActivity(int)));
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->dateEdit->setDate(QDate::currentDate());
}

QString MainWindow::generateJsFunction(const SElementId & elem)

{
    qDebug() << "=======\n" << QString(jsTemplate).arg(elem.m_listId);
    return QString(jsTemplate).arg(elem.m_listId);
}

QString MainWindow::generateJsSelectFunction(const SElementId & elem, const int index)
{
    QString ret = QString(jsSelectTemplate).arg(elem.m_listId).arg(index).arg(elem.m_listSelectId);
    qDebug() << "=======\n" << ret;
    return ret;
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

    if (ui->m_costlistCombo->count() > m_costInd && m_costInd > -1)
    {
        ui->m_costlistCombo->setCurrentIndex(m_costInd);
    }
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

    if (ui->m_projectsCombo->count() > m_projInd && m_projInd > -1)
    {
        ui->m_projectsCombo->setCurrentIndex(m_projInd);
    }
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

    if (ui->m_subProjectsCombo->count() > m_subpInd && m_subpInd > -1)
    {
        ui->m_subProjectsCombo->setCurrentIndex(m_subpInd);
    }
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

void MainWindow::jsCallbackSelectCostlist(const QVariant &v)
{
    // now parse the result
//    m_page.runJavaScript(generateJsFunction(s_lists["costlist"]), invoke(this, &MainWindow::jsCallbackCostlist));
    m_page.runJavaScript(generateJsFunction(s_lists["project"]), invoke(this, &MainWindow::jsCallbackProject));
}

void MainWindow::jsCallbackSelectProject(const QVariant &v)
{
    // now parse the result
    m_page.runJavaScript(generateJsFunction(s_lists["subProject"]), invoke(this, &MainWindow::jsCallbackSubProject));
}

void MainWindow::jsCallbackSelectSubProject(const QVariant &v)
{
    // now parse the result
    m_page.runJavaScript(generateJsFunction(s_lists["activity"]), invoke(this, &MainWindow::jsCallbackActivity));
}

void MainWindow::jsCallbackSelectActivity(const QVariant &v)
{
    // now parse the result
}

void MainWindow::jsSubmit(const QVariant &v)
{
    qDebug() << "Return from submit...";
}

void MainWindow::jsToday(const QVariant &v)
{
    qDebug() << "Return from today...";
}

// to parse HTML list, check:
// http://www.qtcentre.org/threads/65044-Get-Html-element-value-with-QWebEngine
void MainWindow::htmlReader(QString html)
{
    qDebug() << "---------------------\nHTML size: " << html.size();

    QString filename = QString("/home/ilukic/projects/web/html_logs/") + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH:mm:ss") + ".html";
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << html << endl;
    }
    else
    {
        qDebug() << "filename = " << filename << "\nHTML:\n" << html << "\n--------------------------------";
    }

    file.close();

//    m_page.runJavaScript(generateJsFunction(s_lists["project"]), invoke(this, &MainWindow::jsCallbackProject));

    // should cascade to another web page call
    // but purely for test purposes of parsing elements
    // The chaining mechanism will later on include selection
    // and triggering of sending another request and parsing results
//    m_page.runJavaScript(generateJsFunction(s_lists["subProject"]), invoke(this, &MainWindow::jsCallbackSubProject));
//    m_page.runJavaScript(generateJsFunction(s_lists["activity"]), invoke(this, &MainWindow::jsCallbackActivity));
    m_page.runJavaScript(generateJsFunction(s_lists["costlist"]), invoke(this, &MainWindow::jsCallbackCostlist));


//    auto children = m_page.children();
//    for (auto child : children)
//    {
//        qDebug() << "--------------------\nChild found:" << child;
//        qDebug() << "obj Info: ";
//        child->dumpObjectInfo();
//        for (auto propertyName : child->dynamicPropertyNames())
//        {
//            qDebug() << "+++ Property name: " << propertyName.toStdString().c_str();
//        }
//    }
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

void MainWindow::slotComboIndexChangedCosts(int index)
{
    qDebug() << "---------- Combo costs index changed: " << index;

    m_costInd = index;
    // select proper index on page and run js
    m_page.runJavaScript(generateJsSelectFunction(s_lists["costlist"], index), invoke(this, &MainWindow::jsCallbackSelectCostlist));
}

void MainWindow::slotComboIndexChangedProjects(int index)
{
    qDebug() << "---------- Combo project index changed: " << index;

    m_projInd = index;
    // select proper index on page and run js
    m_page.runJavaScript(generateJsSelectFunction(s_lists["project"], index), invoke(this, &MainWindow::jsCallbackSelectProject));
}

void MainWindow::slotComboIndexChangedSubProjects(int index)
{
    qDebug() << "---------- Combo subProject index changed: " << index;

    m_subpInd = index;
    // select proper index on page and run js
    m_page.runJavaScript(generateJsSelectFunction(s_lists["subProject"], index), invoke(this, &MainWindow::jsCallbackSelectSubProject));
}

void MainWindow::slotComboIndexChangedActivity(int index)
{
    qDebug() << "---------- Combo subProject index changed: " << index;

    m_subpInd = index;
    // select proper index on page and run js
    m_page.runJavaScript(generateJsSelectFunction(s_lists["activity"], index), invoke(this, &MainWindow::jsCallbackSelectActivity));
}


void MainWindow::slotComboIndexChanged(const QString & text)
{
    qDebug() << "---------- Combo index changed, text: " << text;
}

void MainWindow::slotComboIndexChanged(int index)
{
    qDebug() << "---------- Combo index changed, index: " << index;
}

// checkout:
// https://stackoverflow.com/questions/36680604/qwebenginepage-tohtml-returns-an-empty-string
void MainWindow::on_pushButton_clicked()
{
    QDate date(2017, 8, 13);
    auto duration = ui->m_timeDuration->time();
    int secOffset = duration.hour() * 3600 + duration.minute() * 60 + duration.second();
    auto item = new CProjectListItem(secOffset, date,
                                     ui->m_costlistCombo->currentText(), ui->m_projectsCombo->currentText(), ui->m_subProjectsCombo->currentText(), ui->m_activitiesCombo->currentText(),
                                     ui->m_teComment->toPlainText(), ui->listWidget);
    ui->listWidget->addItem(item);
}

void MainWindow::on_m_pbGetList_clicked()
{
    //    QString url("http://help.websiteos.com/websiteos/example_of_a_simple_html_page.htm");
        QString url("http://tctmweb/viatimroot/default.aspx");
    //    QString url("file:///home/ilukic/projects/web/tctmweb/viatimroot/default.aspx.html");
    //    QString url("file:///home/ilukic/projects/web/Home%20-%20ViaTim/1/Home%20-%20ViaTim.html");
        m_page.load(QUrl(url));
}

void MainWindow::on_m_pbSubmit_clicked()
{
    // temp, set time/comment
    m_page.runJavaScript(QString("document.getElementById('ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_txtDurationHours').value = '%1'").arg("1"));
    m_page.runJavaScript(QString("document.getElementById('ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_txtDurationMinutes').value = '%1'").arg("0"));
    m_page.runJavaScript(QString("document.getElementById('ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_txtComment').value = '%1'").arg(ui->m_teComment->toPlainText()));

    // temp, set date
    m_page.runJavaScript(QString("document.getElementById('ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_dtDate_dtDateDate').value = '%1'").arg("4/13/2018"));
//    m_page.runJavaScript("WebForm_DoPostBackWithOptions(new WebForm_PostBackOptions(\"ctl00$m$g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d$ctl00$Button1\", \"\", true, \"\", \"\", false, false))",
//                         invoke(this, &MainWindow::jsToday));
//    m_page.runJavaScript(QString("document.getElementById('ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_Button1').click();"));

    // submit form
//    m_page.runJavaScript(jsSubmitTemplate, invoke(this, &MainWindow::jsSubmit));
    m_page.runJavaScript(QString("document.getElementById('ctl00_m_g_9b5aac03_6991_48c4_bdd4_9bf8d083a73d_ctl00_btnAddTimeRecord').click();"));
}
