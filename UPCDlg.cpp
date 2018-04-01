#include "UPCDlg.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include "DBDefine.h"

UPCDialog::UPCDialog(QWidget *parent)
    :DBUiDlg(parent)
{    
       
}

UPCDialog::~UPCDialog()
{

}

void UPCDialog::Init(DBRowData data,int type)
{
    mData = data;
    dbUi.SetDBRowData(&mData,type);    

    QString tableName = mData.GetTableName();
    QStringList list = data.GetNameList();        
    if(type == DBUi::INSERT)
    {        
        list.removeAll(COMMON_TABLE::ID);              
    }    
    if(type == DBUi::INSERT || type == DBUi::EDIT)
    {
        if(tableName == PO_TABLE::TABLE_NAME)
        {            
            list.removeAll(PO_TABLE::TotalCost);
        }
        else if(tableName == ORDERS_TABLE::TABLE_NAME)
        {
            list.removeAll(ORDERS_TABLE::Enough);
            list.removeAll(ORDERS_TABLE::ShippedTime);
            list.removeAll(ORDERS_TABLE::Shipped);
        }
    }
    dbUi.SetNameList(list);
    if(type == DBUi::EDIT)
        dbUi.SetWidgetAttr(COMMON_TABLE::UPCCode,true,false);

    dbUi.SetupWidget(mainWidget);  

    if(tableName != UPC_TABLE::TABLE_NAME)
    {
        if(type == DBUi::INSERT)
        {
            QComboBox *comBox = (QComboBox *)dbUi.GetWidget(COMMON_TABLE::UPCCode);
            connect(comBox,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(onUPCChanged(const QString &)));            

            upcLabelList.resize(3);
            upcLabelList[0] = (QLineEdit *)dbUi.GetWidget(COMMON_TABLE::Category);
            upcLabelList[1] = (QLineEdit *)dbUi.GetWidget(COMMON_TABLE::Name);
            upcLabelList[2] = (QLineEdit *)dbUi.GetWidget(COMMON_TABLE::Company);

            onUPCChanged(comBox->currentText());          
        }
        else
        {
            dbUi.SetWidgetAttr(COMMON_TABLE::UPCCode,true,false);

            if(tableName == PO_TABLE::TABLE_NAME)        
                dbUi.SetWidgetAttr(PO_TABLE::Quantity,true,false);                
            if(tableName == ORDERS_TABLE::TABLE_NAME)        
                dbUi.SetWidgetAttr(ORDERS_TABLE::Quantity,true,false);
        }

        dbUi.SetWidgetAttr(COMMON_TABLE::Category,true,false);
        dbUi.SetWidgetAttr(COMMON_TABLE::Name,true,false);
        dbUi.SetWidgetAttr(COMMON_TABLE::Company,true,false);                
    }
}

void UPCDialog::setUPCInfo(QMap<QString,UPC_INFO> upcMap)
{
    mUPCMap = upcMap;
}

void UPCDialog::onUPCChanged(const QString &text)
{
    upcLabelList[0]->setText(mUPCMap[text].category);
    upcLabelList[1]->setText(mUPCMap[text].name);
    upcLabelList[2]->setText(mUPCMap[text].company);
}

void UPCDialog::accept()
{    
    if(!dbUi.WidgetToData())        
    {
        QMessageBox::information(this,"",dbUi.GetErrorStr());        
        return;
    }
    
    QString error;
    QString tableName = mData.GetTableName();
    if(tableName == PO_TABLE::TABLE_NAME || tableName == ORDERS_TABLE::TABLE_NAME)
    {
        int count = mData[COMMON_TABLE::Quantity].ToInt();
        if(count <= 0)        
            error = "数量必须为正数";                    
    }

    if(!error.isEmpty())
    {
        QMessageBox::information(this,QString(),error);
        return;
    }

    if(tableName == PO_TABLE::TABLE_NAME)
    {
        mData[PO_TABLE::TotalCost] = mData[PO_TABLE::ProductCost].ToDouble() + mData[PO_TABLE::ShipperCost].ToDouble();        
    }
    else if(tableName == ORDERS_TABLE::TABLE_NAME)
    {
        mData[ORDERS_TABLE::Enough] = ORDERS_TABLE::Enough_NO;
        mData[ORDERS_TABLE::Shipped] = ORDERS_TABLE::Shipped_None;
    }

    QDialog::accept();
}