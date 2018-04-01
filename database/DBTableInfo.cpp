#include "DBTableInfo.h"
#include "DBTableManager.h"
#include <QDataStream>

static const char *DB_TYPE_NAME[] ={
	"text",
	"int",
	"double",
	"date",
	"time",
	"datetime",
};

static const char *DB_INVAILD_COL_NAME[] ={
	"group",
};

static QString typeToStr(int type);
static int strToType(int);

//DBDataType
QString DBDataType::typeToStr(int type)
{
	Q_ASSERT(type >= 0 && type < TYPE_NUM);
	return DB_TYPE_NAME[type];
}
	 
int DBDataType::strToType(const QString &name)
{
	QString type_name = name.toLower();	
	for(int i = 0; i < sizeof(DB_TYPE_NAME)/sizeof(DB_TYPE_NAME[0]); i++)	
	{				
		if(type_name == DB_TYPE_NAME[i])
			return i;
	}

	Q_ASSERT(0);
	return -1;
}
//
DBDataLimit::DBDataLimit()
{
	type = LIMIT_NONE;
}

//DBDataInfo
DBDataInfo::DBDataInfo()
{
	dataType = TYPE_STRING;
	attr = NORMAL;
}

bool DBDataInfo::key() const
{
    return attr & KEY;
}

bool DBDataInfo::autoID() const
{
    return attr & AUTO_ID;
}

bool DBDataInfo::autoGen() const
{
    return attr & AUTO_GEN;
}

bool DBDataInfo::mastHave() const
{
    return attr & MAST_HAVE;
}

bool DBDataInfo::unique() const
{
    return attr & UNIQUE;
}

//DBData
DBData::DBData(int type)
{
	Q_ASSERT(type >= 0 && type < TYPE_NUM);
	mType = type;
	
	memset(&mData,0,sizeof(mData));
}

DBData::~DBData()
{

}

DBData &DBData::operator=(const DBData &other)
{
    Q_ASSERT((mType == TYPE_NULL && other.mType != TYPE_NULL) || (mType == other.mType));

    mType    = other.mType;
	mString  = other.mString;	
	mData    = other.mData;	

    return *this;
}

DBData &DBData::operator=(const QString &other)
{
    Q_ASSERT(IsStringType());
    mString = other;

    return *this;
}

DBData &DBData::operator=(int other)
{
    Q_ASSERT(mType == TYPE_INT);
    mData.mInt = other;

    return *this;
}

DBData &DBData::operator=(double other)
{
    Q_ASSERT(mType == TYPE_DOUBLE);
    mData.mDouble = other;

    return *this;
}

bool DBData::operator ==(const DBData &other) const
{
    if(this->IsStringType() && other.IsStringType())
        return mString == other.mString;
    else if(mType == TYPE_DOUBLE && other.mType == TYPE_DOUBLE)
        return mData.mDouble == other.mData.mDouble;
    else if(mType == TYPE_INT && other.mType == TYPE_INT)
        return mData.mInt == other.mData.mInt;
    else
    {
        Q_ASSERT(0);
        return false;
    }
}

DBData::operator QString() const
{
    Q_ASSERT(IsStringType());
    return mString;
}

DBData::operator int() const
{
    Q_ASSERT(mType == TYPE_INT);
    return mData.mInt;
}

DBData::operator double() const
{    
    Q_ASSERT(mType == TYPE_DOUBLE);
    return mData.mDouble;
}

void DBData::SetType(int type)
{
    Q_ASSERT(mType == TYPE_NULL && type >= 0 && type < TYPE_NUM);
    mType = type;
}

int DBData::GetType() const
{
    return mType;
}

bool DBData::IsStringType() const
{
    if(mType == TYPE_STRING
		|| mType == TYPE_DATE
		|| mType == TYPE_TIME	
		|| mType == TYPE_DATETIME)	
		return true;

    return false;
}

