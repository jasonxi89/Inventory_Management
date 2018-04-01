#include "DBUtils.h"

QString str_to_QStr(string str)
{
    return QString::fromLocal8Bit(str.c_str());
}

string QStr_to_str(QString qstr)
{
    QByteArray cdata = qstr.toLocal8Bit();
    return string(cdata);
}
