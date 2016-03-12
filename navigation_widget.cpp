#include "navigation_widget.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStringList>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

NavigationWidget::NavigationWidget(DbHandler const* database, SurfaceWindow *parent) :
   QWidget(parent), db(database), listIndex(-1), parentSurfaceWindow(parent)
{
   QVBoxLayout* navigationLayout = new QVBoxLayout();
// LIST NAVIGATION
// heading
   QLabel* lbList = new QLabel("List navigation");
   navigationLayout->addWidget(lbList);
// lists combo
   QHBoxLayout* comboLayout = new QHBoxLayout();
   comboListOfLists = new QComboBox();
   comboLayout->addStretch();
   QLabel* lblCombo = new QLabel("Lists: ");
   comboLayout->addWidget(lblCombo);
   comboLayout->addWidget(comboListOfLists);
   connect(comboListOfLists, SIGNAL(currentIndexChanged(int)), this, SLOT(setListIndex(int)));
   navigationLayout->addLayout(comboLayout);
//buttons 1
   QHBoxLayout* listButtonsLayout = new QHBoxLayout();
   QPushButton* btnNewList = new QPushButton("New");
   listButtonsLayout->addWidget(btnNewList);
   QPushButton* btnDeleteList = new QPushButton("Del");
   listButtonsLayout->addWidget(btnDeleteList);
   QPushButton* btnLoadList = new QPushButton("Load");
   listButtonsLayout->addWidget(btnLoadList);
   QPushButton* btnSaveList = new QPushButton("Save");
   listButtonsLayout->addWidget(btnSaveList);
//buttons 2
   QHBoxLayout* listButtonsLayout2 = new QHBoxLayout;
   QPushButton* btnAddToList = new QPushButton("Add");
   listButtonsLayout2->addWidget(btnAddToList);
   QPushButton* btnDeleteFromList = new QPushButton("Remove");
   listButtonsLayout2->addWidget(btnDeleteFromList);
//buttons 3
   QHBoxLayout* listButtonsLayout3 = new QHBoxLayout;
   QPushButton* btnListBack100 = new QPushButton("<<<");
   listButtonsLayout3->addWidget(btnListBack100);
   QPushButton* btnListBack10 = new QPushButton("<<");
   listButtonsLayout3->addWidget(btnListBack10);
   QPushButton* btnListBack = new QPushButton("<");
   listButtonsLayout3->addWidget(btnListBack);
   QPushButton* btnListForward = new QPushButton(">");
   listButtonsLayout3->addWidget(btnListForward);
   QPushButton* btnListForward10 = new QPushButton(">>");
   listButtonsLayout3->addWidget(btnListForward10);
   QPushButton* btnListForward100 = new QPushButton(">>>");
   listButtonsLayout3->addWidget(btnListForward100);

   for(int i = 0; i < listButtonsLayout->count(); i++) {
       QWidget* btn = listButtonsLayout->itemAt(i)->widget();
       btn->setMaximumWidth(50);
   }
   for (int i = 0; i < listButtonsLayout3->count(); i++)
   {
      QWidget* btn = listButtonsLayout3->itemAt(i)->widget();
      btn->setMaximumWidth(30);
   }
   setLayout(navigationLayout);

   navigationLayout->addLayout(listButtonsLayout);
   navigationLayout->addLayout(listButtonsLayout2);
   navigationLayout->addLayout(listButtonsLayout3);

   connect(btnNewList, SIGNAL(clicked()), this, SLOT(newList()));
   connect(btnDeleteList, SIGNAL(clicked()), this, SLOT(deleteList()));
   connect(btnAddToList, SIGNAL(clicked()), this, SLOT(addToList()));
   connect(btnDeleteFromList, SIGNAL(clicked()), this, SLOT(deleteFromList()));

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
    printLists();
}

void NavigationWidget::setListIndex(int index) {
    listIndex = index;
    printLists();
}

void NavigationWidget::deleteList() {
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
        listOfLists.removeAt(listIndex);
        comboListOfLists->removeItem(comboListOfLists->currentIndex());
            //this may change listIndex (via SIGNAL/SLOT), but we reset it below
        comboListOfLists->setCurrentIndex(0); // no harm if combo is now empty
        if(listOfLists.size() > 0) {
            listIndex = 0;
        }
        else {
            listIndex = -1;
        }
    }
    printLists();
}

void NavigationWidget::printLists() {
    qDebug() << listOfLists.size();
    EntityList l;
    foreach(l, listOfLists) {
        qDebug() << l.getName();
    }
}

void NavigationWidget::addToList() {
    if(listIndex == -1) return; //no list to add to
    int currentId = parentSurfaceWindow->getSurfId();
    EntityList &currentList = listOfLists[listIndex];
    if(currentList.contains(currentId)) {
        QMessageBox msg;
        msg.setText("Surface already in list");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
    else {
        currentList.append(currentId);
        currentList.setIndex(currentList.size() - 1);
        currentList.displayList();
    }
}

void NavigationWidget::deleteFromList() {
    if(listIndex == -1) return; //no list to remove from
    int currentId = parentSurfaceWindow->getSurfId();
    EntityList &currentList = listOfLists[listIndex];
    if(currentList.removeOne(currentId)) {
        if(currentList.getIndex() >= currentList.size()) {
            currentList.setIndex(currentList.size() - 1);
        }
    }
    else {
        QMessageBox msg;
        msg.setText("Surface is not in list");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
    return;
}
