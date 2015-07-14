#ifndef DB_HANDLER_H
#define DB_HANDLER_H

#include <QSqlQuery>
#include <qimage.h>
#include "surface_imgs.h"
#include "surface_transcription.h"

//TODO - This should be a namspace of functions, not a class.
//TODO the variable db will be the only necessary member variable, and it
//can be accessed as the globally accessible default database.

//TODO: new methods: mergeGraphs, assign lingVal to inscr_graph, insert graph, search for graph strings
//TODO: new methods: find and replace on graph strings.


//class to handle ALL interaction with the db
//AIM: no other class should have any QSqlQuery variables

class DbHandler
{
public:
    DbHandler();  //constructor
    bool connect();
    //connects to ec db as user "qt".
    //returns true if connection succesful, false + error message box otherwise
//DELETE    bool setSurf();
    int getNextSurfIdInPub(int currentId);
    //returns the id of the next surf in pub
    //if currentId is last surf in pub, returns currentId
    int getNext10SurfIdInPub(int currentId);
    //skips 10 forward in pub and returns id
    int getNext100SurfIdInPub(int currentId);
    int getPreviousSurfIdInPub(int currentId);
    int getPrevious10SurfIdInPub(int currentId);
    int getPrevious100SurfIdInPub(int currentId);

    void moveToSurf(int index); //moves to surface at index
    int getCorpusSize() const;
    //returns number of surfaces in corpus
    int getPositionInCorpus() const;
    //returns ordinal position of current surface in corpus
    int getCurrentSurfaceId() const; //returns ec.surfaces.id
    void readSurface(int surfId, SurfaceImgs& surf, SurfaceTranscription& trans, QString font) const; //gets currentSurface via DbHandler
    static void getGrapheme(QString searchString, int& grapheme, QString& glyph);
    //tries to find grapheme in ec.signList corresponding to searchString
    //for now, only searches name field
    //returns grapheme id
    void writeSurface(int surfId, SurfaceImgs& surf, SurfaceTranscription& trans); //writes currentSurface via DbHandler
    //must delete all inscriptions and graphs from old version of surface
    //use transaction
    static void findGraphemeInstances(QList<int>& graphIdList, const int grapheme);
        //returns list of graphIds for the corresponding grapheme
    static void getGraphImage(QImage& image, const int graphId, const int size);
        //returns image of graph specified by graphId.
    static void fixCracks();
    int pubSurf2Id(QString pubName, QString surfName) const;
private:
    //functions
    //data members
    QSqlDatabase db; //connection to the ec database
    QSqlQuery* pCorpusQuery;	//result is a list of surface ids.
    //must be a pointer, so that query can be created after db
    //is opened.
    //see: http://www.qtcentre.org/archive/index.php/t-25184.html
};

#endif
