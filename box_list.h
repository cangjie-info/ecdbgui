#ifndef BOX_LIST_H
#define BOX_LIST_H

//class to represent a list of bounding boxes
//boxes can be for graphs, inscriptions or surfaces
//base class of Inscription and Surface classes
//this should probably have been has-a not is-a, but leave as is

#include "bounding_box.h"
#include <QList>

class BoxList : public QList<BoundingBox>
{
public:
	BoxList(); //constructor
	void insertBox(BoundingBox newBox, int index); //inserts box at index
		//list unchanged if out of range
	void deleteBox(int index); //deletes box at index
		//list unchanged if out of range
	BoundingBox boxAt(int index) const; //returns box at index
		//returns zero box if index oor.
	int boxCount() const; //number of boxes in list
	void report() const;
};

#endif
