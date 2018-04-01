#include "DBDefine.h"
#include "DBTableInfo.h"
#include "DBTableManager.h"
#include "DBManager.h"
#include <QMessageBox>

struct stru_table_define
{
    QString name;
    const char *disp_name;
    QString type;
    QString attr;    
    QString limit;    
};

const stru_table_define upc_tableInfo[] = 
{    
    {UPC_TABLE::UPCCode,        "UPCCode",    "",        "key",          ""},
    {UPC_TABLE::Category,       "类别",       "",        "mastHave",     ""},
    {UPC_TABLE::Name,           "商品名",     "",        "mastHave",     ""},    
    {UPC_TABLE::Company,        "生产厂商",   "",        "mastHave",     ""},    
    {UPC_TABLE::Memo,           "备注",       "",        "",             ""}, 
    {"","","","",""},
};

const stru_table_define po_tableInfo[] = 
{
    {PO_TABLE::ID,             "ID",         "int",     "key autoID",   ""},    
    {PO_TABLE::UPCCode,        "UPCCode",    "",        "",         "limit"},
    {PO_TABLE::Category,       "类别",       "",        "mastHave",     ""},
    {PO_TABLE::Name,           "商品名",     "",        "mastHave",     ""},
    {PO_TABLE::Quantity,       "数量",       "int",     "",     ""},
    {PO_TABLE::Company,        "生产厂商",   "",        "mastHave",     ""},
    {PO_TABLE::WhereToPurchase,"购买途径",   "",        "",     ""},
    {PO_TABLE::Ordertime,  "购买时间",       "date",    "",     ""},
    {PO_TABLE::TrackNumber,"追踪号",         "",        "",     ""},
    {PO_TABLE::ProductCost,"商品成本",       "double",        "",     ""},
    {PO_TABLE::ShipperCost,"运输成本",       "double",        "",     ""},
    {PO_TABLE::TotalCost,  "总成本",         "double",        "",     ""},
    {"","","","",""},
};

const stru_table_define invertory_tableInfo[] = 
{
    {INVENT_TABLE::ID,             "ID",         "int",     "key autoID",   ""},    
    {INVENT_TABLE::UPCCode,        "UPCCode",    "",        "unique",        "limit"},
    {INVENT_TABLE::Category,       "类别",       "",        "mastHave",     ""},
    {INVENT_TABLE::Name,           "商品名",     "",        "mastHave",     ""},
    {INVENT_TABLE::Company,        "生产厂商",   "",        "mastHave",     ""},    
    {INVENT_TABLE::Cost,            "成本",      "double",      "",     ""},
    {INVENT_TABLE::Invent,          "入库货存",   "int",        "",     ""},
    {INVENT_TABLE::IncomingInvent,  "即将入库",   "int",        "",     ""},
    {INVENT_TABLE::TotalInvent,     "总货存",     "int",        "",     ""},
    
    {"","","","",""},
};

const stru_table_define orders_tableInfo[] = 
{
    {ORDERS_TABLE::OrderNumber,    "ID",         "int",     "key autoID",   ""},    
    {ORDERS_TABLE::UPCCode,        "UPCCode",    "",        "mastHave",     "limit"},
    {ORDERS_TABLE::Category,       "类别",       "",        "mastHave",     ""},
    {ORDERS_TABLE::Name,           "商品名",     "",        "mastHave",     ""},
    {ORDERS_TABLE::Company,        "生产厂商",   "",        "mastHave",     ""},
    {ORDERS_TABLE::Quantity,        "数量",       "int",     "",     ""},

    {ORDERS_TABLE::Enough,         "现货是否充足",   "",        "mastHave",     ""},
    {ORDERS_TABLE::Shipped,        "是否已经交货",   "",        "mastHave",     ""},
    {ORDERS_TABLE::ShippedTime,    "交货时间",   "date",        "",             ""},
    {ORDERS_TABLE::Price,          "售价",       "double",      "mastHave",     ""},
    {"","","","",""},
};