QString DBData::ToString() const
{
	char buffer[1024];
	QString str;
	if(IsStringType())	
		return mString;
	else if(mType == TYPE_INT)
	{
		sprintf_s(buffer,1024,"%d",mData.mInt);
		return QString(buffer);
	}
	else if(mType == TYPE_DOUBLE)
	{
		sprintf_s(buffer,1024,"%0.2f",mData.mDouble);
		return QString(buffer);
	}

	Q_ASSERT(0);
	return QString();
}

int DBData::ToInt() const
{
    Q_ASSERT(mType == TYPE_INT);
	return mData.mInt;
}

double DBData::ToDouble() const
{
    Q_ASSERT(mType == TYPE_DOUBLE);
	return mData.mDouble;
}

void DBData::FromString(QString data)
{	
	if(IsStringType())
		mString = data;
	else if(mType == TYPE_INT)
		mData.mInt = data.toInt();
	else if(mType == TYPE_DOUBLE)
		mData.mDouble = data.toDouble();	
}

void DBData::FromInt(int data)
{
	Q_ASSERT(mType == TYPE_INT);
	mData.mInt = data;	
}

void DBData::FromDouble(double data)
{
	Q_ASSERT(mType == TYPE_DOUBLE);
	mData.mDouble = data;
}

QDataStream& operator >>(QDataStream &stream,DBData &info)
{
    int type;
    QString str;

    stream >> type;
    stream >> str;

    info.SetType(type);
    info.FromString(str);
    
    return stream;
}

QDataStream& operator <<(QDataStream &stream,const DBData &info)
{
    stream << info.GetType();
    stream << info.ToString();
  
    return stream;
}

//DBTableInfo
DBTableInfo::DBTableInfo()
{		
}

int DBTableInfo::GetIndex(const QString &name) const
{
	for(int i = 0 ; i < (int)mNameList.size(); i++)
	{		
		if(mNameList[i] == name)
			return i;
	}
	return -1;
}

const QStringList &DBTableInfo::GetNameList() const
{
	return mNameList;
}

bool DBTableInfo::Contains(const QString &name) const
{
	return mNameList.contains(name);
}

int DBTableInfo::GetRowCount() const
{
    return mNameList.count();
}

QString DBTableInfo::GetKey() const
{
	return mKey;
}

void DBTableInfo::SetKey(const QString &key)
{
	Q_ASSERT(Contains(key));
	mKey = key;
}


void DBTableInfo::AddDataInfo(DBDataInfo info)
{
	QString table_name = info.name.toLower();
	for(int i = 0; i < sizeof(DB_INVAILD_COL_NAME)/sizeof(DB_INVAILD_COL_NAME[0]); i++)	
	{				
		Q_ASSERT(table_name != DB_INVAILD_COL_NAME[i]);
	}

	mNameList.push_back(info.name);
	mDataInfo.push_back(info);
    mDefaultData.push_back(DBData(info.dataType));
}

const DBDataInfo *DBTableInfo::GetDataInfo(const QString &name) const
{
	int idx = GetIndex(name);
	return &mDataInfo[idx];	
}

const DBDataInfo *DBTableInfo::GetDataInfo(int index) const
{
	return &mDataInfo[index];	
}

void DBTableInfo::SetDefaultData(const QString &name,DBData data)
{
    mDefaultData[GetIndex(name)] = data;
}

void DBTableInfo::SetDefaultData(int index,DBData data)
{
    mDefaultData[index] = data;
}
	
const DBData &DBTableInfo::GetDefaultData(const QString &name) const
{
    return mDefaultData[GetIndex(name)];
}

const DBData &DBTableInfo::GetDefaultData(int index) const
{
    return mDefaultData[index];
}

void DBTableInfo::SetDataLimit(const QString &name,DBDataLimit &limit)
{	
	Q_ASSERT(Contains(name));	
	mDataLimit[name] = limit;
}

