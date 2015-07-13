#include "db_handler.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QVariant>
#include "cangjie_img.h"

DbHandler::DbHandler()
    : pCorpusQuery(NULL)
{
}

bool DbHandler::connect()
//TODO allow connection to remote db as an alternative
//TODO connect to ecdb instead of ec - this is a big change, requiring new names for all tables and fields.
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("ecdb");
    db.setUserName("ecdbgui");
    db.setPassword("abc123");
    if(!db.open())
    {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              db.lastError().text());
        return false;
    }
    return true;
}

//TODO change to selecting a single surface by id number.
/* DELETE
bool DbHandler::setSurf()
{
    pCorpusQuery = new QSqlQuery();
    pCorpusQuery->exec("SELECT id FROM inscr_surfs WHERE pub_id=113 ORDER BY name;");
    if(pCorpusQuery->size() > 0)
    {
        qDebug() << pCorpusQuery->size() << "rows in the query result";
        return true;
    }
    else
        return false;
}

*/

//TODO get rid of this, and do a nextInPublication method, et seq.
int DbHandler::getNextSurfIdInPub(int currentId)
{
   //get id of next item in the same publication
   QString queryString = QString("SELECT id FROM inscr_surfs WHERE pub_id="
                       "(SELECT pub_id FROM inscr_surfs WHERE id=%1)"
                       " AND name > (SELECT name FROM inscr_surfs WHERE id=%1)"
                       " ORDER BY name ASC"
                       " LIMIT 1;").arg(currentId);
   QSqlQuery query(queryString);
   if(query.next())
   {
      return query.value(0).toInt();
   }
   else
   {
      return currentId;
   }
}

int DbHandler::getNext10SurfIdInPub(int currentId)
{
   //get id of item 10 forward in the same publication
   QString queryString = QString("SELECT id FROM inscr_surfs WHERE pub_id="
                       "(SELECT pub_id FROM inscr_surfs WHERE id=%1)"
                       " AND name > (SELECT name FROM inscr_surfs WHERE id=%1)"
                       " ORDER BY name ASC"
                       " LIMIT 9, 1;").arg(currentId);
   QSqlQuery query(queryString);
   if(query.next())
   {
      return query.value(0).toInt();
   }
   else
   {
      return currentId;
   }
}

int DbHandler::getNext100SurfIdInPub(int currentId)
{
   //get id of item 100 forward in the same publication
   QString queryString = QString("SELECT id FROM inscr_surfs WHERE pub_id="
                       "(SELECT pub_id FROM inscr_surfs WHERE id=%1)"
                       " AND name > (SELECT name FROM inscr_surfs WHERE id=%1)"
                       " ORDER BY name ASC"
                       " LIMIT 99, 1;").arg(currentId);
   QSqlQuery query(queryString);
   if(query.next())
   {
      return query.value(0).toInt();
   }
   else
   {
      return currentId;
   }
}

int DbHandler::getPreviousSurfIdInPub(int currentId)
{   
   //get id of previous item in the same publication
   QString queryString = QString("SELECT id FROM inscr_surfs WHERE pub_id="
                       "(SELECT pub_id FROM inscr_surfs WHERE id=%1)"
                       " AND name < (SELECT name FROM inscr_surfs WHERE id=%1)"
                       " ORDER BY name DESC"
                       " LIMIT 1;").arg(currentId);
   QSqlQuery query(queryString);
   if(query.next())
   {
      return query.value(0).toInt();
   }
   else
   {
      return currentId;
   }
}

int DbHandler::getPrevious10SurfIdInPub(int currentId)
{
   //get id of item 10 back in the same publication
   QString queryString = QString("SELECT id FROM inscr_surfs WHERE pub_id="
                       "(SELECT pub_id FROM inscr_surfs WHERE id=%1)"
                       " AND name < (SELECT name FROM inscr_surfs WHERE id=%1)"
                       " ORDER BY name DESC"
                       " LIMIT 9, 1;").arg(currentId);
   QSqlQuery query(queryString);
   if(query.next())
   {
      return query.value(0).toInt();
   }
   else
   {
      return currentId;
   }
}