const stru_table_define salse_tableInfo[] = 
{
    {SALES_TABLE::ID,             "ID",         "int",     "key autoID",   ""},    
    {SALES_TABLE::UPCCode,        "UPCCode",    "",        "unique",        "limit"},
    {SALES_TABLE::Category,       "类别",       "",        "mastHave",     ""},
    {SALES_TABLE::Name,           "商品名",     "",        "mastHave",     ""},
    {SALES_TABLE::Company,        "生产厂商",   "",        "mastHave",     ""},

    {SALES_TABLE::TotalSales,     "总销量",      "double",        "mastHave",    ""},
    {SALES_TABLE::Profit,         "利润",        "double",        "mastHave",    ""},
    {"","","","",""},

};

void RegistTable(QString table_name,const stru_table_define table_define[])
{	
	DBTableInfo tableinfo;
	QStringList spec_str;	
    QString table_key;
	
    //table
	int i = 0;
	while(true)
	{
        stru_table_define define = table_define[i];
        if(define.name.isEmpty())
            break;			

        //info                
        DBDataInfo data_info;
        QString type = define.type.isEmpty()? "text":define.type;

        data_info.name = define.name;
		data_info.dispName  = define.disp_name;	
        data_info.dataType = DBDataType::strToType(type);

        QString attr = define.attr;
        if(!attr.isEmpty())
		{   
            if(attr.indexOf("autoID") >= 0)            
                data_info.attr |= DBDataInfo::AUTO_ID;
            if(attr.indexOf("autoGen") >= 0)            
                data_info.attr |= DBDataInfo::AUTO_GEN;
			if(attr.indexOf("mastHave") >= 0)
                data_info.attr |= DBDataInfo::MAST_HAVE;
			if(attr.indexOf("unique") >= 0)
            {
                data_info.attr |= DBDataInfo::UNIQUE;            
                data_info.attr |= DBDataInfo::MAST_HAVE;
            }
            if(attr.indexOf("key") >= 0)
            {
                data_info.attr |= DBDataInfo::MAST_HAVE;
                data_info.attr |= DBDataInfo::KEY;
                table_key = define.name;
            }
		}
		tableinfo.AddDataInfo(data_info);		                

        //limit
        if(!define.limit.isEmpty())
        {
            spec_str = define.limit.split("|");
		    for(int j = 0; j < (int)spec_str.size(); j++)		
			    spec_str[j] = spec_str[j].trimmed();

		    DBDataLimit limit_info;
		    if(spec_str[0] == "limit")
		    {
			    limit_info.type = DBDataLimit::LIMIT_LIST;
                for(int i = 2; i < spec_str.size(); i++)
			        limit_info.dataList.push_back(spec_str[i]);
		    }
		    else
		    {
			    Q_ASSERT(0);
		    }
		    tableinfo.SetDataLimit(define.name,limit_info);
        }
        i++;
	}	    		 
    if(!table_key.isEmpty())
        tableinfo.SetKey(table_key);
		
	DBTableManager::Instance()->RegistTable(table_name,tableinfo);		
}

bool DBCreateTable()
{
	if (!DBManager::Instance()->Init())
    {
        QMessageBox::information(NULL, QString(), DBManager::Instance()->GetErrorStr());
		return false;
    }

    //初始化表
    bool need_init_data = false;
    bool ret = true;
	const QMap<QString,DBTableInfo> &table_info_map = DBTableManager::Instance()->GetTableMap();	
	QMap<QString,DBTableInfo>::const_iterator it = table_info_map.begin();
	if(!DBManager::Instance()->IsExist(it.key()))
	{
        need_init_data = true;
		while(it != table_info_map.end())
		{
			ret = DBManager::Instance()->Create(it.key());
            Q_ASSERT(ret);

			it++;
		}
    }
   	
    //加载数据生成器    
    DBTableManager::Instance()->InitGen();
    
    //创建默认数据
    if(need_init_data)
    {

    }

	return true;
}

bool DBInitTable()
{
    //注册表
    RegistTable(UPC_TABLE::TABLE_NAME        ,upc_tableInfo);
	RegistTable(PO_TABLE::TABLE_NAME        ,po_tableInfo);
    RegistTable(INVENT_TABLE::TABLE_NAME ,invertory_tableInfo);
    RegistTable(ORDERS_TABLE::TABLE_NAME    ,orders_tableInfo);
    RegistTable(SALES_TABLE::TABLE_NAME     ,salse_tableInfo);
	return DBCreateTable();	
}
