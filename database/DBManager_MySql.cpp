#include "DBManager_MySql.h"
#include "DBTableManager.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

DBManager_MySql::DBManager_MySql(void)
{
}


DBManager_MySql::~DBManager_MySql(void)
{
	if(mDB.isOpen())
		mDB.close();
}

bool DBManager_MySql::Init()
{    
    mDB = QSqlDatabase::addDatabase("QMYSQL");

    mDB.setHostName(mHost);
    mDB.setDatabaseName(mDataBase);
    mDB.setUserName(mName);
    mDB.setPassword(mPass);
    mDB.setPort(3306);
    if(mDB.open())	
	    return true;
    else
    {        
		mErrorStr = mDB.lastError().text();
        qDebug() << mErrorStr;
        return false;
    }
}

void DBManager_MySql::ClearDatabase()
{
	QStringList table_list = mDB.tables(QSql::Views);

	QSqlQuery sql_cmd(mDB);
	for (int i = 0; i < table_list.size(); i++)
	{		
		QString sql_del = "DROP TABLE IF EXISTS " + table_list[i];
		sql_cmd.exec(sql_del);
	}
}

void DBManager_MySql::SetHost(QString host,QString database,QString name,QString pass)
{
    mHost = host;
    mDataBase = database;
    mName = name;
    mPass = pass;
}

QString DBManager_MySql::GetErrorStr()
{	
	return mDB.lastError().text();
}

bool DBManager_MySql::IsExist(QString table)
{	
	QStringList table_list = mDB.tables(QSql::Tables);
    return table_list.contains(table, Qt::CaseInsensitive);
}

bool DBManager_MySql::Create(QString table)
{
	const DBTableInfo &table_info = DBTableManager::Instance()->GetTableInfo(table);
	const QStringList &nameList = table_info.GetNameList();
	QString primary_key;
	QStringList uniqueList;

	QString sql = "create table if not exists " + table + "(";	
	int count = nameList.size();
	for (int i = 0; i < count; i++)
	{
		QString row_name = nameList[i];
		const DBDataInfo *data_info = table_info.GetDataInfo(row_name);

		QString row_type = DBDataType::typeToStr(data_info->dataType);
		if (row_type == "int")
			row_type = "INTEGER";

		sql += row_name + " ";		
		if (nameList[i] == table_info.GetKey())
		{	
			if (row_type == "text")
				row_type = "varchar(128)";

			sql += row_type;
			if (data_info->autoID())
				sql += " PRIMARY KEY AUTO_INCREMENT";
			else
			{ 
				sql += "";
				primary_key = "primary key(" + row_name + ")";
			}
		}
		else
		{			
			if (data_info->unique() && row_type == "text")
				row_type = "varchar(128)";

			sql += row_type;
			if (data_info->autoID())
			{
				Q_ASSERT(0);
			}
			if (data_info->unique())
			{			
				QString unique_key = "unique key(" + row_name + ")";
				uniqueList.push_back(unique_key);
			}
			if (data_info->mastHave())
				sql += " NOT NULL";
		}
		if (i != count - 1)
			sql += ",";
	}
	if(!primary_key.isEmpty())
	{
		sql += ",";
		sql += primary_key;
	}
	for (int i = 0; i < uniqueList.size(); i++)
	{
		sql += ",";
		sql += uniqueList[i];
	}
	sql += ");";

	QSqlQuery sql_cmd(mDB);
	QString sql_del = "DROP TABLE IF EXISTS " + table;		
	sql_cmd.exec(sql_del);

	if (sql_cmd.exec(sql))
		return true;
	else
	{
		mErrorStr = sql_cmd.lastError().text();
		return false;
	}
}

