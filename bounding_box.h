//class for bounding box around inscription or glyph in image

#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <QRect>

class BoundingBox : public QRect
{
public:
	BoundingBox(); //constructor
   BoundingBox(QPoint point1, QPoint point2, int angle); //constructor
   void setBox(QPoint point1, QPoint point2, int angle);
      //set all member variables of box
	int getRotation() const;		//returns rotation
	bool boxIsNull() const; //true if zero box
	void report() const;
private:
	int rotation;		//rotation of image when rectangle was set
							//center of rotation depends on image !!
};
	
#endif
