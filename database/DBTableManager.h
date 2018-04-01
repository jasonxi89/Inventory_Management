#pragma once

#include "DBCommon.h"
#include "DBTableInfo.h"

class DBDataGen
{
public:
	virtual void Init(QString table,QString row);
    virtual DBData Generate(bool next = false) = 0;
};

class DBAutoGen:public DBDataGen
{
public:    
    DBAutoGen();

    virtual void Init(QString table,QString row);
    virtual DBData Generate(bool next);

    void SetModel(QString model,bool haveDate);
    void SetSeqPos(int s,int len);

protected:    
    QString modelStr;
    bool haveDate;
    int mStart,mLen;
    int mId;
};

class DBTimeGen:public DBDataGen
{    
	virtual DBData Generate(bool next);
};

class DBDateGen:public DBDataGen
{    
	virtual DBData Generate(bool next);  
};

class DBDateTimeGen:public DBDataGen
{ 
	virtual DBData Generate(bool next);
};

class DBTableManager
{
public:
	static DBTableManager *Instance();
	
	void RegistTable(QString table_name,DBTableInfo info);
	const DBTableInfo &GetTableInfo(QString table_name);
	const QMap<QString,DBTableInfo> &GetTableMap();

    void InitGen();
	DBDataGen *GetDataGen(const QString &table,const QString &row);
	void SetDataGen(const QString &table,const QString &row,DBDataGen *gen);    

    void SetDataLimit(QString table_name,QString para_name,DBDataLimit limit);

private:
	DBTableManager(void);
	~DBTableManager(void);

	QMap<QString,DBTableInfo> mTableMap;
	QMap<QString,DBDataGen*> mGenMap;
};
