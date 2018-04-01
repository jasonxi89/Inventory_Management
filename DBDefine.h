#pragma once
#include "DBTableInfo.h"

namespace COMMON_TABLE
{
    static const char *ID = "ID";
    static const char *UPCCode = "UPCCode";
    static const char *Category = "Category";
    static const char *Name = "Name";
    static const char *Company = "Company";    
    static const char *Quantity = "Quantity";
}

namespace UPC_TABLE
{
    static const char *TABLE_NAME = "UPC_TABLE";
    
    static const char *UPCCode	= COMMON_TABLE::UPCCode;
    static const char *Category = COMMON_TABLE::Category;
    static const char *Name = COMMON_TABLE::Name;
    static const char *Company = COMMON_TABLE::Company;
    static const char *Memo = "Memo";
}


namespace PO_TABLE
{
    static const char *TABLE_NAME = "PO_TABLE";

    static const char *ID = COMMON_TABLE::ID;
    static const char *UPCCode	= COMMON_TABLE::UPCCode;
    static const char *Category = COMMON_TABLE::Category;
    static const char *Name = COMMON_TABLE::Name;
    static const char *Quantity = COMMON_TABLE::Quantity;
    static const char *Company = COMMON_TABLE::Company;
    static const char *WhereToPurchase = "WhereToPurchase";
    static const char *Ordertime = "Ordertime";
    static const char *TrackNumber = "TrackNumber";
    static const char *ProductCost = "ProductCost";
    static const char *ShipperCost = "ShipperCost";
    static const char *TotalCost = "TotalCost"; 
}

namespace INVENT_TABLE
{
	static const char *TABLE_NAME = "INVENT_TABLE";     

    static const char *ID = COMMON_TABLE::ID;
    static const char *UPCCode	= COMMON_TABLE::UPCCode;
    static const char *Category = COMMON_TABLE::Category;
    static const char *Name = COMMON_TABLE::Name;
    static const char *Company = COMMON_TABLE::Company;    
    static const char *Cost = "Cost";
    static const char *Invent = "Invent";
    static const char *IncomingInvent = "IncomingInvent";
    static const char *TotalInvent = "TotalIInvent";
}

namespace ORDERS_TABLE
{
    static const char *TABLE_NAME = "ORDERS_TABLE";  

    static const char *OrderNumber = COMMON_TABLE::ID;
    static const char *UPCCode	= COMMON_TABLE::UPCCode;
    static const char *Category = COMMON_TABLE::Category;
    static const char *Name = COMMON_TABLE::Name;
    static const char *Company = COMMON_TABLE::Company;
    static const char *Quantity = COMMON_TABLE::Quantity;

    static const char *Enough = "Enough";
    static const char *Shipped = "Shipped";
    static const char *ShippedTime = "ShippedTime";
    static const char *Price = "Price";

    static const char *Enough_YES = "足够";
    static const char *Enough_NO  = "不足";    

    static const char *Shipped_Send = "已发货";
    static const char *Shipped_None = "未发货";    
}

namespace SALES_TABLE
{
    static const char *TABLE_NAME = "SALES_TABLE";  

    static const char *ID = COMMON_TABLE::ID;
    static const char *UPCCode	= COMMON_TABLE::UPCCode;
    static const char *Category = COMMON_TABLE::Category;
    static const char *Name = COMMON_TABLE::Name;
    static const char *Company = COMMON_TABLE::Company;
    static const char *TotalSales = "TotalSales";
    static const char *Profit = "Profit";
}


bool DBInitTable();
