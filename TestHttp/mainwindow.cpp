#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myhttp.h"

MainWindow* g_MainWindow = NULL;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->SendBtn, SIGNAL(clicked()), this, SLOT(OnSend()));
    g_MainWindow = this;
}

MainWindow::~MainWindow()
{
    delete ui;
}

pthread_t  g_contid;

void MainWindow::OnSend()
{
    ui->resultText->setText("");
    MainWindow *pWindow = (MainWindow *)g_MainWindow;
    int run_count = 0;
    QString strUrl = pWindow->ui->urlText->toPlainText();
    while (run_count < 40)
    {
        myHttp http;
        http.Get(strUrl);
        char buff[1024] = {0};
        http.GetData(buff);
        QString strRet(buff);
        pWindow->ui->resultText->setText(strRet);
    }
}