int DbHandler::getPrevious100SurfIdInPub(int currentId)
{
   //get id of item 100 back in the same publication
   QString queryString = QString("SELECT id FROM inscr_surfs WHERE pub_id="
                       "(SELECT pub_id FROM inscr_surfs WHERE id=%1)"
                       " AND name < (SELECT name FROM inscr_surfs WHERE id=%1)"
                       " ORDER BY name DESC"
                       " LIMIT 99, 1;").arg(currentId);
   QSqlQuery query(queryString);
   if(query.next())
   {
      return query.value(0).toInt();
   }
   else
   {
      return currentId;
   }
}
void DbHandler::moveToSurf(int index)
{
    if(!pCorpusQuery->seek(index))
        pCorpusQuery->first();
}

//TODO delete
int DbHandler::getCorpusSize() const
{
    if(pCorpusQuery)
        return pCorpusQuery->size();
    else
        return 0; //if corpus has not been set
}

//TODO delete. Instead just display the corpus name and number
int DbHandler::getPositionInCorpus() const
{
    if(pCorpusQuery)
        return pCorpusQuery->at(); //zero for first record
    //Qt DOC: <<If the position is invalid, the function returns QSql::BeforeFirstRow
    //or QSql::AfterLastRow, which are special negative values.>>
    else
        return -1; //if corpus has not been set
}

//TODO The id number will be a private member. Just return it.
int DbHandler::getCurrentSurfaceId() const //returns ec.surfaces.id
{
    return pCorpusQuery->value(0).toInt();
}

