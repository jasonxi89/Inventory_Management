#include "DBManager_sqlite.h"
#include "DBTableManager.h"
#include "sqlite3.h"

DBManager_sqlite::DBManager_sqlite(void)
{
}

bool DBManager_sqlite::Init()
{
	const char *db_name = "DataBase.db3";	

	mDB = NULL;				    
    int ret = sqlite3_open(db_name, &mDB);
    if(ret == SQLITE_OK)
    {		
		return true;		
    }

	return false;
}

DBManager_sqlite::~DBManager_sqlite(void)
{
	sqlite3_close(mDB);
}

QString DBManager_sqlite::GetErrorStr()
{	
	return mErrorStr;
}

bool DBManager_sqlite::IsExist(QString table)
{
    bool exist = false;
    QString sql = "select count(*) from sqlite_master where type ='table' and name ='" +table+ "'";

    sqlite3_stmt *pVM;
    char zSQL[4096];
    const char *pzSql;
    strcpy(zSQL,sql.toStdString().c_str());

    int ret = sqlite3_prepare(mDB,zSQL,1024,&pVM,&pzSql);
    if(ret == SQLITE_OK)
    {
        ret = sqlite3_step(pVM);
        if(ret == SQLITE_ROW)
        {
            int count = sqlite3_column_int(pVM,0);
            if(count > 0)
                exist = true;
        }
        sqlite3_finalize(pVM);
    }

    return exist;
}

bool DBManager_sqlite::Create(QString table)
{
	const DBTableInfo &table_info = DBTableManager::Instance()->GetTableInfo(table);

	QString sql = "create table if not exists " + table + "(";
	const QStringList &nameList = table_info.GetNameList();
    int count = nameList.size();
    for(int i = 0; i < count; i++)
	{
		QString row_name = nameList[i];
		const DBDataInfo *data_info = table_info.GetDataInfo(row_name);

        QString row_type = DBDataType::typeToStr(data_info->dataType);
        if(row_type == "int")
            row_type = "INTEGER";

        sql += row_name + " ";		
		sql += row_type;		

		if(nameList[i] == table_info.GetKey())
        {
            if(data_info->autoID())
                sql += " PRIMARY KEY AUTOINCREMENT";
            else
			    sql += " PRIMARY KEY NOT NULL";
        }
        else
        {   
            if(data_info->autoID())
                sql += " identity(1,1) NOT NULL";
            if(data_info->unique())
                sql += " UNIQUE";
            if(data_info->mastHave())
                sql += " NOT NULL";
        }        
		if(i != count - 1)
			sql += ",";
	}	
	sql += ");";

	int ret = sqlite3_exec(mDB,sql.toStdString().c_str(),0,0,NULL);	
	if(ret != SQLITE_OK)
		mErrorStr = sqlite3_errmsg(mDB);
	return (ret == SQLITE_OK);
}

bool DBManager_sqlite::Insert(DBRowData &data)
{
	QString table = data.GetTableName();
	QString sql = "";
	QString str_data;
	QString row_name;
	QString row_value;
    bool has_autoid = false;    

    DBRowData bak_data = data;
	const QStringList &nameList = data.GetNameList();    

	int count = data.GetCount();    
	for(int i = 0; i < count; i++)
	{
        const DBDataInfo *data_info = data.GetDataInfo(i);
        if(data_info->autoID())
        {
            has_autoid = true;               
            str_data.clear();
            continue;
        }
        else
        {
		    str_data = data[i].ToString();
		    if(str_data.isEmpty() && data_info->autoGen())
		    {
			    DBDataGen *gen = DBTableManager::Instance()->GetDataGen(table,nameList[i]);
			    Q_ASSERT(gen);
                
			    DBData tmp = gen->Generate(true);
                str_data = tmp.ToString();            
		    }        
        }

        row_name += nameList[i];        
        if(str_data.isEmpty())
        {
            row_value += "NULL";
        }
        else		    
        {
		    row_value += "'" + str_data + "'";				    
        }

        if(i != count - 1)
	    {
		    row_name += ",";
		    row_value += ",";
	    }
	}
	sql += "insert into " + table + " (" + row_name + ")";
    sql += "values(" + row_value + ");";
	    
	int ret = sqlite3_exec(mDB,sql.toStdString().c_str(),0,0,NULL);
	if(ret != SQLITE_OK)
    {
		mErrorStr = sqlite3_errmsg(mDB);    
        data = bak_data;
    }
    else
    {
        if(has_autoid) //²éÑ¯×ÔÔöid
        {
            int rowid = sqlite3_last_insert_rowid(mDB);
            DBFilter filter;
            filter.Add("ROWID",QString::number(rowid));
            
            DBRowData tmp = data;
            if(SelectOne(tmp,filter))
                data = tmp;
        }
    }

    return (ret == SQLITE_OK);      
}

