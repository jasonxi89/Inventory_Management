#include "upc.h"
#include "DBDefine.h"
#include "DBManager.h"
#include <QMessageBox>
#include <QMenu>
#include <QInputDialog>
#include "DBTableManager.h"
#include "UPCDlg.h"

enum {
    UPC_PAGE,
    PO_PAGE,
    INVENT_PAGE,
    ORDER_PAGE,
    SALES_PAGE,
    PAGE_NUM
};

//
QString currentDate()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd");
}

//
UPC::UPC(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    ui.setupUi(this);

    connect(ui.btnUPC,SIGNAL(clicked()),this, SLOT(changePage()));
    connect(ui.btnPO,SIGNAL(clicked()),this, SLOT(changePage()));
    connect(ui.btnInventory,SIGNAL(clicked()),this, SLOT(changePage()));
    connect(ui.btnOrders,SIGNAL(clicked()),this, SLOT(changePage()));
    connect(ui.btnSales,SIGNAL(clicked()),this, SLOT(changePage()));

    connect(ui.btnBack0,SIGNAL(clicked()),this, SLOT(backToMain()));
    connect(ui.btnBack1,SIGNAL(clicked()),this, SLOT(backToMain()));
    connect(ui.btnBack2,SIGNAL(clicked()),this, SLOT(backToMain()));
    connect(ui.btnBack3,SIGNAL(clicked()),this, SLOT(backToMain()));
    connect(ui.btnBack4,SIGNAL(clicked()),this, SLOT(backToMain()));    

    connect(ui.btnFindInventLess,SIGNAL(clicked()),this, SLOT(onBtnFindInventLess()));    

    mComboBoxList.resize(PAGE_NUM);
    mFilterList.resize(PAGE_NUM);
    mDataBase.resize(PAGE_NUM);
    mDispIndex.resize(PAGE_NUM);

    //table dataTrans
    tableNameList.push_back(UPC_TABLE::TABLE_NAME);
    tableNameList.push_back(PO_TABLE::TABLE_NAME);
    tableNameList.push_back(INVENT_TABLE::TABLE_NAME);
    tableNameList.push_back(ORDERS_TABLE::TABLE_NAME);
    tableNameList.push_back(SALES_TABLE::TABLE_NAME);

    upcDataTrans.setWidget(ui.tableWidgetUPC);
    poDataTrans.setWidget(ui.tableWidgetPo);
    inventDataTrans.setWidget(ui.tableWidgetInvent);
    orderDataTrans.setWidget(ui.tableWidgetOrder);
    salesDataTrans.setWidget(ui.tableWidgetSales);    

    dataTransList.push_back(&upcDataTrans);
    dataTransList.push_back(&poDataTrans);
    dataTransList.push_back(&inventDataTrans);
    dataTransList.push_back(&orderDataTrans);    
    dataTransList.push_back(&salesDataTrans);

    //combo box
    mComboBoxList[UPC_PAGE].push_back(ui.comboBoxUPC1);
    mComboBoxList[UPC_PAGE].push_back(ui.comboBoxUPC2);    

    mComboBoxList[PO_PAGE].push_back(ui.comboBoxPo1);
    mComboBoxList[PO_PAGE].push_back(ui.comboBoxPo2);
    mComboBoxList[PO_PAGE].push_back(ui.comboBoxPo3);
    mComboBoxList[PO_PAGE].push_back(ui.comboBoxPo4);
    mComboBoxList[PO_PAGE].push_back(ui.comboBoxPo5);

    mComboBoxList[INVENT_PAGE].push_back(ui.comboBoxInvent1);
    mComboBoxList[INVENT_PAGE].push_back(ui.comboBoxInvent2);

    mComboBoxList[ORDER_PAGE].push_back(ui.comboBoxOrders1);
    mComboBoxList[ORDER_PAGE].push_back(ui.comboBoxOrders2);
    mComboBoxList[ORDER_PAGE].push_back(ui.comboBoxOrders3);

    mComboBoxList[SALES_PAGE].push_back(ui.comboBoxSales1);
    mComboBoxList[SALES_PAGE].push_back(ui.comboBoxSales2);    
    
    //filter
    mFilterList[UPC_PAGE].push_back(UPC_TABLE::Category);
    mFilterList[UPC_PAGE].push_back(UPC_TABLE::Company);

    mFilterList[PO_PAGE].push_back(PO_TABLE::Category);
    mFilterList[PO_PAGE].push_back(PO_TABLE::Company);
    mFilterList[PO_PAGE].push_back(PO_TABLE::WhereToPurchase);
    mFilterList[PO_PAGE].push_back(PO_TABLE::Ordertime);
    mFilterList[PO_PAGE].push_back(PO_TABLE::ShipperCost);

    mFilterList[INVENT_PAGE].push_back(INVENT_TABLE::Category);
    mFilterList[INVENT_PAGE].push_back(INVENT_TABLE::Company);

    mFilterList[ORDER_PAGE].push_back(ORDERS_TABLE::Category);
    mFilterList[ORDER_PAGE].push_back(ORDERS_TABLE::Company);
    mFilterList[ORDER_PAGE].push_back(ORDERS_TABLE::Shipped);

    mFilterList[SALES_PAGE].push_back(SALES_TABLE::Category);
    mFilterList[SALES_PAGE].push_back(SALES_TABLE::Company); 

    //btn
    btnNewList.push_back(ui.btnNewUPC);
    btnNewList.push_back(ui.btnNewPo);
    btnNewList.push_back(ui.btnNewInvent);
    btnNewList.push_back(ui.btnNewOrder);
    btnNewList.push_back(ui.btnNewSales);

    btnDeleteList.push_back(ui.btnDeleteUPC);
    btnDeleteList.push_back(ui.btnDeletePo);
    btnDeleteList.push_back(ui.btnDeleteInvent);
    btnDeleteList.push_back(ui.btnDelOrder);
    btnDeleteList.push_back(ui.btnDelSales);       

    lineFindList.push_back(ui.lineFindUPC); 
    lineFindList.push_back(ui.lineFindPo);       
    lineFindList.push_back(ui.lineFindInvent);       
    lineFindList.push_back(ui.lineFindOrder);       
    lineFindList.push_back(ui.lineFindSales);       

    btnFindList.push_back(ui.btnFindUPC);     
    btnFindList.push_back(ui.btnFindPo);       
    btnFindList.push_back(ui.btnFindInvent);       
    btnFindList.push_back(ui.btnFindOrder);       
    btnFindList.push_back(ui.btnFindSales);       

    ui.dateEditOrderFrom->setDate(QDate(2001,1,1));
    ui.dateEditOrderTo->setDate(QDate(2020,1,1));

    for(int i = 0; i < PAGE_NUM; i++)
    {
        connect(btnNewList[i],SIGNAL(clicked()),this, SLOT(onBtnNew()));    
        connect(btnDeleteList[i],SIGNAL(clicked()),this, SLOT(onBtnDelete()));

        connect(lineFindList[i],SIGNAL(returnPressed()),btnFindList[i], SIGNAL(clicked()));
        connect(btnFindList[i],SIGNAL(clicked()),this, SLOT(onBtnFind()));

        for(int j = 0; j < mComboBoxList[i].size(); j++)
            connect(mComboBoxList[i][j],SIGNAL(currentIndexChanged(int)),this, SLOT(onComboBoxChanged()));

        dataTransList[i]->initTable(tableNameList[i]);
        updateDataBase(i);
    }        

    btnNewList[INVENT_PAGE]->setVisible(false);
    btnDeleteList[INVENT_PAGE]->setVisible(false);

    btnNewList[SALES_PAGE]->setVisible(false);
    btnDeleteList[SALES_PAGE]->setVisible(false);    

    connect(ui.dateEditOrderFrom,SIGNAL(dateChanged(const QDate &)),this, SLOT(onDateRefreshList()));	
    connect(ui.dateEditOrderTo,SIGNAL(dateChanged(const QDate &)),this, SLOT(onDateRefreshList()));	

    connect(ui.tableWidgetInvent,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(inventContextMenu(const QPoint&)));		
    ui.tableWidgetInvent->setContextMenuPolicy(Qt::CustomContextMenu);    

    connect(ui.tableWidgetPo,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(editContextMenu(const QPoint&)));		
    ui.tableWidgetPo->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui.tableWidgetOrder,SIGNAL(customContextMenuRequested(const QPoint&)),this,SLOT(editContextMenu(const QPoint&)));		
    ui.tableWidgetOrder->setContextMenuPolicy(Qt::CustomContextMenu);

    backToMain();
}

