#include "entity_list.h"
#include <QDebug>

EntityList::EntityList()
    : index(-1)
{
    return;
}

EntityList::EntityList(EntityType entity_type, QString name)
   : index(-1), type(entity_type), listName(name)
{
    return;
}

void EntityList::setName(QString newName)
{
   listName = newName;
   dirty = true;
}

void EntityList::moveLast()
{
   index = size() - 1;
}

void EntityList::moveFirst()
{
   if(isEmpty()) index = -1;
   else index = 0;
}

void EntityList::forward()
{
    if(index < size() - 1) {
        index++;
    }
}

void EntityList::back() {
    if(index > 0) {
        index--;
    }
}

int EntityList::current()
{
    if(!isEmpty()) {
        return at(index);
    }
    else {
        return 0;
    }
}

void EntityList::displayList()
{
   qDebug() << QString("ENTITY LIST: %1").arg(listName);
   for(int i = 0; i < size(); i++)
   {
      qDebug() << at(i);
   }
}
