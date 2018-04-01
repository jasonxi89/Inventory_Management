#include "DBManager.h"

DBManager *DBManager::g_inst = NULL;
DBManager *DBManager::Instance()
{	
	return g_inst;
}

void DBManager::SetInstance(DBManager *inst)
{
    g_inst = inst;
}
