#ifndef NAVIGATION_WIDGET_H
#define NAVIGATION_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include "db_handler.h"

class NavigationWidget : public QWidget
{
   Q_OBJECT
public:
   explicit NavigationWidget(DbHandler const * database, QWidget *parent = 0);

signals:
   void setSurf(int surfId);
   /*
   void forward();
   void forward10();
   void forward100();
   void back();
   void back10();
   void back100();
*/
public slots:

private:
   DbHandler const * db;
   QLineEdit* lePubSurfName;

private slots:
   void pubSurf2id();
};

#endif // NAVIGATION_WIDGET_H
