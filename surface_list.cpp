#include "surface_list.h"
#include <QDebug>

SurfaceList::SurfaceList()
   : index(-1)
{
}

void SurfaceList::moveLast()
{
   index = size() - 1;
}

void SurfaceList::displayList()
{
   qDebug() << "SURFACES LIST";
   for(int i = 0; i < size(); i++)
   {
      qDebug() << at(i);
   }
}
