#pragma once

#include <stdlib.h>

#include <QString>
#include <QVector>
#include <QMap>
#include <QStringList>


#define DB_ATTR(attr,name,type) type attr(){ return (*this)[name];} \
                                void set##attr(type var){ (*this)[name] = var;}

