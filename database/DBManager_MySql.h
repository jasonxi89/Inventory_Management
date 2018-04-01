#pragma once

#include "DBManager.h"
#include <QSqlDatabase>

class DBManager_MySql:public DBManager
{
public:
    DBManager_MySql(void);
	~DBManager_MySql(void);	 

	virtual bool Init();	
	void ClearDatabase(); //删除所有数据表
    void SetHost(QString host,QString database,QString name,QString pass);

	virtual bool IsExist(QString table);
    virtual bool Create(QString table);

	virtual bool Insert(DBRowData &data); //插入成功后，DBRowData的值可能会改变，存在自动生成的项
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

