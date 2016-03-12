#ifndef ENTITY_LIST_H
#define ENTITY_LIST_H
#include <QList>
#include <QString>

enum EntityType {surface, graph, inscr_graph, inscr};

class EntityList : public QList<int>
{
public:
   EntityList();
   EntityList(EntityType entity_type, QString name);
   bool isDirty() {return dirty;}
   void setDirty(bool state) {dirty = state;}
   void moveLast();
   void moveFirst();
   void forward();
   void setIndex(int newIndex);
   int getIndex() {return index;}
   void back();
   void displayList();
   void saveList() {}
   bool openList() {return false;} //return false on failure
   void setName(QString newName);
   QString getName() {return listName;}
   int current(); //returns current entity, 0 if list if empty
private:
   int index; //current position in list
   EntityType type;
   QString listName;
   bool dirty; //set to false on save, set to true on append or delete
   };

#endif // ENTITY_LIST_H
