#include "image_pane.h"
#include <QRubberBand>
#include <QMouseEvent>
#include <QRect>
#include <QImage>
#include <QPainter>
#include <QString>
#include <QChar>
#include <QFont>
#include <QDebug>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QDir>

ImagePane::ImagePane(QWidget* parent, SurfaceImgs* surface) //constructor
    : 	QLabel(parent), surf(surface), rubberBand(NULL)
{
    setAlignment(Qt::AlignTop | Qt::AlignLeft); 	//label contents aligned top left
    mode = SURFACE;
}

void ImagePane::newSurf()
{
    hide();
    QString imageFile = surf->getImageFile();
    imageFile.prepend(QDir::homePath() + "/ecdb/repository/text_imgs/");
qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAA";
//THIS IS REALLLLLY SLOW ON WINDOWS. WHY??
    originalImage = QImage(imageFile); // load image file from disk.
qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAA";
    currentBoxIndex=0; //in SURFACE mode, so this is the only possible value
    //remember that there may not actually be any boxes at all
    locked = true;
    if(originalImage.isNull())
    {
       mode = NOIMAGE;
       originalImage = QImage(":/no_image.png");
    }
    else
    {
      mode = SURFACE;
    }
    currentImage = originalImage;
    reset();
}

void ImagePane::reset()
{
    zoom = 1;
    rotation = 0;
    indexNumbersVisible = false;
    transformedImage = currentImage;
    const QRect res = QApplication::desktop()->availableGeometry();
    while(transformedImage.height() > res.height() ||
          transformedImage.width() > res.width())
    {
        zoomOut();
    }
    //in case no resizing took place, we still need to ensure that
    //private member transform is updated, so...
    transformImage();
    update();
    show();
}

ImagePane::Mode ImagePane::getMode() const
{
    return mode;
}

QString ImagePane::getModeName() const
{
    switch (mode)
    {
    case NOIMAGE:
       return "NO IMAGE";
    case SURFACE:
        return "SURFACE";
    case INSCRIPTION:
        return "INSCRIPTION";
    case GRAPH:
        return "GRAPH";
    }
    return "";
}

double ImagePane::getZoom() const
{
    return zoom;
}

double ImagePane::getRotation() const
{
    return rotation;
}

void ImagePane::getGraphImageList(const int imageIndex, QList<QImage>& imgList, const QSize size)
{
    //check to see whether imageIndex is oor
    if(imageIndex < 0 || imageIndex >= surf->inscriptionCount())
        return;

    //get surface image
    //since there is at least one inscription img,
    //there will certianly be a surface img
    QImage surfImg;
    rotateAndCrop(originalImage, surf, surfImg);

    //get corresponding inscription image
    QImage inscrImg;
    rotateAndCrop(surfImg, surf->ptrInscrAt(imageIndex), inscrImg);

    //for each graph
    for(int graphIndex = 0; graphIndex < surf->ptrInscrAt(imageIndex)->count(); graphIndex++)
    {
        //get graph image
        QImage graphImg;
        rotateAndCrop(inscrImg, &(surf->ptrInscrAt(imageIndex)->at(graphIndex)), graphImg);
        //fit to size keeping aspect ratio
        graphImg = graphImg.scaled(size, Qt::KeepAspectRatio);
        //append to list
        imgList.append(graphImg);
    }
}

//TODO replace with funciton in CangjieImg::
void ImagePane::rotateAndCrop(const QImage& startImg, const BoundingBox* box, QImage& endImg)
{
    QTransform tf;
    tf.rotate(box->getRotation());
    QRect selection = *box;
    endImg = (startImg.transformed(tf)) . copy(selection);
}

void ImagePane::modeDown()
{
    switch(mode)
    {
    case NOIMAGE:
       break;
    case GRAPH:
        break;
    case SURFACE:
        if(!surf->boxIsNull())
        {
            mode = INSCRIPTION;
            QTransform trans; //identity matrix
            trans.rotate(surf->getRotation()); //rotation of surface bounding box
            QRect selection = *surf; //rectangle of surface bounding box
            currentImage = (originalImage.transformed(trans)) . copy (selection);
            //set current image to be the rotated surface bounding box
            if(surf->inscriptionCount() > 0)
                currentBoxIndex = 0;
            else
                currentBoxIndex = -1;
            reset();
        }
        break;
	case INSCRIPTION:
        if(surf->inscriptionCount() > 0)
        {
            mode = GRAPH;
            currentInscrIndex = currentBoxIndex;
            QTransform trans;
            trans.rotate(surf->inscrAt(currentInscrIndex).getRotation());
            QRect selection = surf->inscrAt(currentInscrIndex);
            surfaceImage = currentImage;
            currentImage = surfaceImage.transformed(trans) . copy(selection);
            if(surf->ptrInscrAt(currentInscrIndex)->boxCount() > 0)
                currentBoxIndex = 0;
            else
                currentBoxIndex = -1;
            reset();
        }
        break;
    }
}