UPC::~UPC()
{

}

void UPC::changePage()
{
    const char *window_name[] = {"protect","po","inventory","orders","sales"};
    int cur_page = 0;
    if(sender() == ui.btnUPC)
        cur_page = UPC_PAGE;
    else if(sender() == ui.btnPO)
        cur_page = PO_PAGE;
    else if(sender() == ui.btnInventory)
        cur_page = INVENT_PAGE;
    else if(sender() == ui.btnOrders)
        cur_page = ORDER_PAGE;
    else if(sender() == ui.btnSales)
        cur_page = SALES_PAGE;

    setWindowTitle(window_name[cur_page]);
    ui.stackedWidget->setCurrentIndex(cur_page + 1);
}

void UPC::backToMain()
{
    setWindowTitle("GUI PROJECT");
    ui.stackedWidget->setCurrentIndex(0);
}

void UPC::reportError()
{
    if(mErrorStr.isEmpty())
    {
        QString log = tr("数据库错误:");
        log += DBManager::Instance()->GetErrorStr();
        QMessageBox::information(this,QString(),log);
    }
    else
    {
        QMessageBox::information(this,QString(),mErrorStr);
        mErrorStr.clear();
    }
}

void UPC::updateDataBase(int idx)
{
    QString table_name = tableNameList[idx];    
    QVector<DBRowData> &row_list = mDataBase[idx];        
    row_list.clear();        
    
    if(DBManager::Instance()->Select(table_name,row_list))
    {        
        refreshLimit(idx);
        refreshList(idx);        
    }
    else
        reportError();
}

