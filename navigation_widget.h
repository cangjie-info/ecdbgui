#ifndef NAVIGATION_WIDGET_H
#define NAVIGATION_WIDGET_H

#include <QWidget>
#include <QList>
#include <QLineEdit>
#include <QComboBox>
#include "entity_list.h"
#include "db_handler.h"
#include "surface_window.h"

class SurfaceWindow; //forward declaration
    //required because of cyclic dependency btw this and SurfaceWindow

class NavigationWidget : public QWidget
{
   Q_OBJECT
public:
   explicit NavigationWidget(DbHandler const * database, SurfaceWindow *parent = 0);

signals:
   void setSurf(int surfId);

public slots:

private:
   DbHandler const * db;
   QLineEdit* lePubSurfName;
   QList<EntityList> listOfLists; //list of lists of surface is
   int listIndex; //index of the current list within the list of lists.
   QComboBox* comboListOfLists;
   void printLists(); //debugging function
   SurfaceWindow* parentSurfaceWindow;

private slots:
   void pubSurf2id();
   void newList();
   void deleteList();
   void setListIndex(int index);
   void addToList();
   void deleteFromList();
};

#endif // NAVIGATION_WIDGET_H
