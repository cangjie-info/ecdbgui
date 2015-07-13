#include "navigation_widget.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

NavigationWidget::NavigationWidget(QWidget *parent) :
   QWidget(parent)
{
   QVBoxLayout* navigationLayout = new QVBoxLayout(this);
   QLabel* lbPub = new QLabel("Publication nav.");
   navigationLayout->addWidget(lbPub);
   QHBoxLayout* pubNavigationLayout = new QHBoxLayout(this);
   QPushButton* btnBack100 = new QPushButton("<<<");
   pubNavigationLayout->addWidget(btnBack100);
   QPushButton* btnBack10 = new QPushButton("<<");
   pubNavigationLayout->addWidget(btnBack10);
   QPushButton* btnBack = new QPushButton("<");
   pubNavigationLayout->addWidget(btnBack);
   QPushButton* btnForward = new QPushButton(">");
   pubNavigationLayout->addWidget(btnForward);
   QPushButton* btnForward10 = new QPushButton(">>");
   pubNavigationLayout->addWidget(btnForward10);
   QPushButton* btnForward100 = new QPushButton(">>>");
   pubNavigationLayout->addWidget(btnForward100);

   navigationLayout->addLayout(pubNavigationLayout);

   connect(btnBack, SIGNAL(clicked()), parent, SLOT(back()));
   connect(btnBack10, SIGNAL(clicked()), parent, SLOT(back10()));
   connect(btnBack100, SIGNAL(clicked()), parent, SLOT(back100()));
   connect(btnForward, SIGNAL(clicked()), parent, SLOT(advance()));
   connect(btnForward10, SIGNAL(clicked()), parent, SLOT(advance10()));
   connect(btnForward100, SIGNAL(clicked()), parent, SLOT(advance100()));


   QList<QPushButton *> allBtns = this->findChildren<QPushButton *>();
   foreach(QPushButton* btn, allBtns)
   {
      btn->setMaximumWidth(30);
   }

   setLayout(navigationLayout);
}
