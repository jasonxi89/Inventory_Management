#pragma once
#include <QDialog>
#include "DBUiSetup.h"

class DBUiDlg : public QDialog
{
    Q_OBJECT

public:
    DBUiDlg(QWidget *parent = 0);
	~DBUiDlg();
    
    virtual void Init(DBRowData data,int type);
    DBRowData rowData();

protected slots:
    virtual void accept();

protected:
    DBRowData mData;
    DBUiSetup dbUi;

    QWidget *mainWidget;
};