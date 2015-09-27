#include "navigation_widget.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStringList>

NavigationWidget::NavigationWidget(DbHandler const* database, QWidget *parent) :
   QWidget(parent), db(database)
{
   QVBoxLayout* navigationLayout = new QVBoxLayout(this);
   QLabel* lbPub = new QLabel("<h2>Publication navigation</h2>");
   navigationLayout->addWidget(lbPub);

   QHBoxLayout* pubSurfNameLayout = new QHBoxLayout(this);
   QLabel* lbPubSurfName = new QLabel("Publication name and/or surface:");
   lePubSurfName = new QLineEdit();
         //Q: why does this have to be a member, unlike the buttons?
         //A: becuase the slot it triggers needs to access its .text member.
   pubSurfNameLayout->addWidget(lbPubSurfName);
   pubSurfNameLayout->addWidget(lePubSurfName);
   navigationLayout->addLayout(pubSurfNameLayout);


   QHBoxLayout* pubNavBtnLayout = new QHBoxLayout(this);
   QPushButton* btnBack100 = new QPushButton("<<<");
   pubNavBtnLayout->addWidget(btnBack100);
   QPushButton* btnBack10 = new QPushButton("<<");
   pubNavBtnLayout->addWidget(btnBack10);
   QPushButton* btnBack = new QPushButton("<");
   pubNavBtnLayout->addWidget(btnBack);
   QPushButton* btnForward = new QPushButton(">");
   pubNavBtnLayout->addWidget(btnForward);
   QPushButton* btnForward10 = new QPushButton(">>");
   pubNavBtnLayout->addWidget(btnForward10);
   QPushButton* btnForward100 = new QPushButton(">>>");
   pubNavBtnLayout->addWidget(btnForward100);

   navigationLayout->addLayout(pubNavBtnLayout);

   connect(btnBack, SIGNAL(clicked()), parent, SLOT(back()));
   connect(btnBack10, SIGNAL(clicked()), parent, SLOT(back10()));
   connect(btnBack100, SIGNAL(clicked()), parent, SLOT(back100()));
   connect(btnForward, SIGNAL(clicked()), parent, SLOT(advance()));
   connect(btnForward10, SIGNAL(clicked()), parent, SLOT(advance10()));
   connect(btnForward100, SIGNAL(clicked()), parent, SLOT(advance100()));

   connect(lePubSurfName, SIGNAL(returnPressed()), this, SLOT(pubSurf2id()));

   QList<QPushButton *> allBtns = this->findChildren<QPushButton *>();
   foreach(QPushButton* btn, allBtns)
   {
      btn->setMaximumWidth(30);
   }

   setLayout(navigationLayout);
}

void NavigationWidget::pubSurf2id()
{
   QStringList list = lePubSurfName->text().split(":");
   QString pubName = list.at(0);
   QString surfName = "";
   if(list.size()>1) surfName = list.at(1);
   //call dbHandler function to return the id of that surface
   int newSurfId = db->pubSurf2Id(pubName, surfName);
   if(newSurfId == 0); // do nothing
   else emit setSurf(newSurfId);
}

