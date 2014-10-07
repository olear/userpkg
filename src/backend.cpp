#include "backend.h"

Backend::Backend(QObject *parent) :
    QObject(parent)
{
    moveToThread(&t);
    t.start();
}

Backend::~Backend()
{
    t.quit();
    t.wait();
}

void Backend::requestUpdateList()
{
    QMetaObject::invokeMethod(this, "updateList");
}

void Backend::updateList()
{
    QStringList result;
    result = pkgsrc.packageUpdates();
    if (!result.isEmpty())
        emit updateListResult(result);
}
