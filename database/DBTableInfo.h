#pragma once

#include "DBCommon.h"

enum{
    TYPE_NULL = -1,
	TYPE_STRING,		
	TYPE_INT,
	TYPE_DOUBLE,
	TYPE_DATE,
	TYPE_TIME,	
	TYPE_DATETIME,
	TYPE_NUM,
};	

class DBDataType
{
public:
	static QString typeToStr(int type);
	static int strToType(const QString &name);
};

class DBDataLimit
{
public:
	enum{
		LIMIT_NONE,
		LIMIT_LIST,
		LIMIT_BETWEEN,
	};

	DBDataLimit();	
	
	int type;
	QStringList dataList;	
};


class DBDataInfo
{
public:	
    enum{
        NORMAL    = 0,
        KEY       = 0x1,
        AUTO_ID   = 0x2,
        MAST_HAVE = 0x4,
        UNIQUE    = 0x8,        
        AUTO_GEN  = 0x10,
    };

	DBDataInfo();

    bool key() const;
    bool autoID() const;
    bool autoGen() const;
    bool mastHave() const;
    bool unique() const;    

    QString name;
	QString dispName;	
	QString defaultValue;	
	
	int dataType;	
    int attr;	
};


class DBData
{
public:    
	DBData(int type = TYPE_NULL);
	~DBData();
    
    DBData &operator=(const DBData &other);  
    DBData &operator=(const QString &other);  
    DBData &operator=(int other);  
    DBData &operator=(double other);  

    bool operator ==(const DBData &other) const;  

    operator QString() const;  
    operator int() const;  
    operator double() const;  

    void SetType(int type);
    int GetType() const;

	QString ToString() const;
	int ToInt() const;
	double ToDouble() const;	

	void FromString(QString data);
	void FromInt(int data);
	void FromDouble(double data);		

private:
    bool IsStringType() const;

    int mType;
	QString mString;
	union{
		int mInt;	
		double mDouble;
	}mData;	
};
QDataStream& operator >>(QDataStream &stream,DBData &info);
QDataStream& operator <<(QDataStream &stream,const DBData &info);

class DBTableInfo{

public:	
	DBTableInfo();
	
	QString TableName();
	const QStringList &GetNameList() const; 
	int GetIndex(const QString &name) const;
	bool Contains(const QString &name) const;
    int GetRowCount() const; 

	void SetKey(const QString &key);
	QString GetKey() const;

	void AddDataInfo(DBDataInfo info);
	const DBDataInfo *GetDataInfo(const QString &name) const;
	const DBDataInfo *GetDataInfo(int index) const;

    void SetDefaultData(const QString &name,DBData data);
    void SetDefaultData(int index,DBData data);
	const DBData &GetDefaultData(const QString &name) const;
	const DBData &GetDefaultData(int index) const;

	void SetDataLimit(const QString &name,DBDataLimit &limit);    
	const DBDataLimit *GetDataLimit(const QString &name) const;
	const DBDataLimit *GetDataLimit(int index) const;

private:	
	QStringList mNameList;
    QVector<DBData> mDefaultData;
	QVector<DBDataInfo> mDataInfo;    
	QMap<QString,DBDataLimit> mDataLimit;
	QString mKey;
};

class DBRowData
{		
public:	
	DBRowData();
    DBRowData(QString table_name,const QStringList &data_list = QStringList());
    virtual ~DBRowData();

	const DBTableInfo &GetTableInfo() const;

    bool IsVaild();
    void Init(QString tableName,const QStringList &data_list);
	QString GetTableName() const;		    

	const QStringList &GetNameList() const;
    const QStringList &GetInnerNameList() const;
	int GetCount() const;    
	int GetIndex(const QString &name) const;
	bool Contains(const QString &name) const;
    QString Key() const;

    const DBDataInfo *GetDataInfo(int index) const;
    const DBDataInfo *GetDataInfo(QString &name) const;	

    const DBData &operator[](int col) const;
    DBData &operator[](int col);

    const DBData &operator[](const QString &name) const;
    DBData &operator[](const QString &name);

protected:    	
	QString mTableName;
	QStringList mNameList;
    QVector<DBData> mDataList;    	
};
QDataStream& operator >>(QDataStream &stream,DBRowData &info);
QDataStream& operator <<(QDataStream &stream,const DBRowData &info);