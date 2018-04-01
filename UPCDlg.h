#pragma once
#include <QDialog>
#include "DBUiDlg.h"

struct UPC_INFO
{
    QString category;
    QString name;
    QString company;
    QString memo;
};

class QLineEdit;
class UPCDialog : public DBUiDlg
{
    Q_OBJECT

public:
    UPCDialog(QWidget *parent = 0);
	~UPCDialog();
    
    virtual void Init(DBRowData data,int type);
    void setUPCInfo(QMap<QString,UPC_INFO> upcMap);

protected slots:
    void onUPCChanged(const QString &text);

protected:
    virtual void accept();

    QVector<QLineEdit*> upcLabelList;
    QMap<QString,UPC_INFO> mUPCMap;
};