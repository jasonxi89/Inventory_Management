#pragma once

#include <QWidget>
#include <QVector>
#include "DBTableInfo.h"

namespace DBUi
{
    enum
    {
        INSERT,        
        EDIT,  
        BROWSER,
        CUSTOM,
    };
};

class DBUiSetup
{
public:    
	DBUiSetup();
	~DBUiSetup(void);

	void SetDBRowData(DBRowData *data,int type);
	DBRowData *GetDBRowData();
	
	void SetupWidget(QWidget *widget);
    void SetNameList(QStringList &nameList);	
    void SetWidgetList(QStringList &nameList,QVector<QWidget*> &widgetList);	

    QWidget *GetWidget(QString name);
    
    void SetWidgetAttr(const QString &name,bool show,bool edit);
	
	bool WidgetToData();
	void DataToWidget();

	QString GetErrorStr();	
private:
	int OptControlType(int idx,const DBDataInfo *data_info,const DBDataLimit *limit);
    void InitWidgetInfo();
    void UpdateWidgetAttr(int i);

	QStringList mNameList;
    QVector<bool>  mShowList;    //是否显示
    QVector<bool>  mEditList;    //是否允许编辑

	QVector<QWidget*> mWidgetList;		
    
	DBRowData *mData;	
	QString errorStr;
    int mType;
};

class QTableWidget;
class DBUiTableWidget
{
public:
    DBUiTableWidget();

    void initTable(QString table_name);
    QTableWidget *widget();
    void setWidget(QTableWidget *widget);    
    
    void addRow(const DBRowData &row);
    int findRow(const DBRowData &row);

    void deleteRow(const DBRowData &row);
    void updateRow(const DBRowData &row);    

private:
    QStringList nameList;
    QTableWidget *tableWidget;
    int keyIndex;
};