#include "DBFilter.h"

DBFilter::DBFilter()
{
}

void DBFilter::Clear()
{
	mFilter.clear();
}

bool DBFilter::IsVaild()
{
	return !mFilter.isEmpty();
}

void DBFilter::FromString(QString &str)
{
	mFilter = str;
}

QString DBFilter::ToString()
{
	return mFilter;
}

void DBFilter::Last(QString table)
{
	QString sql = "ROWID = (SELECT max(ROWID) FROM " + table + ")";
	Add(sql);
}

void DBFilter::Add(QString name,QString var,QString var_op,int relate)
{
	QString sql = name + " " + var_op + " '" + var + "'";
	Add(sql,relate);
}

void DBFilter::Add(DBFilter filter,int relate)
{
    Add(filter.ToString(),relate);
}

void DBFilter::Add(QString sql,int relate)
{
	if(mFilter.isEmpty())
	{
		mFilter = "(" + sql + ")";
	}
	else
	{
		QString op;
		if(relate == 0)
			op = " AND ";
		else
			op = " OR ";

		mFilter = mFilter + op + "(" + sql + ")";
	}
}