void UPC::refreshLimit(int idx)
{    
    QVector<QComboBox*> &box_list = mComboBoxList[idx];  
    QVector<QString> &filter_list = mFilterList[idx];  
    QVector<DBRowData> &row_list = mDataBase[idx];  
    
    for(int i = 0; i < box_list.size(); i++)
    {
        QStringList item_list;
        QString table_name = filter_list[i];

        item_list.push_back("ALL");      
        box_list[i]->clear();
        for(int row = 0; row < row_list.size(); row++)
        {
            QString var = row_list[row][table_name].ToString();
            if(var.isEmpty())
                var = "NULL";
            if(item_list.indexOf(var) < 0)
                item_list.push_back(var);    
        }
        box_list[i]->addItems(item_list);
    }    

    if(idx == UPC_PAGE)
    {
        QStringList upc_list;

        //刷新upd 列表
        mUpcMap.clear();        
        for(int i = 0; i < row_list.size(); i++)
        {                        
            DBRowData &data = row_list[i];
            QString upc = data[UPC_TABLE::UPCCode];            

            UPC_INFO info;
            info.category = data[UPC_TABLE::Category].ToString();
            info.name     = data[UPC_TABLE::Name].ToString();
            info.company  = data[UPC_TABLE::Company].ToString();
            info.memo = data[UPC_TABLE::Memo].ToString();

            mUpcMap[upc] = info;
            upc_list.push_back(upc);            
        }

        DBDataLimit limit;
        limit.type = DBDataLimit::LIMIT_LIST;
        limit.dataList = upc_list;
        DBTableManager::Instance()->SetDataLimit(PO_TABLE::TABLE_NAME,PO_TABLE::UPCCode,limit);
        DBTableManager::Instance()->SetDataLimit(ORDERS_TABLE::TABLE_NAME,ORDERS_TABLE::UPCCode,limit);        
    }   
}

void UPC::onComboBoxChanged()
{
    int idx = -1;
    for(int i = 0; i < PAGE_NUM; i++)
    {        
        if(mComboBoxList[i].indexOf((QComboBox*)sender()) >= 0)
        {
            idx = i;
            break;
        }
    } 
    refreshList(idx);
}

