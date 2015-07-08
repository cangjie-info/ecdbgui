#include <QDebug>
#include "bounding_box.h"

BoundingBox::BoundingBox()
{
	//constructor
}

BoundingBox::BoundingBox(QPoint point1, QPoint point2, int angle)
   : QRect(point1, point2), rotation(angle)
{
	//constructor
}

void BoundingBox::setBox(QPoint point1, QPoint point2, int angle)
{
	setTopLeft(point1);
	setBottomRight(point2);
	rotation = angle;
}

int BoundingBox::getRotation() const
{
	return rotation;
}

bool BoundingBox::boxIsNull() const
{
   if(height()==0 || width()==0)	//yuk
		return true;
	else
		return false;
}


void BoundingBox::report() const
{
	qDebug() << "REPORT FOR BoundingBox";
	qDebug() << "x1 = " << x() << ", y1 = " << y();
	qDebug() << "x2 = " << x()+width()-1 << ", y2 = " << y()+height()-1;
	qDebug() << "rotation = " << getRotation();
}
