#include "DBTableManager.h"
#include "DBManager.h"
#include <QDate>
#include <QTime>
#include <QDateTime>

void DBDataGen::Init(QString table,QString row)
{
    Q_UNUSED(table);
    Q_UNUSED(row);
}

DBAutoGen::DBAutoGen()
{
    haveDate = false;
    mStart = mLen = 0;
    mId = 1;
}

void DBAutoGen::Init(QString table,QString row)
{
    QStringList rowName;
    rowName.push_back(row);

    DBFilter filter;
    filter.Last(table);

    DBRowData data(table,rowName);
    if(DBManager::Instance()->SelectOne(data,filter))
    {
        QString str = data[0].ToString();
        str = str.mid(mStart,mLen);

        mId = str.toInt();
        mId++;
    }
}

void DBAutoGen::SetModel(QString model,bool transDate)
{
    modelStr = model;
    haveDate = transDate;
}

void DBAutoGen::SetSeqPos(int s,int len)
{
    mStart = s;
    mLen = len;
}

DBData DBAutoGen::Generate(bool next) 
{
    DBData data(TYPE_STRING);
    char format[16];
    char buffer[64];
    QString str = modelStr;
       
    sprintf(format,"%%0%dd",mLen);
    sprintf(buffer,format,mId);

    if(haveDate)
    {        
        str = QDateTime::currentDateTime().toString(str);        
    }
    str.replace(mStart,mLen,buffer);

    if(next)    
        mId++;  

    data.FromString(str);
    return data;
}

DBData DBTimeGen::Generate(bool next)
{
    Q_UNUSED(next);
    DBData data(TYPE_TIME);
    QString str = QTime::currentTime().toString("hh:mm:ss");

    data.FromString(str);
    return data;
}

DBData DBDateGen::Generate(bool next)
{
    Q_UNUSED(next);
    DBData data(TYPE_DATE);
    QString str = QDate::currentDate().toString("yyyyMMdd");

    data.FromString(str);
    return data;
}

DBData DBDateTimeGen::Generate(bool next)
{
    Q_UNUSED(next);
    DBData data(TYPE_DATETIME);
    QString str = QDateTime::currentDateTime().toString("yyyyMMdd hh:mm:ss");

    data.FromString(str);
    return data;
}

//DBTableManager
DBTableManager *DBTableManager::Instance()
{
	static DBTableManager inst;
	return &inst;
}

DBTableManager::DBTableManager(void)
{

}

DBTableManager::~DBTableManager(void)
{
	QMap<QString,DBDataGen*>::iterator it_gen = mGenMap.begin();
	while(it_gen != mGenMap.end())
	{
		delete it_gen.value();
		it_gen++;
	}
}

void DBTableManager::RegistTable(QString table_name,DBTableInfo info)
{
	mTableMap[table_name] = info;
}

const DBTableInfo &DBTableManager::GetTableInfo(QString table_name)
{
	Q_ASSERT(mTableMap.find(table_name) != mTableMap.end());
	return mTableMap[table_name];
}

const QMap<QString,DBTableInfo> &DBTableManager::GetTableMap()
{
	return mTableMap;
}

void DBTableManager::InitGen()
{
    QMap<QString,DBDataGen*>::iterator it_gen = mGenMap.begin();
	while(it_gen != mGenMap.end())
	{
        QStringList spec_str = it_gen.key().split(".");
		it_gen.value()->Init(spec_str[0],spec_str[1]);
		
        it_gen++;
	}

}

DBDataGen *DBTableManager::GetDataGen(const QString &table,const QString &row)
{
	QMap<QString,DBDataGen*>::iterator it_gen = mGenMap.find(table + "." + row);
	if(it_gen != mGenMap.end())
		return it_gen.value();

	return NULL;
}

void DBTableManager::SetDataGen(const QString &table,const QString &row,DBDataGen *gen)
{
	mGenMap[table + "." + row] = gen;
}
    

void DBTableManager::SetDataLimit(QString table_name,QString para_name,DBDataLimit limit)
{
    mTableMap[table_name].SetDataLimit(para_name,limit);
}