void ImagePane::modeUp()
{
    switch(mode)
    {
    case SURFACE:
        //do nothing
        break;
    case INSCRIPTION:  //from INSCRIPTION to SURFACE
        currentImage = originalImage; //retrieve image
        mode = SURFACE;
        currentBoxIndex = 0;
        reset();
        break;
    case GRAPH: //from GRAPH to INSCRIPTION
        currentBoxIndex = currentInscrIndex;
        currentImage = surfaceImage; //retrieve surface image
        mode = INSCRIPTION;
        reset();
        break;
    default:
       break;
    }
}

void ImagePane::toggleIndexNumbers()
{
    indexNumbersVisible = !indexNumbersVisible;
    update(); //to ensure that they become (in)visible immediately
}

void ImagePane::advanceCurrentBoxIndex()
{
    if(currentBoxIndex == -1) //there are no boxes
        return;
    switch(mode)
    {
    case NOIMAGE:
       break;
    case SURFACE:
        //do nothing
        break;
    case INSCRIPTION:
        currentBoxIndex++;	//advance index by one
        if(currentBoxIndex >= surf->inscriptionCount())
            currentBoxIndex = 0;	//wrap around if necessary
        update();
        break;
    case GRAPH:
        currentBoxIndex++;
        if(currentBoxIndex >= surf->ptrInscrAt(currentInscrIndex)->count())
            currentBoxIndex = 0;
        break;
    }
    update();
}

void ImagePane::reverseCurrentBoxIndex()
{
    if(currentBoxIndex == -1) //there are no boxes
        return;
    switch(mode)
    {
    case NOIMAGE:
       break;
    case SURFACE:
        //do nothing
        break;
    case INSCRIPTION:
        currentBoxIndex--; //go back one
        if(currentBoxIndex < 0)	//wrap around if necessary
            currentBoxIndex = surf->inscriptionCount() - 1;
        update(); //so that that current box is visible
        break;
    case GRAPH:
        currentBoxIndex--;
        if(currentBoxIndex < 0)
            currentBoxIndex = surf->ptrInscrAt(currentInscrIndex)->count() - 1;
        break;
    }
    update();
}

void ImagePane::deleteCurrentBox()
        //if an inscription or surface box is deleted
        //all boxes below it in the hierarchy (graphs, or graphs and inscriptions)
        //are deleted too
{
    if(locked || currentBoxIndex == -1)
        return;
    switch(mode)
    {
    case SURFACE:
        if(surf->inscriptionCount() == 0 || confirmDelete("inscription"))
        {
            *surf = BoundingBox(QPoint(0,0), QPoint(0,0), 0);
            surf->deleteAllInscriptions();
            emit inscrImgListModified(); //signal picked up by TranscriptionWindow
            currentBoxIndex = -1;
        }
        break;
	case INSCRIPTION:
        if(surf->ptrInscrAt(currentBoxIndex)->boxCount() == 0
           || confirmDelete("graph"))
        {
            surf->deleteInscr(currentBoxIndex);
            emit inscrImgListModified(); //signal picked up by TranscriptionWindow
            currentBoxIndex--;
            if(currentBoxIndex == -1)
                currentBoxIndex = surf->inscriptionCount() - 1;
        }
        break;
	case GRAPH:
        surf->ptrInscrAt(currentInscrIndex)->deleteBox(currentBoxIndex);
        currentBoxIndex--;
        if(currentBoxIndex == -1)
            currentBoxIndex = surf->ptrInscrAt(currentInscrIndex)->count() - 1;
        break;
    default:
       break;
    }
    update();
}

bool ImagePane::confirmDelete(QString type)
{
    QMessageBox msgBox(this);
    msgBox.setText(QString("This will delete all %1 images.").arg(type));
    msgBox.setInformativeText("OK to proceed?");
    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Warning);
    return (msgBox.exec() == QMessageBox::Ok);
}

void ImagePane::unlock()
{
   if(mode == NOIMAGE) return;
   locked = false;
}