void DbHandler::readSurface(int surfId, SurfaceImgs& surf,
                            SurfaceTranscription& trans, QString font) const
{
    //clear both lists of inscriptions (which deletes graphs too)
    surf.deleteAllInscriptions();
    trans.clear();

    //Query ecdb.inscr_surfs
//***************
//TODO - publicationId changes from string to int.
// need JOIN with publicaitons table
//***************
    QString surfaceQueryString = QString(
            "SELECT img_file, x1, y1, x2, y2, rotation, pub_id, inscr_surfs.name, "
            "surf_type_id, pubs.name FROM inscr_surfs "
            "INNER JOIN pubs ON pub_id=pubs.id "
            "WHERE inscr_surfs.id=%1;").arg(surfId);

qDebug() << surfaceQueryString;
    QSqlQuery surfaceQuery(surfaceQueryString);
    surfaceQuery.next(); //move first (and last, we hope)
    //get imageFile
    QString imgDirFile = surfaceQuery.value(9).toString().toLower()
          + "/" + surfaceQuery.value(0).toString();
    surf.setImageFile(imgDirFile);
qDebug() << surfaceQuery.value(0).toString();
qDebug() << surfaceQuery.value(9).toString();
    //get surface box
    //if x1 is NULL, then surface box is zero
    if(surfaceQuery.isNull(1))
    {
        surf.setBox(QPoint(0,0), QPoint(0,0), 0);
    }
    else
    {
        surf.setBox(	QPoint(surfaceQuery.value(1).toInt(), surfaceQuery.value(2).toInt()),
                        QPoint(surfaceQuery.value(3).toInt(), surfaceQuery.value(4).toInt()),
                        surfaceQuery.value(5).toInt());
    }
    //for trans, get last three fields in query
    trans.setPubId(surfaceQuery.value(6).toString());
    trans.setPubNumber(surfaceQuery.value(7).toString());
    trans.setSurfaceType(surfaceQuery.value(8).toString());
    trans.setPubName(surfaceQuery.value(9).toString());

    //query ecdb.inscrs
    QString inscriptionQueryString = QString(
             "SELECT id, x1, y1, x2, y2, rotation "
             "FROM inscrs WHERE inscr_surf_id=%1").arg(surfId);
    inscriptionQueryString += " ORDER BY id DESC;"; //DESC allows insertion at index=0

    QSqlQuery inscriptionQuery(inscriptionQueryString);
qDebug() << inscriptionQueryString;
    while(inscriptionQuery.next())
    {
        //add inscription to trans, no matter what
        trans.prepend(InscriptionTranscription());
        if(!inscriptionQuery.isNull(1)) //add inscription bbox to list if not null
        {
            surf.insertInscr(
                    BoundingBox(
                            QPoint(inscriptionQuery.value(1).toInt(), inscriptionQuery.value(2).toInt()),
                            QPoint(inscriptionQuery.value(3).toInt(), inscriptionQuery.value(4).toInt()),
                            inscriptionQuery.value(5).toInt()),
                    0); //inserting at the beginning of the list
            trans[0].setCanHaveImage(true);
        }
        else //if bbox is null, set insription trans canHaveImage to false
            trans[0].setCanHaveImage(false);

        //for each inscription, query graphs.
        QString currentInscrId = inscriptionQuery.value(0).toString();
qDebug() << currentInscrId;
        QString graphQueryString("SELECT x1, y1, x2, y2, rotation, markup, graph_id, ");
        graphQueryString += font;
        graphQueryString += " FROM inscr_graphs LEFT JOIN graphs ON graph_id=graphs.id "
                "WHERE inscr_id=";
        graphQueryString += currentInscrId;
        graphQueryString += " ORDER BY number DESC;"; //DESC allows insertion at index=0
qDebug() << graphQueryString;
        QSqlQuery graphQuery(graphQueryString);
        while(graphQuery.next())
        {
            //add graph transcription to trans[0]
            GraphTranscription newGraph(
                    graphQuery.value(5).toInt(), 	//markup
                    graphQuery.value(6).toInt()); 	//grapheme
            newGraph.setGlyph(graphQuery.value(7).toString());
qDebug() << graphQuery.value(6).toInt();
            trans[0].prepend(newGraph);
            //add graph bbox to inscription
            if(!graphQuery.isNull(0)) //if non-null graph box
            {
                surf.ptrInscrAt(0)->insertBox(
                        BoundingBox(
                                QPoint(graphQuery.value(0).toInt(), graphQuery.value(1).toInt()),
                                QPoint(graphQuery.value(2).toInt(), graphQuery.value(3).toInt()),
                                graphQuery.value(4).toInt()),
                        0); 	//prepend (i.e. insert at index 0) since we used DESC
                trans[0][0].setCanHaveImage(true);
            }
            else //null graph box, so set canHaveImage for transcription to false
                trans[0][0].setCanHaveImage(false);
        }
    }
}