const DBDataLimit *DBTableInfo::GetDataLimit(const QString &name) const
{
	QMap<QString,DBDataLimit>::const_iterator it = mDataLimit.find(name);
	if(it != mDataLimit.end())
		return &(it.value());

	return NULL;
}

const DBDataLimit *DBTableInfo::GetDataLimit(int index) const
{
	return GetDataLimit(mNameList[index]);
}

//DBRowData
DBRowData::DBRowData()
{

}

DBRowData::DBRowData(QString table_name,const QStringList &in_name_list)
{
	Init(table_name,in_name_list);	
}

DBRowData::~DBRowData()
{

}

bool DBRowData::IsVaild()
{
    return !mTableName.isEmpty();
}

void DBRowData::Init(QString tableName,const QStringList &in_name_list)
{
    Q_ASSERT(mTableName.isEmpty());
    
    mTableName = tableName;
	mNameList = in_name_list;

	const QStringList &name_list = GetNameList();
	for(int i = 0; i < (int)name_list.size(); i++)
	{
        const QString &name = name_list[i];
		const DBDataInfo *info = DBTableManager::Instance()->GetTableInfo(mTableName).GetDataInfo(name);
        const DBData &data = DBTableManager::Instance()->GetTableInfo(mTableName).GetDefaultData(name);
		mDataList.push_back(data);
	}
}

QString DBRowData::GetTableName() const
{
	return mTableName;
}

const DBTableInfo &DBRowData::GetTableInfo() const
{
	return DBTableManager::Instance()->GetTableInfo(mTableName);
}	

const QStringList &DBRowData::GetNameList() const
{
	if(mNameList.size() != 0)
		return mNameList;	
	else
		return DBTableManager::Instance()->GetTableInfo(mTableName).GetNameList();
}

const QStringList &DBRowData::GetInnerNameList() const
{
    return mNameList;
}

int DBRowData::GetCount() const
{
    return GetNameList().size();
}

QString DBRowData::Key() const
{
	return GetTableInfo().GetKey();
}

int DBRowData::GetIndex(const QString &name) const
{    
    const QStringList &nameList = GetNameList();
    return nameList.indexOf(name);
}

bool DBRowData::Contains(const QString &name) const
{
	return GetIndex(name) >= 0;
}

const DBDataInfo *DBRowData::GetDataInfo(int index) const
{
    const QStringList &name_list = GetNameList();
    return DBTableManager::Instance()->GetTableInfo(mTableName).GetDataInfo(name_list[index]);
}

const DBDataInfo *DBRowData::GetDataInfo(QString &name) const
{
    int index = GetIndex(name);
    return GetDataInfo(index);
}

const DBData &DBRowData::operator[](int col) const
{
    return mDataList[col];
}

DBData &DBRowData::operator[](int col)
{
    return mDataList[col];
}

const DBData &DBRowData::operator[](const QString &name) const
{
    int index = GetIndex(name);
    return mDataList[index];
}

DBData &DBRowData::operator[](const QString &name)
{
    int index = GetIndex(name);
    return mDataList[index];
}

QDataStream& operator >>(QDataStream &stream,DBRowData &info)
{
    QString table_name;
    QStringList name_list;
    QString data_str;

    stream >> table_name;
    if(!table_name.isEmpty())
    {
        stream >> name_list;
        
        info.Init(table_name,name_list);
        for(int i = 0; i < info.GetCount(); i++)
        {
            stream >> data_str;
            info[i].FromString(data_str);
        }
    }

    return stream;
}

QDataStream& operator <<(QDataStream &stream,const DBRowData &info)
{
    QString table_name = info.GetTableName();

    stream << table_name;
    if(!table_name.isEmpty())
    {
        stream << info.GetInnerNameList();        
        for(int i = 0; i < info.GetCount(); i++)    
            stream << info[i].ToString();    
    }
    return stream;
}