void ImagePane::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    //draw the transformed version of the text-object image
    painter.drawImage(0, 0, transformedImage);

    //then we draw the bounding boxes
    //TODO inscriptions with at least one graph are a different color (?)

    //in case index numbers are visible, set font
    QFont font;
    font.setPixelSize(30/zoom); //TODO make font size dependent on resolution
    painter.setFont(font);
    QPen pen;
    pen.setWidth(0);
    pen.setColor(Qt::red);
    painter.setPen(pen);
    //make a list of bounding boxes according to current mode
    BoxList currentBoxList; //this is a list of all boxes
    currentBoxList.clear();
    switch(mode)
    {
    case SURFACE:
        currentBoxList.append(*surf); //list of one item, consting of the surface bounding box
        break;
    case INSCRIPTION:
        for(int i=0; i < surf->inscriptionCount(); i++)
            currentBoxList.insertBox(surf->inscrAt(i), i);
        break;
    case GRAPH:
        for(int i=0; i < surf->ptrInscrAt(currentInscrIndex)->count(); i++)
            currentBoxList.insertBox(surf->ptrInscrAt(currentInscrIndex)->at(i), i);
        break;
    default:
       break;
    }
    //iterate through the list of bounding boxes
    for (int i=0; i<currentBoxList.size(); i++)
    {
        BoundingBox currentBox = currentBoxList.at(i);

        //the bounding boxes need to be rotated and scaled
        QTransform boxTransform; //identity matrix
        //first we need to handle the bounding box's own rotation
        //by inverting the true matrix that was applied to the image
        //at the time each bounding box was created
        //(note: we don't need to worry about scale, as we took account of that
        //when the bounding box was created)
        boxTransform.rotate(currentBox.getRotation());
        boxTransform = QImage::trueMatrix(boxTransform,
                                          currentImage.width(), currentImage.height());
        boxTransform = boxTransform.inverted();
	
        //then we compound the above matrix with the current transformation of the image
        QTransform imageTrueTransform = QImage::trueMatrix(transform,
                                                           currentImage.width(), currentImage.height());
        painter.setWorldTransform(boxTransform * imageTrueTransform);
        //now draw the box
        //pen color is red; set the pen-color to green if this is the current box.
        if(i==currentBoxIndex)
        {
            QPen pen;
            pen.setWidth(0);
            pen.setColor(Qt::green);
            painter.setPen(pen);
        }
        painter.drawRect(currentBox);
        //and add an (optional) index number
        if(indexNumbersVisible)
        {
            painter.drawText(currentBox.left(), currentBox.top(), 50/zoom, 50/zoom,
                             Qt::AlignBottom,  QString("%1").arg(i+1)); //visible index, so base = 1, not zero
        }
        //return pen color to red (might be green)
        QPen pen;
        pen.setWidth(0);
        pen.setColor(Qt::red);
        painter.setPen(Qt::red);
    }

    //if label is not resized, it will stay large on zoom out
    //resulting in misleading / redundant scroll bars
    resize(transformedImage.size()); // keep label same size as image
}

void ImagePane::transformImage()
{
    transform.reset(); //set to identity matrix
    transform.rotate(rotation);	//set matrix
    transform.scale(zoom, zoom);
    transformedImage = currentImage.transformed(transform);		//make rotated version of image
}	

//mouse down, start rubberband
void ImagePane::mousePressEvent(QMouseEvent* event)
{
    origin = event->pos();
    if(!rubberBand)
    {
        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        //attempting to set the palette of the QRubberBand
        //for colors etc. does not have any affect on apearance.
    }
    rubberBand->setGeometry(QRect(origin, QSize())); 	//rubberband as a QRect
    //of zero size at origin
    rubberBand->show();
}

//dragging rubberband
void ImagePane::mouseMoveEvent(QMouseEvent* event)
{
    rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
    //normalized, in case dragging up or left
}