bool UPC::dataFilter(int idx,DBRowData &data)
{        
    QVector<QComboBox*> &box_list = mComboBoxList[idx];
    QVector<QString> &filter_list = mFilterList[idx]; 
    for(int i = 0; i < box_list.size(); i++)
    {
        QString table_name = filter_list[i];
        QString var = box_list[i]->currentText();
        if(var == "ALL")
            continue;
        if(var == "NULL")
            var.clear();
        if(data[table_name].ToString() != var)
            return false;
    }

    if(idx == ORDER_PAGE)
    {
        QString from = ui.dateEditOrderFrom->date().toString("yyyy-MM-dd");
        QString to = ui.dateEditOrderTo->date().toString("yyyy-MM-dd");

        if((data[ORDERS_TABLE::Shipped].ToString() == ORDERS_TABLE::Shipped_Send) &&
          (data[ORDERS_TABLE::ShippedTime].ToString() < from || data[ORDERS_TABLE::ShippedTime].ToString() > to))
        {
            return false;
        }
    }
    
    return true;
}

void UPC::refreshList(int idx)
{
    QString table_name = tableNameList[idx];
    DBUiTableWidget *pDataTrans = dataTransList[idx];    
    QVector<DBRowData> &row_list = mDataBase[idx];
    QVector<int> &disp_list = mDispIndex[idx];

    pDataTrans->widget()->setRowCount(0);
    disp_list.clear();   

    double profit = 0;
    for(int i = 0; i < row_list.size(); i++)
    {
        if(dataFilter(idx,row_list[i]))
        {
            pDataTrans->addRow(row_list[i]);
            disp_list.push_back(i);

            if(idx == SALES_PAGE)
            {
                profit += row_list[i][SALES_TABLE::Profit].ToDouble();
            }
        }
    }    
    ui.lineTotalProfit->setText(QString::number(profit,'f',2));
}

void UPC::showList(int idx,QString upccode)
{
    ui.stackedWidget->setCurrentIndex(idx + 1);

    QString table_name = tableNameList[idx];
    DBUiTableWidget *pDataTrans = dataTransList[idx];    
    QVector<DBRowData> &row_list = mDataBase[idx];
    QVector<int> &disp_list = mDispIndex[idx];

    pDataTrans->widget()->setRowCount(0);
    disp_list.clear();   
    for(int i = 0; i < row_list.size(); i++)
    {
        if(row_list[i][COMMON_TABLE::UPCCode].ToString() == upccode)
        {
            pDataTrans->addRow(row_list[i]);
            disp_list.push_back(i);
        }
    }
}

int UPC::FindDBRow(QVector<DBRowData> &row_list,QString name,DBData data)
{    
    for(int i = 0; i < row_list.size(); i++)
    {
        if(row_list[i][name] == data)
            return i;       
    }
    return -1;
}

QVector<int> UPC::FindDBRows(QVector<DBRowData> &row_list,QString name,DBData data)
{
    QVector<int> list;
    for(int i = 0; i < row_list.size(); i++)
    {
        if(row_list[i][name] == data)
            list.push_back(i);
    }
    return list;
}

void UPC::CopyUPCTo(const DBRowData &src,DBRowData &des)
{
    des[COMMON_TABLE::UPCCode] = src[COMMON_TABLE::UPCCode];
    des[COMMON_TABLE::Category] = src[COMMON_TABLE::Category];
    des[COMMON_TABLE::Name] = src[COMMON_TABLE::Name];
    des[COMMON_TABLE::Company] = src[COMMON_TABLE::Company];  
}

void UPC::InsertData(int idx,DBRowData &data)
{       
    if(idx == ORDER_PAGE)
    {
        int need = data[ORDERS_TABLE::Quantity];
        int book = 0;
        
        //计算总量
        QVector<int> idx_list = FindDBRows(mDataBase[ORDER_PAGE],COMMON_TABLE::UPCCode,data[ORDERS_TABLE::UPCCode]);
        for(int i = 0; i < idx_list.size(); i++)
        {
            DBRowData &order = mDataBase[ORDER_PAGE][idx_list[i]];
            if(order[ORDERS_TABLE::Shipped].ToString() == ORDERS_TABLE::Shipped_None)
                need += order[ORDERS_TABLE::Quantity].ToInt();
        }

        //计算库存
        QVector<DBRowData> &invent_list = mDataBase[INVENT_PAGE];
        int invent_idx = FindDBRow(mDataBase[INVENT_PAGE],COMMON_TABLE::UPCCode,data[COMMON_TABLE::UPCCode]);  
        if(invent_idx == -1)
            book = need;
        else 
        {
            DBRowData &invent = mDataBase[INVENT_PAGE][invent_idx];
            int total = invent[INVENT_TABLE::TotalInvent].ToInt();
            if(total < need)
                book = need - total;
        }

        if(book && QMessageBox::Ok == QMessageBox::question(this,"","库存不足,是否自动订货",QMessageBox::Ok | QMessageBox::Cancel))
        {
            DBRowData po(PO_TABLE::TABLE_NAME);
            CopyUPCTo(data,po);
            po[PO_TABLE::Quantity] = book;

            InsertData(PO_PAGE,po);
        }
    }

    //add
    bool ret = DBManager::Instance()->Insert(data);
    if(ret)
    {
        if(idx == PO_PAGE)
        {
            ret = AddPo(data);            
        }
        if(!ret) //新增失败，删除
            DBManager::Instance()->Delete(data);        
    }

    if(ret)
    {
        AddRow(idx,data);        
        refreshLimit(idx);

        if(idx == INVENT_PAGE || idx == ORDER_PAGE)
            UpdateOrder(data[COMMON_TABLE::UPCCode].ToString());
    }
    else
        reportError();
}

