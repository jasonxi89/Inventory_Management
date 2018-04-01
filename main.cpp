#include "upc.h"
#include <QApplication>
#include <QTextStream>
#include <QTextCodec>
#include <QSettings>
#include "DBManager_MySql.h"
#include "DBDefine.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);       
    a.addLibraryPath("./plugins");

    QStringList drivers = QSqlDatabase::drivers(); 

    QString setting_file = QCoreApplication::applicationDirPath() + "/Option.ini";
    QSettings settings(setting_file, QSettings::IniFormat);    
    QString host = settings.value("host").toString(); //host
    QString database = settings.value("database").toString(); //database
	QString name = settings.value("name").toString(); //ÓÃ»§ÕËºÅ	
	QString pass = settings.value("pass").toString(); //ÓÃ»§ÃÜÂë

    DBManager_MySql mysql;
    mysql.SetHost(host,database,name,pass);

    DBManager::SetInstance(&mysql);

	if (!DBInitTable())
	{		
		QMessageBox::information(NULL, QString(), "数据库初始化失败");
		return 0;
	}

    UPC w;
    w.show();
    return a.exec();
}