bool DBManager_MySql::Insert(DBRowData &data)
{
	QString table = data.GetTableName();
	QString sql = "";
	QString str_data;
	QString row_name;
	QString row_value;
	QString auto_id;	

	DBRowData bak_data = data;
	const QStringList &nameList = data.GetNameList();

	int count = data.GetCount();
	for (int i = 0; i < count; i++)
	{
		const DBDataInfo *data_info = data.GetDataInfo(i);
		if (data_info->autoID())
		{
			auto_id = data.Key();
			str_data.clear();
			continue;
		}
		else
		{
			str_data = data[i].ToString();
			if (str_data.isEmpty() && data_info->autoGen())
			{
				DBDataGen *gen = DBTableManager::Instance()->GetDataGen(table, nameList[i]);
				Q_ASSERT(gen);

				DBData tmp = gen->Generate(true);
				str_data = tmp.ToString();
			}
		}

		row_name += nameList[i];
		if (str_data.isEmpty())
		{
			row_value += "NULL";
		}
		else
		{
			row_value += "'" + str_data + "'";
		}

		if (i != count - 1)
		{
			row_name += ",";
			row_value += ",";
		}
	}
	sql += "insert into " + table + " (" + row_name + ")";
	sql += "values(" + row_value + ");";

	QSqlQuery sql_cmd(mDB);	
	if (!sql_cmd.exec(sql))
	{
		mErrorStr = sql_cmd.lastError().text();
		data = bak_data;
		return false;
	}
	else
	{
		if (!auto_id.isEmpty()) //²éÑ¯×ÔÔöid
		{
			int rowid = sql_cmd.lastInsertId().toInt();	
			data[auto_id] = rowid;
		}
		return true;
	}
}

bool DBManager_MySql::Delete(const DBRowData &data)
{
	QString table = data.GetTableName();
	QString sql = "DELETE FROM " + table;

	sql += " WHERE ";
	QString key = data.Key();
	if (!key.isEmpty())
		sql += key + "='" + data[key].ToString() + "'";
	else
	{
		const QStringList &nameList = data.GetNameList();
		int count = nameList.size();
		for (int i = 0; i < count; i++)
		{
			sql += nameList[i] + "='" + data[i].ToString() + "'";
			if (i != count - 1)
				sql += " AND ";
		}
	}

	QSqlQuery sql_cmd(mDB);
	if (sql_cmd.exec(sql))
		return true;
	else
	{
		mErrorStr = sql_cmd.lastError().text();
		return false;
	}	
}

		
bool DBManager_MySql::Update(const DBRowData &data,DBFilter filter)
{
	QString table = data.GetTableName();
	QString sql = "UPDATE " + table + " SET ";

	const QStringList &nameList = data.GetNameList();
	QString key = data.Key();
	int count = nameList.size();
	bool hasKey = false;
	for (int i = 0; i < count; i++)
	{
		if (nameList[i] == key)
		{
			hasKey = true;
			continue;
		}

		sql += nameList[i] + "=";
		sql += "'";
		sql += data[i].ToString();
		sql += "'";
		if (i != count - 1)
			sql += ",";
	}
	sql += " WHERE ";
	if (filter.IsVaild())
		sql += filter.ToString();
	else if (hasKey && !key.isEmpty())
		sql += key + "='" + data[key].ToString() + "'";

	QSqlQuery sql_cmd(mDB);
	if (sql_cmd.exec(sql))
		return true;
	else
	{
		mErrorStr = sql_cmd.lastError().text();
		return false;
	}
}

bool DBManager_MySql::Select(QString table,QVector<DBRowData> &data_list,DBFilter filter)
{    
    return Select(table,data_list,QStringList(),filter);
}

bool DBManager_MySql::Select(QString table,QVector<DBRowData> &varList,const QStringList &row_list,DBFilter filter)
{
	varList.clear();
    
	DBRowData var(table, row_list);
	const QStringList &nameList = var.GetNameList();
	int count = nameList.size();
	bool ret;

	QString sql = "SELECT ";
	for (int i = 0; i < count; i++)
	{
		sql += nameList[i];
		if (i != count - 1)
			sql += ",";
	}
	sql += " FROM " + table;
	if (filter.IsVaild())
		sql += " WHERE " + filter.ToString();

	char zSQL[4096];
	const char *pzSql;
	strcpy(zSQL, sql.toStdString().c_str());

	QSqlQuery sql_cmd(mDB);
	ret = sql_cmd.exec(sql);

	while(ret && sql_cmd.next())
	{		
		QString str_var;
		for (int i = 0; i < count; i++)
		{			
            var[i].FromString(sql_cmd.value(i).toString());
		}
		varList.push_back(var);
	}

	if (ret)
		return true;
	else
	{
		mErrorStr = sql_cmd.lastError().text();
		return false;
	}	
}

bool DBManager_MySql::SelectOne(DBRowData &data,DBFilter filter)
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
