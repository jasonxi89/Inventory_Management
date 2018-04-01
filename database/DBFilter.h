#pragma once
#include "DBCommon.h"

class DBFilter
{
public:
	enum{
		AND,
		OR,
	};

	DBFilter();

	bool IsVaild();

	void Clear();
	void Add(QString name,QString var,QString var_op = "=",int relate = AND);
	void Add(DBFilter filter,int relate = AND);
    void Add(QString str,int relate = AND);

	void FromString(QString &str);
	QString ToString();	

	void Last(QString table);

private:
	QString mFilter;
};

