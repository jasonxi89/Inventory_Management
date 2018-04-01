#include "DBUiSetup.h"
#include "DBTableManager.h"
#include <QGridLayout>
#include <QTextEdit>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QDateEdit>

enum{
    OPT_LINE_EDIT,
    OPT_COMBO_BOX,
    OPT_DATE_EDIT,
};

static const char *DateFormate = "yyyy-MM-dd";

DBUiSetup::DBUiSetup()	
{	
    mType = DBUi::INSERT;
}

DBUiSetup::~DBUiSetup(void)
{
}

void DBUiSetup::SetDBRowData(DBRowData *data,int type)
{
	mData = data;
    mNameList.clear();
    mWidgetList.clear();
    mShowList.clear();
    mEditList.clear();
    
    if(data)
    {
        mType = type;
        mNameList = data->GetNameList();
        InitWidgetInfo();
    }    
}

DBRowData *DBUiSetup::GetDBRowData()
{
	return mData;
}


void DBUiSetup::InitWidgetInfo()
{
    mShowList.resize(mNameList.size());
    mEditList.resize(mNameList.size());
    mWidgetList.resize(mNameList.size());

    //设置默认显示
    mShowList.fill(true);
    mEditList.fill(true);
    if(mType != DBUi::CUSTOM)
    {
        const DBTableInfo &table_info = DBTableManager::Instance()->GetTableInfo(mData->GetTableName());
        for(int i = 0; i < mNameList.size(); i++)
        {
            const DBDataInfo *data_info = table_info.GetDataInfo(mNameList[i]);
            if(mType == DBUi::INSERT)
            {
                if(data_info->autoGen())
                {
                    mShowList[i] = false;
                    mEditList[i] = false;
                }
                if(data_info->autoID())
                    mEditList[i] = false;
            }
            else if(mType == DBUi::EDIT)
            {
                if(data_info->autoGen() && data_info->autoID())
                    mEditList[i] = false;
                if(mNameList[i] == table_info.GetKey())
                    mEditList[i] = false;
            }
            else
            {
                mEditList[i] = false;
            }
        }
    }    
}

QString DBUiSetup::GetErrorStr()
{
	return errorStr;
}

int DBUiSetup::OptControlType(int idx,const DBDataInfo *data_info,const DBDataLimit *limit)
{
	if(!mEditList[idx])
		return OPT_LINE_EDIT;

    if(limit && limit->type == DBDataLimit::LIMIT_LIST)
        return OPT_COMBO_BOX;

    if(data_info->dataType == TYPE_DATE)
        return OPT_DATE_EDIT;

    return OPT_LINE_EDIT;
}

void DBUiSetup::SetWidgetAttr(const QString &name,bool show,bool edit)
{
    for(int i = 0 ; i < (int)mNameList.size(); i++)
	{		
		if(mNameList[i] == name)
        {
	        mShowList[i] = show;
            mEditList[i] = edit;	
            UpdateWidgetAttr(i);
            return;
        }
	}    

    Q_ASSERT(0);
}

void DBUiSetup::SetNameList(class QStringList &list)
{
    mNameList = list;
    InitWidgetInfo();
}

QWidget *DBUiSetup::GetWidget(QString name)
{
    int i = mNameList.indexOf(name);	
    if(i >= 0)
        return mWidgetList[i];
    else
        return NULL;
}

void DBUiSetup::UpdateWidgetAttr(int i)
{
    QWidget *widget = mWidgetList[i];
    if(!widget)
        return;

    if(widget->inherits("QLineEdit"))
	{
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(widget);
		lineEdit->setReadOnly(!mEditList[i]);
	}
    else
        widget->setEnabled(false);
}

void DBUiSetup::SetupWidget(QWidget *widget)
{
    //创建控件
	QGridLayout  *main_layout = new QGridLayout(widget);		

    const QStringList &tableNameList = mData->GetNameList();	
	int count = mNameList.size();
    int layout_idx = 0;
	for(int i = 0; i < count; i++)
	{				        
        if(!mShowList[i])
            continue;

        QString name = mNameList[i];
        
        QWidget *control_widget = NULL;
	    const DBDataInfo  *data_info  = mData->GetTableInfo().GetDataInfo(name);
	    const DBDataLimit *data_limit = mData->GetTableInfo().GetDataLimit(name);
		
	    QLabel *lable = new QLabel(widget);		
	    lable->setText(data_info->dispName);		

	    int type = OptControlType(i,data_info,data_limit);		
	    if(type == OPT_LINE_EDIT)
	    {
		    QLineEdit *lineEdit = new QLineEdit(widget);		    
            if(!mEditList[i])
                lineEdit->setReadOnly(true);

            control_widget = lineEdit;
	    }
	    else if(type == OPT_COMBO_BOX)
	    {
		    QComboBox *comboBox = new QComboBox(widget);
            comboBox->addItems(data_limit->dataList);

		    control_widget = comboBox;
	    }
        else if(type == OPT_DATE_EDIT)
        {
            QDateEdit *dateEdit = new QDateEdit(widget);		                
            dateEdit->setCalendarPopup(true);   
            dateEdit->setDisplayFormat(DateFormate);

            control_widget = dateEdit;
        }

	    main_layout->addWidget(lable,layout_idx,0);						
	    main_layout->addWidget(control_widget,layout_idx,1);
        layout_idx++;

        mWidgetList[i] = control_widget;		        	    		
	}	
	widget->setLayout(main_layout);

	DataToWidget();
}

