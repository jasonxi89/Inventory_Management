#pragma once

#include "DBManager.h"

struct sqlite3;
class DBManager_sqlite:public DBManager
{
public:
    DBManager_sqlite(void);
	~DBManager_sqlite(void);	 

	virtual bool Init();	

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
	sqlite3 *mDB;	
	QString mErrorStr;
};