void UPC::UpdateData(int idx,DBRowData &data)
{
    bool ret = DBManager::Instance()->Update(data);
    if(ret)
    {
        UpdateRow(idx,data); 
        refreshLimit(idx);

        if(idx == INVENT_PAGE)
            UpdateOrder(data[COMMON_TABLE::UPCCode].ToString());
    }
    else
        reportError();    
}

void UPC::DeleteData(int idx,DBRowData &data)
{
    bool ret = DBManager::Instance()->Delete(data);
    if(ret)
    {
        DelRow(idx,data); 
        refreshLimit(idx);

        if(idx == INVENT_PAGE)
            UpdateOrder(data[COMMON_TABLE::UPCCode].ToString());
        else if(idx == PO_PAGE)
            DelPo(data);
    }
    else
        reportError();    
}


bool UPC::AddPo(DBRowData &row)
{
    QString upccode = row[COMMON_TABLE::UPCCode];
    DBRowData invent(INVENT_TABLE::TABLE_NAME);    

    QVector<DBRowData> &invent_list = mDataBase[INVENT_PAGE];
    int invent_idx = FindDBRow(invent_list,COMMON_TABLE::UPCCode,row[COMMON_TABLE::UPCCode]);    
    int old_count = 0;    
    int new_count = row[PO_TABLE::Quantity];    
    
    if(invent_idx >= 0)
    {          
        invent = invent_list[invent_idx];
        old_count = invent[INVENT_TABLE::TotalInvent];                
    }
    else
    {
        CopyUPCTo(row,invent);
    }

    int sum_count = new_count + old_count;
    invent[INVENT_TABLE::IncomingInvent] = invent[INVENT_TABLE::IncomingInvent].ToInt() + new_count;    
    invent[INVENT_TABLE::TotalInvent] = sum_count;

    bool ret = true;
    if(invent_idx >= 0)
    {
        UpdateData(INVENT_PAGE,invent);                  
    }
    else
    {       
        InsertData(INVENT_PAGE,invent);        
    }

    return ret;
}

bool UPC::DelPo(DBRowData &po)
{
    int count = po[PO_TABLE::Quantity];
    UpdateInvent(po[COMMON_TABLE::UPCCode].ToString(),0,count);

    return true;
}

bool UPC::DonePo(DBRowData &po)
{            
    int invent_idx = FindDBRow(mDataBase[INVENT_PAGE],COMMON_TABLE::UPCCode,po[PO_TABLE::UPCCode]);    
    DBRowData invent = mDataBase[INVENT_PAGE][invent_idx];    

    int old_count = invent[INVENT_TABLE::Invent];
    double old_cost = invent[INVENT_TABLE::Cost];
    int new_count = po[PO_TABLE::Quantity];
    double new_cost = po[PO_TABLE::TotalCost];   

    int sum_count = new_count + old_count;
    invent[INVENT_TABLE::Invent] = invent[INVENT_TABLE::Invent].ToInt() + new_count;
    invent[INVENT_TABLE::IncomingInvent] = invent[INVENT_TABLE::IncomingInvent].ToInt() - new_count;
    invent[INVENT_TABLE::Cost] = (new_cost + old_cost) / sum_count;    

    UpdateData(INVENT_PAGE,invent);

    return true;
}

