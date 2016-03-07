#include "navigation_widget.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStringList>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

NavigationWidget::NavigationWidget(DbHandler const* database, QWidget *parent) :
   QWidget(parent), db(database), listIndex(-1)
{
   QVBoxLayout* navigationLayout = new QVBoxLayout();
// LIST NAVIGATION
   QLabel* lbList = new QLabel("<h2>List navigation</h2>");
   navigationLayout->addWidget(lbList);

   comboListOfLists = new QComboBox();
   connect(comboListOfLists, SIGNAL(currentIndexChanged(int)), this, SLOT(setListIndex(int)));
   navigationLayout->addWidget(comboListOfLists);

   QHBoxLayout* listButtonsLayout = new QHBoxLayout();

   QPushButton* btnNewList = new QPushButton("New list");
   listButtonsLayout->addWidget(btnNewList);
   QPushButton* btnDeleteList = new QPushButton("Delete list");
   listButtonsLayout->addWidget(btnDeleteList);
   QPushButton* btnLoadList = new QPushButton("Load list");
   listButtonsLayout->addWidget(btnLoadList);
   QPushButton* btnSaveList = new QPushButton("Save list");
   listButtonsLayout->addWidget(btnSaveList);

   navigationLayout->addLayout(listButtonsLayout);

   connect(btnNewList, SIGNAL(clicked()), this, SLOT(newList()));
   connect(btnDeleteList, SIGNAL(clicked()), this, SLOT(deleteList()));

//PUBLICATION NAVIGATION
   QLabel* lbPub = new QLabel("<h2>Publication navigation</h2>");
   navigationLayout->addWidget(lbPub);

   QHBoxLayout* pubSurfNameLayout = new QHBoxLayout();
   QLabel* lbPubSurfName = new QLabel("Pub name and/or surface:");
   lePubSurfName = new QLineEdit();
         //Q: why does this have to be a member, unlike the buttons?
         //A: becuase the slot it triggers needs to access its .text member.
   pubSurfNameLayout->addWidget(lbPubSurfName);
   pubSurfNameLayout->addWidget(lePubSurfName);
   navigationLayout->addLayout(pubSurfNameLayout);


   QHBoxLayout* pubNavBtnLayout = new QHBoxLayout();
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

   for (int i = 0; i < pubNavBtnLayout->count(); i++)
   {
      QWidget* btn = pubNavBtnLayout->itemAt(i)->widget();
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

void NavigationWidget::newList() {
    QString listName = QInputDialog::getText(this, "List name",
            "New list name:");
    if(listName.isEmpty()) {
        return;
    }
    else {
        comboListOfLists->addItem(listName);
        EntityList* list = new EntityList(surface, listName);
        listOfLists.append(*list);
    }
}

void NavigationWidget::setListIndex(int index) {
    listIndex = index;
}

void NavigationWidget::deleteList() {
    qDebug() << "number of lists = " << listOfLists.size();
    if(listOfLists.size() < 1) return;
    QMessageBox msgBox;

    QString text = "Current list = " + comboListOfLists->currentText();
    msgBox.setText(text);
    msgBox.setInformativeText("Are you sure you want to delete this list?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    if(ret == QMessageBox::No) {
        return;
    }
    else {
        comboListOfLists->removeItem(comboListOfLists->currentIndex());
        comboListOfLists->setCurrentIndex(0); // no harm if combo is now empty
        listOfLists.removeAt(listIndex);
        if(listOfLists.size() > 0) {
            listIndex = 0;
        }
        else {
            listIndex = -1;
        }
    }
}

/*
void NavigationWidget::addCurrentSurfToList()
{
   QPair<QString, int> surfPair;
   surfPair.first = parent->trans.getPubName() + parent->trans.getPubNumber();
   surfPair.second = parent->currentSurfId;
   if(surfList.contains(surfPair)) return;
   surfList.append(surfPair);
   surfList.moveLast(); //so that the newly apended surf is current in the list.
   surfList.displayList();
}

void NavigationWidget::sortList()
{
   using namespace::std;
   std::sort(surfList.begin(), surfList.end());
}
*/
