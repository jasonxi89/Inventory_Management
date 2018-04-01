#ifndef UPC_H
#define UPC_H

#include <QMainWindow>
#include "ui_upc.h"
#include "UPCDlg.h"
#include "DBFilter.h"

class UPC : public QMainWindow
{
    Q_OBJECT

public:
    UPC(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~UPC();

protected slots:
    void changePage();
    void backToMain();

    void onBtnNew();    
    void onBtnDelete();
    void onBtnFind();
    void onBtnFindInventLess();
    void onComboBoxChanged();
    void onDateRefreshList();

    void inventContextMenu(const QPoint&);
    void editContextMenu(const QPoint&);
private:    
    void reportError();    
    bool dataFilter(int idx,DBRowData &data);
    void InsertData(int idx,DBRowData &row);
    void UpdateData(int idx,DBRowData &row);
    void DeleteData(int idx,DBRowData &row);

    void updateDataBase(int idx);
    void refreshLimit(int idx);
    void refreshList(int idx);    
    void showList(int idx,QString upccode);

    int FindDBRow(QVector<DBRowData> &row_list,QString name,DBData data);
    QVector<int> FindDBRows(QVector<DBRowData> &row_list,QString name,DBData data);
    void CopyUPCTo(const DBRowData &src,DBRowData &des);

    void AddRow(int idx,DBRowData &row);
    void UpdateRow(int idx,DBRowData &row);
    void DelRow(int idx,DBRowData &row);

    bool AddPo(DBRowData &row);       //增加进货     
    bool DelPo(DBRowData &row);       //删除进货     
    bool DonePo(DBRowData &row);      //完成进货
    bool UpdateInvent(QString upc,int invent,int incoming); //更新库存
    bool AddOrder(DBRowData &row);    //增加订单
    bool UpdateOrder(QString upc);    //更新订单
    bool DoneOrder(DBRowData &row);   //完成订单

    QStringList tableNameList;    
    QVector<QPushButton*> btnNewList;
    QVector<QPushButton*> btnDeleteList;
    QVector<QPushButton*> btnFindList;
    QVector<QLineEdit*>   lineFindList;
    QVector<DBUiTableWidget*> dataTransList;
    QVector<QVector<DBRowData> > mDataBase;
    QVector<QVector<int> > mDispIndex;  //显示到实际的索引

    QVector<QVector<QComboBox*> > mComboBoxList;
    QVector<QVector<QString> >    mFilterList;

    Ui::UPCClass ui;
    DBUiTableWidget upcDataTrans;
    DBUiTableWidget poDataTrans;
    DBUiTableWidget inventDataTrans;
    DBUiTableWidget orderDataTrans;
    DBUiTableWidget salesDataTrans;

    QMap<QString,UPC_INFO> mUpcMap;
    QString mErrorStr;
};

#endif // UPC_H