bool UPC::UpdateInvent(QString upc,int invent_count,int incoming_count)
{
    DBData upc_data(TYPE_STRING);
    upc_data = upc;

    int invent_idx = FindDBRow(mDataBase[INVENT_PAGE],COMMON_TABLE::UPCCode,upc_data);    
    DBRowData invent = mDataBase[INVENT_PAGE][invent_idx];    

    invent[INVENT_TABLE::Invent] = invent[INVENT_TABLE::Invent].ToInt() + invent_count;
    invent[INVENT_TABLE::IncomingInvent] = invent[INVENT_TABLE::IncomingInvent].ToInt() - incoming_count;
    invent[INVENT_TABLE::TotalInvent]    = invent[INVENT_TABLE::Invent].ToInt() + invent[INVENT_TABLE::IncomingInvent].ToInt();

    if(invent[INVENT_TABLE::TotalInvent].ToInt() == 0)
        DeleteData(INVENT_PAGE,invent);
    else
        UpdateData(INVENT_PAGE,invent);

    return true;
}

bool UPC::AddOrder(DBRowData &row)
{
    return false;
}

bool UPC::UpdateOrder(QString upccode)
{    
    DBData upc_data(TYPE_STRING);
    upc_data = upccode;

    int invent_idx = FindDBRow(mDataBase[INVENT_PAGE],COMMON_TABLE::UPCCode,upc_data);    
    int remain = 0;
    if(invent_idx >= 0)
    {
        DBRowData invent = mDataBase[INVENT_PAGE][invent_idx];
        remain = invent[INVENT_TABLE::Invent];
    }
    else
        remain = 0;

    QVector<int> idx_list = FindDBRows(mDataBase[ORDER_PAGE],COMMON_TABLE::UPCCode,upc_data);
    for(int i = 0; i < idx_list.size(); i++)
    {
        DBRowData &data = mDataBase[ORDER_PAGE][idx_list[i]];
        if(data[ORDERS_TABLE::Shipped].ToString() == ORDERS_TABLE::Shipped_None)
        {
            if(data[ORDERS_TABLE::Quantity].ToInt() <= remain)
                data[ORDERS_TABLE::Enough] = ORDERS_TABLE::Enough_YES;
            else
                data[ORDERS_TABLE::Enough] = ORDERS_TABLE::Enough_NO;

            UpdateData(ORDER_PAGE,data);
        }
    }

    return true;
}

bool UPC::DoneOrder(DBRowData &order)
{
    int invent_idx = FindDBRow(mDataBase[INVENT_PAGE],COMMON_TABLE::UPCCode,order[COMMON_TABLE::UPCCode]);    
    DBRowData invent = mDataBase[INVENT_PAGE][invent_idx];
    double cost = invent[INVENT_TABLE::Cost];
    double price = order[ORDERS_TABLE::Price];

    DBRowData sale(SALES_TABLE::TABLE_NAME);    

    QVector<DBRowData> &sales_list = mDataBase[SALES_PAGE];
    int sales_idx = FindDBRow(sales_list,COMMON_TABLE::UPCCode,order[COMMON_TABLE::UPCCode]);    
    double old_count = 0;
    double old_profit = 0;
    int sales_count = order[ORDERS_TABLE::Quantity];
    int new_sales = sales_count * price;
    double new_profit = sales_count * (price - cost);
    
    if(sales_idx >= 0)
    {          
        sale = sales_list[sales_idx];
        old_count   = sale[SALES_TABLE::TotalSales];
        old_profit  = sale[SALES_TABLE::Profit];
    }
    else
    {
        CopyUPCTo(order,sale);
    }
    sale[SALES_TABLE::TotalSales] = old_count + sales_count;
    sale[SALES_TABLE::Profit] = old_profit + new_profit;

    if(sales_idx >= 0)
        UpdateData(SALES_PAGE,sale);
    else
        InsertData(SALES_PAGE,sale);

    //更新库存
    UpdateInvent(order[COMMON_TABLE::UPCCode].ToString(),-1 * sales_count,0);    

    return true;
}

void UPC::AddRow(int idx,DBRowData &row)
{
    DBUiTableWidget *pDataTrans = dataTransList[idx];
    QVector<int> &disp_list = mDispIndex[idx];    

    mDataBase[idx].push_back(row);        
    disp_list.push_back(mDataBase[idx].size() - 1);        
    pDataTrans->addRow(mDataBase[idx].last());
}

