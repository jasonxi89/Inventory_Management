#pragma once

#include "DBCommon.h"
#include "DBTableInfo.h"
#include "DBFilter.h"

struct sqlite3;
class DBManager
{
public:
	static DBManager *Instance();
    static void SetInstance(DBManager *inst);

	virtual bool Init() = 0;	

	virtual bool IsExist(QString table) = 0;
    virtual bool Create(QString table) = 0;

	virtual bool Insert(DBRowData &data) = 0; //����ɹ���DBRowData��ֵ���ܻ�ı䣬�����Զ����ɵ���
    virtual bool Select(QString table,QVector<DBRowData> &data_list,DBFilter filter = DBFilter()) = 0;
	virtual bool Select(QString table,QVector<DBRowData> &data_list,const QStringList &name_list,DBFilter filter = DBFilter()) = 0;
	virtual bool SelectOne(DBRowData &data,DBFilter filter = DBFilter()) = 0;	
    virtual bool Delete(const DBRowData &data) = 0;	
    virtual bool Update(const DBRowData &data,DBFilter filter = DBFilter()) = 0;	
    
	virtual QString GetErrorStr() = 0;

private:		    
    static DBManager *g_inst;	
};