void DbHandler::writeSurface(int surfId, SurfaceImgs& imgs, SurfaceTranscription& trans)
{
db.transaction(); //begin transaction

    //*** SURFACE ***//

    //UPDATE bbox in ec.surface
    QString surfaceUpdateString = QString(
            "UPDATE inscr_surfs SET x1=%1, y1=%2, x2=%3, y2=%4, rotation=%5 WHERE id=%6;")
            .arg(imgs.x())  //x1
            .arg(imgs.y())  //y1
            .arg(imgs.x()+imgs.width())     //x2
            .arg(imgs.y()+imgs.height())    //y2
            .arg(imgs.getRotation())        //rotation
            .arg(surfId); //id
    QSqlQuery surfaceUpdateQuery;
    if(!surfaceUpdateQuery.exec(surfaceUpdateString))
    {
  db.rollback(); //TODO handle as error!!
qDebug() << "ROLLBACK!!";
qDebug() << surfaceUpdateString;

        return;
    }

    //*** DELETE OLD INSCRIPTIONS AND GRAPHS ***//

    //DELETE query for corresponding inscriptions and graphs
            //inscr_graphs is set to cascade delete when corresponding inscrs are deleted.
    QString inscriptionDeleteString = QString(
            "DELETE inscrs "
            "FROM inscrs "
            "WHERE inscrs.inscr_surf_id=%1;")
            .arg(surfId); //surface id
    QSqlQuery inscriptionDeleteQuery;
    if(!inscriptionDeleteQuery.exec(inscriptionDeleteString))
    {
        qDebug() << inscriptionDeleteQuery.lastError().text();
    db.rollback(); //TODO handle as error - return false? or use popup dialog?
 qDebug() << "ROLLBACK!!";
 qDebug() << inscriptionDeleteString;
        return;
    }

    //*** INSERT NEW INSCRIPTIONS AND GRAPHS ***//

    //for each inscription in trans, INSERT inscription,
    //then INSERT inscription graphs
    int imgsIndex = -1; //increment on encountering each image
    for(int transIndex = 0; transIndex < trans.count(); transIndex++)
    {

        //*** NEW INSCRIPTION ***//

        QString insertInscriptionString = QString(
                "INSERT INTO inscrs "
                "SET number=%1, "
                    "inscr_surf_id=%2")
                .arg(transIndex + 1) //1-based index in ec, not zero based
                .arg(surfId);
        bool inscrHasImage = false;
        if(imgs.inscriptionCount() > 0
           && imgsIndex < imgs.inscriptionCount()-1
            && trans[transIndex].getCanHaveImage())
            //inscription has an img
        {
            inscrHasImage = true;
            imgsIndex++;
            int x1 = imgs.ptrInscrAt(imgsIndex)->x();
            int y1 = imgs.ptrInscrAt(imgsIndex)->y();
            int x2 = x1 + imgs.ptrInscrAt(imgsIndex)->width();
            int y2 = y1 + imgs.ptrInscrAt(imgsIndex)->height();
            int rotation = imgs.ptrInscrAt(imgsIndex)->getRotation();

            insertInscriptionString += QString(
                    ", x1=%1, y1=%2, x2=%3, y2=%4, rotation=%5")
                    .arg(x1).arg(y1).arg(x2).arg(y2).arg(rotation);
        }
        else
        {
            //nothing to do - db will provide default NULL values
            //TODO check this.
        }
        insertInscriptionString += ";";
        QSqlQuery insertInscriptionQuery;
        if(!insertInscriptionQuery.exec(insertInscriptionString))
        {
            db.rollback(); //TODO handle as error
qDebug() << "ROLLBACK!!";
qDebug() << insertInscriptionString;
            return;
        }

        //*** NEW GRAPHS FOR LAST INSCRIRPTION ***//

        //INSERT graphs for the inscription just inserted if there are any
        //number of rows = maximum of trans[transIndex].count() and imgs.ptrInscAt(imgsIndex)->count()
        int numberOfGraphs;
        if(inscrHasImage)
        {
            numberOfGraphs = (trans[transIndex].count() > imgs.ptrInscrAt(imgsIndex)->boxCount()
                            ? trans[transIndex].count() : imgs.ptrInscrAt(imgsIndex)->boxCount());
        }
        else //no image, only transcription or nothing
        {
            numberOfGraphs = trans[transIndex].count();
        }
        if(numberOfGraphs == 0)
        {
            //no need for INSERT query - do nothing
        }
        else
        {
            int inscriptionId = insertInscriptionQuery.lastInsertId().toInt();
            int graphImgsIndex = 0;

            QString insertGraphsString = QString(
                    "INSERT INTO inscr_graphs "
                    "(inscr_id, number, x1, y1, x2, y2, rotation, markup, graph_id) "
                    "VALUES ");             //in format (1, 2, 3, 4, 5, 6, 7, 8, 9), ...(...);
            for (int graphIndex = 0; graphIndex < numberOfGraphs; graphIndex++)
            {
                QString x1, y1, x2, y2, rotation;
                int markup, graphemeId;
                //if imgs are left && (no trans left || canHaveImage) => row has graph image
                if(inscrHasImage
                    && graphImgsIndex < imgs.ptrInscrAt(imgsIndex)->boxCount() //imgs are left
                    && (
                        graphIndex >= trans[transIndex].count() //no trans left
                        || trans[transIndex][graphIndex].getCanHaveImage() == true) ) //can have image
                    //the last term in the boolean is only evaluated if there are trans left
                { //get bbox and add to query, increment graphsImgsIndex
                    BoundingBox graphBox = imgs.ptrInscrAt(imgsIndex)->at(graphImgsIndex);
                    x1.setNum( graphBox.x() );
                    y1.setNum( graphBox.y() );
                    x2.setNum( graphBox.x() + graphBox.width() );
                    y2.setNum( graphBox.y() + graphBox.height() );
                    rotation.setNum( graphBox.getRotation() );
                    graphImgsIndex++;
                }
                else //NULL bbox
                {
                    x1 = "NULL";
                    y1 = "NULL";
                    x2 = "NULL";
                    y2 = "NULL";
                    rotation = "NULL";
                }
                //if trans left
                if(graphIndex < trans[transIndex].count())
                {
                    //get markup and graphemeId and add to query
                    markup = trans[transIndex][graphIndex].getMarkup();
                    graphemeId = trans[transIndex][graphIndex].getGrapheme();
                }
                else // no more trans graphs
                {
                    markup = 0;
                    graphemeId = 0;
                }
                QString values = QString("(%1, %2, %3, %4, %5, %6, %7, %8, %9)")
                                 .arg(inscriptionId)
                                 .arg(graphIndex + 1) //one-based, not zero-based in ec
                                 .arg(x1).arg(y1).arg(x2).arg(y2)
                                 .arg(rotation)
                                 .arg(markup)
                                 .arg(graphemeId);
                if(graphIndex != 0)
                    values.prepend(", ");
                insertGraphsString += values;
            }
            insertGraphsString += ";"; //query string finished
            QSqlQuery insertGraphsQuery;
            if(!insertGraphsQuery.exec(insertGraphsString))
            {
qDebug() << "ROLLBACK!!";
qDebug() << insertGraphsString;
                db.rollback(); //TODO handle as error.
                return;
            }
        }
    }
db.commit(); //done - yeay!
}

