#ifndef NAVIGATION_WIDGET_H
#define NAVIGATION_WIDGET_H

#include <QWidget>

class NavigationWidget : public QWidget
{
   Q_OBJECT
public:
   explicit NavigationWidget(QWidget *parent = 0);

signals:
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
};

#endif // NAVIGATION_WIDGET_H