void UPC::UpdateRow(int idx,DBRowData &row)
{
    DBUiTableWidget *pDataTrans = dataTransList[idx];        
    int data_idx = FindDBRow(mDataBase[idx],row.Key(),row[row.Key()]);

    mDataBase[idx][data_idx] = row;
    pDataTrans->updateRow(mDataBase[idx][data_idx]);    
}

void UPC::DelRow(int idx,DBRowData &row)
{
    DBUiTableWidget *pDataTrans = dataTransList[idx];        
    int data_idx = FindDBRow(mDataBase[idx],row.Key(),row[row.Key()]);
    QVector<int> &disp_list = mDispIndex[idx];    
    int disp_idx = disp_list.indexOf(data_idx);

    pDataTrans->deleteRow(row);
    mDataBase[idx].remove(data_idx);    
    for(int i = 0; i < disp_list.size(); i++)
    {
        if(disp_list[i] > data_idx)
            disp_list[i] -= 1;
    }
    disp_list.remove(disp_idx);    
}

void UPC::onBtnNew()
{
    int idx = btnNewList.indexOf((QPushButton*)sender());
    QString table_name = tableNameList[idx];    

    DBRowData row(table_name);

    UPCDialog dlg;
    dlg.setUPCInfo(mUpcMap);
    dlg.Init(row,DBUi::INSERT);    
    dlg.exec();
    if (dlg.result() != QDialog::Accepted)
        return;    
    
    row = dlg.rowData();
    InsertData(idx,row);       
}

void UPC::onBtnDelete()
{
    int idx = btnDeleteList.indexOf((QPushButton*)sender());    
    DBUiTableWidget *pDataTrans = dataTransList[idx];    
    QVector<int> &disp_list = mDispIndex[idx];    
    int row = pDataTrans->widget()->currentRow();   
    if(row == -1)
        return;

    int data_idx = disp_list[row];
    DBRowData data = mDataBase[idx][data_idx];
    DeleteData(idx,data);
}

void UPC::onBtnFind()
{
    int idx = btnFindList.indexOf((QPushButton*)sender());     
    DBUiTableWidget *pDataTrans = dataTransList[idx];
    QVector<DBRowData> &row_list = mDataBase[idx];
    QVector<int> &disp_list = mDispIndex[idx];
    QString find = lineFindList[idx]->text();    
    
    pDataTrans->widget()->setRowCount(0);
    disp_list.clear();   
    for(int i = 0; i < row_list.size(); i++)
    {
        DBRowData &row = row_list[i];
        if((row[COMMON_TABLE::UPCCode].ToString().indexOf(find) >= 0)
            || (row[COMMON_TABLE::Category].ToString().indexOf(find) >= 0)
            || (row[COMMON_TABLE::Name].ToString().indexOf(find) >= 0))
        {                
            disp_list.push_back(i);
        }
    }

    for(int i = 0; i < disp_list.size(); i++)
        pDataTrans->addRow(row_list[disp_list[i]]);
}

void UPC::onBtnFindInventLess()
{
    int less = ui.lineInventLess->text().toInt();

    int idx = INVENT_PAGE;     
    DBUiTableWidget *pDataTrans = dataTransList[idx];
    QVector<DBRowData> &row_list = mDataBase[idx];
    QVector<int> &disp_list = mDispIndex[idx];
    QString find = lineFindList[idx]->text();    
    
    pDataTrans->widget()->setRowCount(0);
    disp_list.clear();   
    for(int i = 0; i < row_list.size(); i++)
    {
        DBRowData &row = row_list[i];
        if(row[INVENT_TABLE::TotalInvent].ToInt() < less)
        {                
            disp_list.push_back(i);
        }
    }

    for(int i = 0; i < disp_list.size(); i++)
        pDataTrans->addRow(row_list[disp_list[i]]);
}

void UPC::onDateRefreshList()
{
    refreshList(ORDER_PAGE);
}