void DBUiSetup::SetWidgetList(QStringList &set_name_list,QVector<QWidget*> &widget_list)
{
    set_name_list = set_name_list;
    mWidgetList = widget_list;
    
    InitWidgetInfo();    
}

bool DBUiSetup::WidgetToData()
{	
	for(int i = 0; i < (int)mNameList.size(); i++)
	{
        QWidget *widget = mWidgetList[i];
        QString name = mNameList[i];
        if(!widget)
            continue;
        
		const DBDataInfo *data_info  = mData->GetTableInfo().GetDataInfo(name);
		QString data_str;					
		if(widget->inherits("QLineEdit"))
		{
            QLineEdit *lineEdit = qobject_cast<QLineEdit*>(widget);
			data_str = lineEdit->text();
		}
		else if(widget->inherits("QComboBox"))
		{
            QComboBox *comboBox = qobject_cast<QComboBox*>(widget);
			data_str = comboBox->currentText();
		}
        else if(widget->inherits("QTextEdit"))
        {
            QTextEdit *textEdit = qobject_cast<QTextEdit*>(widget);
			data_str = textEdit->toPlainText();
        }
        else if(widget->inherits("QDateEdit"))
        {
            QDateEdit *dateEdit = qobject_cast<QDateEdit*>(widget);
			data_str = dateEdit->date().toString(DateFormate);
        }
        else
			Q_ASSERT(0);

        if(data_str.isEmpty() && data_info->mastHave())			
		{
			errorStr = QObject::tr("字段:") + data_info->dispName + QObject::tr("不能为空");
			return false;
		}

		(*mData)[name].FromString(data_str);
	}

	return true;
}

void DBUiSetup::DataToWidget()
{
    for(int i = 0; i < (int)mNameList.size(); i++)
	{
        QWidget *widget = mWidgetList[i];
        QString name = mNameList[i];
        if(widget == NULL)
            continue;
		
		DBData &data = (*mData)[name];
		QString data_str = data.ToString();

		if(widget->inherits("QLineEdit"))
		{
			QLineEdit *lineEdit = qobject_cast<QLineEdit*>(widget);
			lineEdit->setText(data_str);
		}
		else if(widget->inherits("QComboBox"))
		{
			QComboBox *comboBox = qobject_cast<QComboBox*>(widget);
			for(int item_idx = 0; item_idx < comboBox->count(); item_idx++)
			{
				if(comboBox->itemText(item_idx) == data_str)
				{
					comboBox->setCurrentIndex(item_idx);
					break;
				}
			}			
		}
		else if(widget->inherits("QTextEdit"))
		{
			QTextEdit *textEdit = qobject_cast<QTextEdit*>(widget);
			textEdit->setText(data_str);
		}
        else if(widget->inherits("QDateEdit"))
        {
            QDateEdit *dateEdit = qobject_cast<QDateEdit*>(widget);
            dateEdit->setDate(QDate::fromString(data_str,DateFormate));
        }
		else
			Q_ASSERT(0);
	}
}

//DBUiTableWidget
DBUiTableWidget::DBUiTableWidget()
{
    keyIndex = -1;
    tableWidget = NULL;    
}

void DBUiTableWidget::initTable(QString table_name)
{    
    const DBTableInfo &table_info = DBTableManager::Instance()->GetTableInfo(table_name);
    int count = table_info.GetRowCount();
    keyIndex = table_info.GetIndex(table_info.GetKey());
    
    tableWidget->setColumnCount(count);            
    for(int i = 0; i < count; i++)
    {
        const DBDataInfo *p_data_info = table_info.GetDataInfo(i);
        nameList.push_back(p_data_info->dispName);        
    }
    tableWidget->setHorizontalHeaderLabels(nameList);
}

QTableWidget *DBUiTableWidget::widget()
{
    return tableWidget;
}

void DBUiTableWidget::setWidget(QTableWidget *widget)   
{
    tableWidget = widget;

    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers );
}

void DBUiTableWidget::addRow(const DBRowData &row)
{
    int idx = tableWidget->rowCount();
    tableWidget->insertRow(idx);          

    QTableWidgetItem *item = new QTableWidgetItem();				      
    for(int i = 0; i < row.GetCount(); i++)
    {                
        QTableWidgetItem *newItem = new QTableWidgetItem(row[i].ToString());
        tableWidget->setItem(idx,i,newItem);        
    }    
}

int DBUiTableWidget::findRow(const DBRowData &row)
{
    if(keyIndex == -1)
        return -1;

    QString key = nameList[keyIndex];
    int count = tableWidget->rowCount();
    for(int i = 0; i < count; i++)
    {                
        if(tableWidget->item(i,keyIndex)->text() == row[key].ToString())
            return i;
    }

    return -1;
}

void DBUiTableWidget::deleteRow(const DBRowData &row)
{
    int idx = findRow(row);
    if(idx == -1)
        return;

    tableWidget->removeRow(idx);
}

void DBUiTableWidget::updateRow(const DBRowData &row)
{
    int idx = findRow(row);
    if(idx == -1)
        return;

    QTableWidgetItem *item = new QTableWidgetItem();				      
    for(int i = 0; i < row.GetCount(); i++)
    {                
        QTableWidgetItem *newItem = new QTableWidgetItem(row[i].ToString());
        tableWidget->setItem(idx,i,newItem);        
    } 
}