void DbHandler::getGrapheme(QString searchString, int &grapheme, QString &glyph) //static
{
    //TODO - search using things other than name field.
    //maybe write alternate function that
    QSqlQuery signListQuery;
    QString queryString = QString("SELECT id, ics3_glyph "
                                  "FROM graphs WHERE name=\"%1\";")
                          .arg(searchString);
    signListQuery.exec(queryString);
    if(signListQuery.first())
    {
        grapheme = signListQuery.value(0).toInt();
        glyph = signListQuery.value(1).toString();
    }
    else grapheme = 0;
    return;
}
//tries to find grapheme in ec.signList corresponding to searchString
//for now, only searches name field
//returns grapheme id

void DbHandler::findGraphemeInstances(QList<int> &graphIdList, const int grapheme)
{
    QString queryString = QString("SELECT id FROM inscriptionGraphs WHERE graphemeId=%1;")
                          .arg(grapheme);

    QSqlQuery query;
    query.exec(queryString);
    while(query.next())
    {
        graphIdList.append(query.value(0).toInt());
    }
}

void DbHandler::getGraphImage(QImage& graphImage, const int graphId, const int size)
{
    //query graphId (with inner joins to inscriptions and surfaces)
    QString queryString = QString("SELECT surfaces.imageFile, "   //0
                                  "surfaces.x1, surfaces.y1, "  //1, 2
                                  "surfaces.x2, surfaces.y2, "  //3, 4
                                  "surfaces.rotation, "         //5
                                  "inscriptions.x1, inscriptions.y1, " //6, 7
                                  "inscriptions.x2, inscriptions.y2, " //8, 9
                                  "inscriptions.rotation, "     //10
                                  "inscriptionGraphs.x1, inscriptionGraphs.y1, " //11, 12
                                  "inscriptionGraphs.x2, inscriptionGraphs.y2, " //13, 14
                                  "inscriptionGraphs.rotation " //15
                                  "FROM inscriptionGraphs "
                                  "INNER JOIN inscriptions "
                                  "INNER JOIN surfaces "
                                  "ON inscriptionGraphs.inscriptionId=inscriptions.id "
                                  "AND inscriptions.surfaceId=surfaces.id "
                                  "WHERE inscriptionGraphs.id=%1;"
                                  ).arg(graphId);
    QSqlQuery query;
    qDebug() << queryString;
    query.exec(queryString);
    //if null image return black image
    if(!query.next() || query.value(11).isNull())
    {
        graphImage = QImage();
        return;
    }
    else
    {
        //get image file
        QString fileName = query.value(0).toString();
        fileName.prepend("/home/ads/ecdb/repository/text_imgs/"); //TODO make platform independent
        QImage image = QImage(fileName);
        QImage surfaceImage;
    //make surface image
        CangjieImg::rotateAndCrop(image,
            BoundingBox(QPoint(query.value(1).toInt(), query.value(2).toInt()),
                        QPoint(query.value(3).toInt(), query.value(4).toInt()),
                        query.value(5).toInt()),
            surfaceImage);
    //make inscription image
        QImage inscriptionImage;
        CangjieImg::rotateAndCrop(surfaceImage,
            BoundingBox(QPoint(query.value(6).toInt(), query.value(7).toInt()),
                        QPoint(query.value(8).toInt(), query.value(9).toInt()),
                        query.value(10).toInt()),
            inscriptionImage);
    //make graph image
        CangjieImg::rotateAndCrop(inscriptionImage,
            BoundingBox(QPoint(query.value(11).toInt(), query.value(12).toInt()),
                        QPoint(query.value(13).toInt(), query.value(14).toInt()),
                        query.value(15).toInt()),
            graphImage);
    //adjust to size
        graphImage = graphImage.scaled(QSize(size, size), Qt::KeepAspectRatio);
    }
}

