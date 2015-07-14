#ifndef SURFACE_LIST_H
#define SURFACE_LIST_H
#include <QList>
#include <QString>
#include <QPair>

class SurfaceList : public QList<QPair<QString, int> > //string first for sorting
{
public:
   SurfaceList();
   void moveLast();
   void displayList();
private:
   int index;
};

#endif // SURFACE_LIST_H
