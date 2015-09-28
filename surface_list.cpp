#include "surface_list.h"
#include <QDebug>

int SurfaceList::count = 0;

SurfaceList::SurfaceList()
   : index(-1)
{
   count++;
   listName = QString("list%1").arg(count);
}

void SurfaceList::setName(QString newName)
{
   listName = newName;
}

void SurfaceList::moveLast()
{
   index = size() - 1;
}

void SurfaceList::moveFirst()
{
   if(isEmpty()) index = -1;
   else index = 0;
}

void SurfaceList::displayList()
{
   qDebug() << QString("SURFACES LIST: %1").arg(listName);
   for(int i = 0; i < size(); i++)
   {
      qDebug() << at(i);
   }
}