void DbHandler::fixCracks()
{
    //select all graphs "." or numerals
    QString queryString = "SELECT id, inscriptionId, graphNumber, graphemeId "
                          "FROM inscriptionGraphs "
                          "WHERE (graphemeId>=718 "
                          "AND graphemeId<=727) "
                          "OR graphemeId=1001 "
                          "ORDER BY inscriptionId, graphNumber;";
    QSqlQuery query;
    query.exec(queryString);
    int previousInscriptionId = 0;
    int previousGraphNumber = 0;
    int previousGraphemeId = 0;

    int currentGraphId = 0;
    int currentInscriptionId = 0;
    int currentGraphNumber = 0;
    int currentGraphemeId = 0;
	 int adjustment = 0;
    while(query.next())
    {
        currentGraphId = query.value(0).toInt();
        currentInscriptionId = query.value(1).toInt();
        currentGraphNumber = query.value(2).toInt();
        currentGraphemeId = query.value(3).toInt();
        if(previousInscriptionId == currentInscriptionId
           && previousGraphNumber + 1 == currentGraphNumber
           && previousGraphemeId == 1001)
        {
					adjustment++;
            QString updateQueryString = QString("UPDATE inscriptionGraphs "
                                        "SET markup = markup | %1 , graphNumber = %2 "
                                        "WHERE id = %3;")
                    .arg(GraphTranscription::CRACK_NUMBER)
                    .arg(previousGraphNumber - adjustment)
                    .arg(currentGraphId);
            QSqlQuery updateQuery;
            updateQuery.exec(updateQueryString);

            qDebug() << currentGraphId;
        }
		  if(previousInscriptionId != currentInscriptionId)
		  	adjustment = 0;
        previousInscriptionId = currentInscriptionId;
        previousGraphNumber = currentGraphNumber;
        previousGraphemeId = currentGraphemeId;
    }

    QString deleteQueryString = "DELETE inscriptionGraphs "
                        "WHERE graphemeId = 1001;";
    QSqlQuery deleteQuery;
    deleteQuery.exec(deleteQueryString);
}
