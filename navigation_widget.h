#ifndef NAVIGATION_WIDGET_H
#define NAVIGATION_WIDGET_H

#include <QWidget>
#include <QList>
#include <QLineEdit>
#include <QComboBox>
#include "entity_list.h"
#include "db_handler.h"

class NavigationWidget : public QWidget
{
   Q_OBJECT
public:
   explicit NavigationWidget(DbHandler const * database, QWidget *parent = 0);

signals:
   void setSurf(int surfId);

public slots:

private:
   DbHandler const * db;
   QLineEdit* lePubSurfName;
   QList<EntityList> listOfLists; //list of lists of surface is
   int listIndex; //index of the current list within the list of lists.
   QComboBox* comboListOfLists;

private slots:
   void pubSurf2id();
   void newList();
   void deleteList();
   void setListIndex(int index);
};

#endif // NAVIGATION_WIDGET_H
