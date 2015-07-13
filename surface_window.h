#ifndef SURFACE_WINDOW_H
#define SURFACE_WINDOW_H

/*
Main window of ecdbgui. It is a somewhat arbitrary choice to make the surface
window the main window, but as good as any. Can spawn child windows for other
entities: graphs, archeology, lingistics, etc. as needed.
*/

/*TODO instead of navigating a query result set, keep a QList of id numbers, and
 * query each in turn. In that way, items can be added or removed from the list
 * by various search procedures. The list and the current item in the list can be saved
 * to the configuration file.
*/

/* TODO save list to file
 * clear list
 * save inscriptions to inscription list
 * save graphs to graph list
*/

#include <QMainWindow>
#include <QScrollArea>
#include <QDockWidget>
#include <QDialog>
#include "surface_imgs.h"
#include "db_handler.h"
#include "image_pane.h"
#include "surface_transcription.h"
#include "transcription_pane.h"
#include "config_handler.h"
#include "navigation_widget.h"

class SurfaceWindow : public QMainWindow
{
    Q_OBJECT //necessary for use of signals and slots
public:
    SurfaceWindow();	//constructor
    //sets DbHandler instance for communicating with database
    //requests DbHandler to query surfaces
    //sets up GUI
    //advances to first surface
public slots:
    void statusUpdate(); //write status bar text. TODO: make private?
    void setModified(); //sets modified to true, connect to signals in imageLabel and TranscriptionWindow
signals:
    void unlockSignal();
private slots:
    void advance();
    //requests id of next surface in pub from dbHandler
    //reads new surface
    //and sets image in imageLabel.
    void advance10();
    void advance100();
    void back();
    void back10();
    void back100();
    //TODO advance(int jump), back(int jump), advanceTenPc(), moveLast(), etc.
    void unlock();
    void editTranscription(); //opens dialog to edit currnet transcription
    void toggleFullScreen();
    void save(); //writes surface to db
    void discardChanges(); //discards all changes and reloads surface from db
    void changeFont(QAction* fontAction); //changes font according to fontActon.data

private:
    void newSurf(); //reads surface from db
    //sets up transcriptions window in scroll area
    //connects up signals and slots
    //updates status bar
    //functions to set up UI
    void closeEvent(QCloseEvent *event);
    void createActions();
    void createMenus();
    void saveWarning(); //msgBox warning to save or discard.
    //actions
    QAction* exitAction; //quit application
    QAction* advanceAction; //move to next item in publication
    QAction* advance10Action; //move 10 forward in publication
    QAction* advance100Action; //move 100 forward
    QAction* backAction; 	//decrement the row in the db if in SURFACE mode
    QAction* back10Action; //move 10 back
    QAction* back100Action; //move 100 back
    QAction* unlockAction; //allows editing of surface
    QAction* toggleFullScreenAction;
    QAction* saveAction; //save current state to db
    QAction* discardChangesAction; //discard changes and reload from db

    //actions related to imgs
    QAction* modeDownAction; //SURFACE > INSCRIPTION or INSCRIPTION > GRAPH
    QAction* modeUpAction; //GRAPH > INSCRIPTION or INSCRIPTION > SURFACE
    //TODO	QAction* saveThumbnailsAction; //save all current thumbnails to disk
    QAction* zoomInAction; 	//image * 1+1/3
    QAction* zoomOutAction;	//image * 3/4
    QAction* zoomRestoreAction;	//restore original size
    QAction* rotateClockwiseAction; // +5 degrees
    QAction* rotateAntiClockwiseAction; // -5 degrees
    QAction* rotateRestoreAction; // resent rotation to 0
    QAction* toggleIndexNumbersAction; //switches index numbers for bounding boxes on and off
    QAction* boxForwardAction; //advances box index
    QAction* boxBackAction; //box back by one
    QAction* deleteCurrentBoxAction; //deletes current boudning box
    QAction* toggleCanHaveImageAction; //toggles canHaveImage variable of currnet inscription
    QAction* raiseBoxAction; //swaps current bbox with bbox of next higher index
    //and increments current box index
    QAction* lowerBoxAction; //swaps current bbox with bbox of next lower index
    //and decrements current box index

    //actions related to transcriptions
    QAction* nextTransAction; //increments current transcription
    QAction* prevTransAction; //decrements current transcription
    QAction* deleteTransAction; //deletes current transcription
    QAction* insertTransAction; //inserts transctption before current
    QAction* raiseTransAction; //raises index number of current transcription
    QAction* lowerTransAction; //lowers index number of current transcription
    QAction* allCanHaveImageAction; //sets canHaveImage to true for all transcriptions
    QAction* editTranscriptionAction; //opens dialog window for editing a single insciption
    QAction* copyTransAction; //duplicates current inscription transcription and
        //inserts it at current index
    QList<QAction*> setFontActionList; //one action for each possible font

    //MENUS
    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* zoomMenu;
    QMenu* rotateMenu;
    QMenu* viewMenu;
    QMenu* fontSubmenu;

    //private data members
    ConfigHandler* config; //handles interaction with configuration file
    ImagePane* imagePane; //widget that holds and manipulates the image being viewed
    DbHandler db; //handles all SQL queries and other interaction with ec db
        //TODO: reimplement as namespace
    QScrollArea* imgScrollArea; //provides scroll bars for large imageLabel
    QScrollArea* transScrollArea; //provides scroll bars for the transcriptions list
    SurfaceImgs surf; //complete representation of inscribed surface
    //passed as reference to imageLabel.
    SurfaceTranscription trans; //stores complete transcription and markup for one surface
    //consists of data for surface, plus a list of inscriptionTrans, each of which
    //consists of a list of graphTrans.
    TranscriptionPane* transcriptionPane; 	//widget for displaying and editing
    //sets of transcriptions.
    QDockWidget* metadataDock; // dock window for display/editing metadata, and controls
    QDockWidget* navigationDock; // dock window for navigation
    NavigationWidget* navigationWidget;
    QDockWidget* transcriptionsDock;// dock window for the TranscriptinWindow.
    bool locked; //prevents any modification TODO: differnet kind of locks, e.g. that allows
    //reading but not writing
    bool modified; //true if either surf or trans has been modified since last save

    QStringList fonts;
    QString font;

    int currentSurfId;
    QList<int> surfList;
    int currentSurfListIndex;
}; 

#endif