//mouse up, end rubberband
void ImagePane::mouseReleaseEvent(QMouseEvent* event)
        //TODO after adding new item, TranscriptionWindow::reset() must be called
	//use signal and slot arrangement.
{
    rubberBand->hide();
    //get bBox, accounting for rotation and zoom
    QRect rect(origin/zoom, event->pos()/zoom);
    rect = rect.normalized(); //topLeft etc. are not reliable without this
    //if it was a tiny drag, it was probably an accident
    if(rect.height()<3 || rect.width()<3 || locked)
    {
        update(); //to remove the mess
        return;
    }
    BoundingBox box(rect.topLeft(), rect.bottomRight(), rotation/*, false*/);
    //append the box to the appropriate list, according to mode
    switch(mode)
    {
    case SURFACE:
        if(surf->inscriptionCount() == 0 || confirmDelete("inscription"))
        {
            surf->deleteAllInscriptions();
            surf->setBox(rect.topLeft(), rect.bottomRight(), rotation/*, false*/);
            currentBoxIndex = 0;
            emit inscrImgListModified(); //picked up by TranscriptionWindow
            //TODO FIX THIS!!
        }
        break;
    case INSCRIPTION:
        surf->insertInscr(box, ++currentBoxIndex);
        emit inscrImgListModified(); //picked up by TranscriptionWindow
        break;
    case GRAPH:
        surf->ptrInscrAt(currentInscrIndex)->insertBox(box, ++currentBoxIndex);
        break;
    default:
       break;
    }
    update();
}

void ImagePane::zoomIn()
{
    if(zoom<5.0)
    {
        zoom *= 4.0/3.0; //enlarge displayed image by 1+1/3
        transformImage();
        update();
    }
}

void ImagePane::zoomOut()
{
    if(zoom>0.1)
    {
        zoom*=0.75; //shrink displayed image to 3/4
        transformImage();
        update();
    }
}

void ImagePane::zoomRestore()
{
    zoom=1.0; //reset displayed image scale
    transformImage();
    update();
}

void ImagePane::rotateClockwise()
{
    rotation+=5; // +5 degrees
    if(rotation>=360)			//keep 0 <= rotation < 360
        rotation -= 360;
    transformImage();
    update();
}

void ImagePane::rotateAntiClockwise()
{
    rotation-=5; // -5 degrees
    if(rotation < 0)		//keep 0 <= rotation < 360
        rotation += 360;
    transformImage();
    update();
}

void ImagePane::rotateRestore()
{
    rotation=0;
    transformImage();
    update();
}

void ImagePane::raiseBoxIndex()
{
    if(locked)
        return;
    switch(mode)
    {
    case SURFACE:
        //do nothing
        break;
    case INSCRIPTION:
        if(currentBoxIndex >= 0 &&
           currentBoxIndex < surf->inscriptionCount() - 1) //not last in list
        {
            surf->swap(currentBoxIndex, currentBoxIndex + 1);
            currentBoxIndex++;
            emit inscrImgListModified(); //signal picked up by TranscriptionWindow
        }
        break;
    case GRAPH:
        if(currentBoxIndex >= 0 &&
           currentBoxIndex < surf->ptrInscrAt(currentInscrIndex)->count() - 1) //not last in list
        {
            surf->ptrInscrAt(currentInscrIndex)->swap(currentBoxIndex,
                                                            currentBoxIndex + 1);
            currentBoxIndex++;
            emit inscrImgListModified(); //signal picked up by TranscriptionWindow
        }
        break;
    default:
       break;
    }
    update();
}

void ImagePane::lowerBoxIndex()
{
    if(locked)
        return;
    switch(mode)
    {
    case SURFACE:
        //do nothing
        break;
    case INSCRIPTION:
        if(currentBoxIndex >= 1 &&
           currentBoxIndex < surf->inscriptionCount()) //not first in list
        {
            surf->swap(currentBoxIndex, currentBoxIndex - 1);
            currentBoxIndex--;
            emit inscrImgListModified(); //signal picked up by TranscriptionWindow
        }
        break;
    case GRAPH:
        if(currentBoxIndex >=1 &&
           currentBoxIndex < surf->ptrInscrAt(currentInscrIndex)->count()) //not first in list
        {
            surf->ptrInscrAt(currentInscrIndex)->swap(currentBoxIndex,
                                                            currentBoxIndex - 1);
            currentBoxIndex--;
            emit inscrImgListModified(); //signal picked up by TranscriptionWindow
        }
        break;
    default:
       break;
    }
    update();
}

/*
void ImageLabel::saveThumbnails()
{
	//iterate through list of bBoxes
	for (int i=0; i < pBoxList->size(); i++)
	{
		BoundingBox currentBox = pBoxList->at(i);
		//rotate copy of image by the appropriate amount
		QTransform trans; //identity matrix
		trans.rotate(pBoxList->at(i).getRotation());
		QRect selection = pBoxList->at(i);
		QImage imageCopy = ( image->transformed(trans) ) . copy(selection);
		//make zero padded file-name
		QString fileName = QString("img_%1.jpg").arg(i, 3, 10, QChar('0'));
		//save selection to disk
		imageCopy.save(fileName);
	}
}
*/