bool DBManager_sqlite::Delete(const DBRowData &data)
{
	QString table = data.GetTableName();
    QString sql = "DELETE FROM " + table;

    sql += " WHERE ";
    QString key = data.Key();
    if(!key.isEmpty())
		sql += key + "='" + data[key].ToString() + "'";
    else
    {
        const QStringList &nameList = data.GetNameList();
        int count = nameList.size();
        for(int i = 0; i < count; i++)
        {
            sql += nameList[i] + "='" + data[i].ToString() + "'";
            if(i != count - 1)
                sql += " AND ";
        }
    }
    
    int ret = sqlite3_exec(mDB,sql.toStdString().c_str(),0,0,NULL);
	if(ret != SQLITE_OK)
		mErrorStr = sqlite3_errmsg(mDB);
    return (ret == SQLITE_OK);	
}

		
bool DBManager_sqlite::Update(const DBRowData &data,DBFilter filter)
{
	QString table = data.GetTableName();
	QString sql = "UPDATE " + table + " SET ";

    const QStringList &nameList = data.GetNameList();
    QString key = data.Key();
    int count = nameList.size();
    bool hasKey = false;
	for(int i = 0; i < count; i++)
	{
        if(nameList[i] == key)
        {
            hasKey = true;
			continue;
        }

        sql += nameList[i] + "=";
		sql += "'";
        sql += data[i].ToString();
		sql += "'";
		if(i != count - 1)
			sql += ",";
	}
	sql += " WHERE ";
    if(filter.IsVaild())
        sql += filter.ToString();
    else if(hasKey && !key.isEmpty())
        sql += key + "='" + data[key].ToString() + "'";

	int ret = sqlite3_exec(mDB,sql.toStdString().c_str(),0,0,NULL);
	if(ret != SQLITE_OK)
		mErrorStr = sqlite3_errmsg(mDB);
    return (ret == SQLITE_OK);     
}

bool DBManager_sqlite::Select(QString table,QVector<DBRowData> &data_list,DBFilter filter)
{    
    return Select(table,data_list,QStringList(),filter);
}

bool DBManager_sqlite::Select(QString table,QVector<DBRowData> &varList,const QStringList &row_list,DBFilter filter)
{
	varList.clear();
        
	DBRowData var(table,row_list);    
    const QStringList &nameList = var.GetNameList();
    int count = nameList.size();

    QString sql = "SELECT "; 
    for(int i = 0; i < count;i++)
    {
        sql += nameList[i];
        if(i != count - 1)
            sql += ",";
    }
    sql += " FROM " + table;
	if(filter.IsVaild())
		sql += " WHERE " + filter.ToString();

	char zSQL[4096];
	const char *pzSql;
	strcpy(zSQL,sql.toStdString().c_str());
	 
	sqlite3_stmt *pVM;			
	int ret = sqlite3_prepare(mDB,zSQL,1024,&pVM,&pzSql);	
	while(ret == SQLITE_OK)
	{
		ret = sqlite3_step(pVM);
		if(ret == SQLITE_ROW)
		{
			int count = sqlite3_data_count(pVM);			

			QString str_var;
			for(int i = 0; i < count; i++)
			{
				const char *pdata = (const char *)sqlite3_column_text(pVM,i);
				var[i].FromString(pdata);
			}
			varList.push_back(var);

			ret = SQLITE_OK;
		}
		else
		{								
			sqlite3_finalize(pVM);
		}  
	}

	if(ret == SQLITE_DONE)			
		ret = SQLITE_OK;

	if(ret != SQLITE_OK)
		mErrorStr = sqlite3_errmsg(mDB);
	return (ret == SQLITE_OK);
}

bool DBManager_sqlite::SelectOne(DBRowData &data,DBFilter filter)
{
	QVector<DBRowData> varList;
	bool ret = Select(data.GetTableName(),varList,data.GetNameList(),filter);	
	if(ret)
	{
		if(varList.size() > 0)
		{
			data = varList[0];
		}
		else
		{
			mErrorStr ="not find data";
			ret = false;
		}
	}
	return ret;
}