void UPC::editContextMenu(const QPoint &pos)
{
    QTableWidget *widget = NULL;

    int idx = -1;
    for(int i = 0; i < dataTransList.size(); i++)
    {
        if(sender() == dataTransList[i]->widget())
        {
            idx = i;
            widget = dataTransList[i]->widget();            
            break;
        }
    }

    int row = widget->currentRow();    
	if(row >= 0)
    {
        bool b_po    = (idx == PO_PAGE);
        bool b_order = (idx == ORDER_PAGE);

        QVector<DBRowData> &row_list = mDataBase[idx];         
        DBRowData data = row_list[mDispIndex[idx][row]];

        QMenu menu(widget);
        QAction *action0 = menu.addAction("浏览");
        QAction *action1 = menu.addAction("编辑");    
        QAction *action2 = menu.addAction("删除");        
        QAction *action3 = NULL;
        
        if(b_po || (b_order && data[ORDERS_TABLE::Shipped].ToString() == ORDERS_TABLE::Shipped_None
            &&  data[ORDERS_TABLE::Enough].ToString() == ORDERS_TABLE::Enough_YES))
            action3 = menu.addAction("完成订单");
                
        connect(action2,SIGNAL(triggered()),btnDeleteList[idx], SIGNAL(clicked()));
        QAction *sel = menu.exec(widget->mapToGlobal(pos));   
        if(!sel)
            return;

        if(sel == action0)
        {
            UPCDialog dlg;            
            dlg.Init(data,DBUi::BROWSER);            
            dlg.exec();  
        }
        else if(sel == action1)
        {                                    
            UPCDialog dlg;
            dlg.setUPCInfo(mUpcMap);
            dlg.Init(data,DBUi::EDIT);            
            dlg.exec();
            if (dlg.result() != QDialog::Accepted)
                return;    
            
            data = dlg.rowData();
            UpdateData(idx,data);            
        }
        else if(sel == action3)
        {            
            QString question = b_po? "确认收货":"确认交货";
            if(QMessageBox::Ok == QMessageBox::question(this,"",question,QMessageBox::Ok | QMessageBox::Cancel))
            {                
                if(b_po)                
                {               
                    int bak_count = data[PO_TABLE::Quantity]; //特殊处理下，避免删除时联动 
                    data[PO_TABLE::Quantity] = 0;
                    DeleteData(idx,data);
                    data[PO_TABLE::Quantity] = bak_count;
                    DonePo(data);                
                }
                else                
                {                   
                    QString text = currentDate();
                    text = QInputDialog::getText(this,"输入时间","输入时间(2016-05-01)",QLineEdit::Normal,text);
                    QDate date = QDate::fromString(text,"yyyy-MM-dd");
                    if(!date.isValid ())
                    {
                        QMessageBox::information(this,QString(),"请按格式输入");
                        return;
                    }

                    data[ORDERS_TABLE::Shipped] = ORDERS_TABLE::Shipped_Send;
                    data[ORDERS_TABLE::ShippedTime] = text;

                    UpdateData(idx,data);
                    DoneOrder(data);                
                }
            }
        }
    }
}


void UPC::inventContextMenu(const QPoint &pos)
{
    int row = ui.tableWidgetInvent->currentRow();    
	if(row >= 0)
	{        
        DBRowData invent = mDataBase[INVENT_PAGE][row];
        QString upccode = invent[COMMON_TABLE::UPCCode];

		QMenu menu(ui.tableWidgetInvent);
		QAction *action1 = menu.addAction("在进货中查看该物品");    
	    QAction *action2 = menu.addAction("在订单中查看该物品");
        QAction *action3 = menu.addAction("在销量中查看该物品");
        QAction *action4 = NULL;//menu.addAction("新入库物品");
		QAction *sel = menu.exec(ui.tableWidgetInvent->mapToGlobal(pos));        
        if(!sel)
            return;

        if(sel == action1)
        {
            showList(PO_PAGE,upccode);
        }
        else if(sel == action2)
        {
            showList(ORDER_PAGE,upccode);
        }
        else if(sel == action3)
        {
            showList(SALES_PAGE,upccode);
        }
        else if(sel == action4) 
        {
            int in = invent[INVENT_TABLE::Invent];
            int out = invent[INVENT_TABLE::IncomingInvent];

            bool ok = false;
            int count = QInputDialog::getInt(this,"新入库物品","新入库物品",out,1,out,1,&ok);
            if(ok)
            {
                in += count;
                out -= count;

                invent[INVENT_TABLE::Invent] = in;
                invent[INVENT_TABLE::IncomingInvent] = out; 
                UpdateData(INVENT_PAGE,invent);
            }
        }
	}
}
