#include "DBUiDlg.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

DBUiDlg::DBUiDlg(QWidget *parent)
    :QDialog(parent)
{
    mainWidget = new QWidget(this);

    QWidget *btnWidget = new QWidget(this);
    QPushButton *btnOk    = new QPushButton("OK",this);
    QPushButton *btnClose = new QPushButton("Cancel",this);

    QHBoxLayout *hlayout = new QHBoxLayout(this);
    hlayout->addStretch();
    hlayout->addWidget(btnOk);
    hlayout->addWidget(btnClose);
    btnWidget->setLayout(hlayout);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(mainWidget);
    layout->addWidget(btnWidget);

    connect(btnOk,SIGNAL(clicked()),this, SLOT(accept()));
    connect(btnClose,SIGNAL(clicked()),this, SLOT(reject()));
       
}

DBUiDlg::~DBUiDlg()
{

}

void DBUiDlg::Init(DBRowData data,int type)
{
    mData = data;

    dbUi.SetDBRowData(&mData,type);
    dbUi.SetupWidget(mainWidget);    
}

DBRowData DBUiDlg::rowData()
{
    return mData;
}

void DBUiDlg::accept()
{
    if(dbUi.WidgetToData())
        QDialog::accept();
    else
        QMessageBox::information(this,"",dbUi.GetErrorStr());        
}