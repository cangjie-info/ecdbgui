#include <QDebug>
#include "box_list.h"

BoxList::BoxList()
{
	//constructor
}

void BoxList::insertBox(BoundingBox newBox, int index)
{
	if(index < 0 || index > count())
	{
		qDebug() << "index out of range in BoxList::insertBox";
		return;
	}
	insert(index, newBox);
}

void BoxList::deleteBox(int index)
{
	if(index < 0 || index >= count()) //this also guarantees that the boxList is not empty
	{
		qDebug() << "list empty or index out of range in BoxList::deleteBox";
		return;
	}
	removeAt(index);
}


BoundingBox BoxList::boxAt(int index) const
{
	if(index < 0 || index >= count())
	{
		qDebug() << "list empty or index oor in BoxList::boxAt";
      return BoundingBox(QPoint(0,0), QPoint(0,0), 0);
	}
	return at(index); //returns value, not modifiable reference
}

int BoxList::boxCount() const
{
	return count();
}

void BoxList::report() const
{
	qDebug() << "*** REPORT FOR BoxList ***";
	qDebug() << "BoxList contains" << count() << "boxes: ";
	for(int i = 0; i < count(); i++)
		operator[](i).report();
	qDebug() << "*** END REPORT FOR BoxList ***";
}
