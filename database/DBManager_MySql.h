#pragma once

#include "DBManager.h"
#include <QSqlDatabase>

class DBManager_MySql:public DBManager
{
public:
    DBManager_MySql(void);
	~DBManager_MySql(void);	 

	virtual bool Init();	
	void ClearDatabase(); //ɾ���������ݱ�
    void SetHost(QString host,QString database,QString name,QString pass);

	virtual bool IsExist(QString table);
    virtual bool Create(QString table);

	virtual bool Insert(DBRowData &data); //����ɹ���DBRowData��ֵ���ܻ�ı䣬�����Զ����ɵ���
    virtual bool Select(QString table,QVector<DBRowData> &data_list,DBFilter filter = DBFilter());
	virtual bool Select(QString table,QVector<DBRowData> &data_list,const QStringList &name_list,DBFilter filter = DBFilter());
	virtual bool SelectOne(DBRowData &data,DBFilter filter = DBFilter());	
    virtual bool Delete(const DBRowData &data);	
    virtual bool Update(const DBRowData &data,DBFilter filter = DBFilter());	
    
	virtual QString GetErrorStr();

private:	          
    QString mHost,mDataBase,mName,mPass;
	QString mErrorStr;
    QSqlDatabase mDB;
};

