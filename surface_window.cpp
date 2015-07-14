#include "surface_window.h"
#include <QtWidgets>
#include <QDebug>
#include <QMessageBox>
#include "edit_transcription_dialog.h"

SurfaceWindow::SurfaceWindow()
    : imagePane(NULL), transcriptionPane(NULL)
{
    //open config file
    config = new ConfigHandler();

    //set up connection with db
    db = DbHandler();
    if(db.connect())
        qDebug() << "Connected to db.";
    else
        qDebug() << "Connection to db failed.";

    currentSurfId = config->getLastSurf();

    //create ImageLabel - for displaying the surface images
    imagePane = new ImagePane(this, &surf); //surf is passed as pointer
    //so that imageLabel can manipulate it.

    //TODO dock area to hold metadata and control buttons.
    metadataDock = new QDockWidget("Surface metadata", this);
    metadataDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, metadataDock);
    //TODO populate with data and controls.

    navigationDock = new QDockWidget("Surface navigation", this);
    navigationDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, navigationDock);
    navigationWidget = new NavigationWidget(&db, this);
    navigationDock->setWidget(navigationWidget);
    connect(navigationWidget, SIGNAL(setSurf(int)), this, SLOT(setSurf(int)));


    //create dock and scroll area for transWindow
    //TODO - currently, the dock can be closed and there is no way to
    //reopen it => fix by adding a Window menu with a show dock action.
    transcriptionsDock = new QDockWidget("Surface transcriptions", this);
    transcriptionsDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, transcriptionsDock);
    transScrollArea = new QScrollArea(this);
    transScrollArea->setMinimumWidth(500);
    transcriptionsDock->setWidget(transScrollArea);

    fonts = config->getFontList();
    font = config->getFont();

    createActions();
    createMenus();

    //create scroll area for surface image
    imgScrollArea = new QScrollArea(this);
    imgScrollArea->setBackgroundRole(QPalette::Dark); //assigns bground color accroding to theme
    imgScrollArea->setWidget(imagePane);	//scrollArea holds the imageLabel
    setCentralWidget(imgScrollArea); //makes scrollArea the central widget of the MainWindow (SurfaceWindow)

    showMaximized();

    //TODO replace this with getSavedSurfList() and make moveToSurf a method of this.
//    db.moveToSurf(config->getLastSurf());

    newSurf();
}

void SurfaceWindow::closeEvent(QCloseEvent* event)
{
    if(modified)
    {
        QMessageBox msgBox(this);
        msgBox.setText("Save or discard changes before closing.");
        msgBox.exec();
        event->ignore();
        return;
    }
    //TODO modify to save current list of surfs, and the current surf
      //saving as id numbers.
    config->setLastSurf(currentSurfId);
    config->save();
    event->accept();

}

void SurfaceWindow::newSurf()
{
    db.readSurface(currentSurfId, surf, trans, font);

    //TODO - decide if locked is useful or not
    locked = true;
    modified = false;

    imagePane->newSurf();
    //delete old transcription window and create new one
    if(transcriptionPane)
        delete transcriptionPane;
    transcriptionPane = new TranscriptionPane(&trans, &surf, font);
    //connect ImageLabel signals to transcription window slots
    connect(imagePane, SIGNAL(inscrImgListModified()), transcriptionPane, SLOT(refresh()));
    connect(imagePane, SIGNAL(inscrImgListModified()), this, SLOT(setModified()));
    //connect Actions to transcription window slots
    connect(toggleCanHaveImageAction, SIGNAL(triggered()), transcriptionPane, SLOT(toggleCanHaveImage()));
    connect(nextTransAction, SIGNAL(triggered()), transcriptionPane, SLOT(nextInscription()));
    connect(prevTransAction, SIGNAL(triggered()), transcriptionPane, SLOT(prevInscription()));
    connect(deleteTransAction, SIGNAL(triggered()), transcriptionPane, SLOT(deleteInscription()));
    connect(insertTransAction, SIGNAL(triggered()), transcriptionPane, SLOT(insertInscription()));
    connect(raiseTransAction, SIGNAL(triggered()), transcriptionPane, SLOT(raiseInscription()));
    connect(lowerTransAction, SIGNAL(triggered()), transcriptionPane, SLOT(lowerInscription()));
    connect(allCanHaveImageAction, SIGNAL(triggered()), transcriptionPane, SLOT(allCanHaveImage()));
    connect(copyTransAction, SIGNAL(triggered()), transcriptionPane, SLOT(copyTrans()));
    connect(this, SIGNAL(unlockSignal()), transcriptionPane, SLOT(unlock()));
    connect(this, SIGNAL(unlockSignal()), imagePane, SLOT(unlock()));

    connect(transcriptionPane, SIGNAL(inscrListModified()), this, SLOT(setModified()));

    //install transcription window in scroll area in dock
    transScrollArea->setWidget(transcriptionPane);
    transcriptionPane->show();

    //TODO update for metadata dock
    //TODO set to inscription mode if there is alread a bounding box.
    if(!surf.isNull()) imagePane->modeDown();
    statusUpdate();
    imagePane->setFocus();//ensure all keystrokes are captured,
                        //instead of going to controls
}

void SurfaceWindow::saveWarning()
{
   QMessageBox msgBox(this);
   msgBox.setText("Save or discard changes before moving to a new surface.");
   msgBox.exec();
}

void SurfaceWindow::advance()
{
//  if(imagePane->getMode() == ImagePane::SURFACE) //can only advance to a new surface from SURFACE mode
    {
        if(modified) saveWarning();
        else
        {
            currentSurfId = db.getNextSurfIdInPub(currentSurfId); //stays put if already on last record
            newSurf();
        }
    }
}

void SurfaceWindow::advance10()
{
   {
        if(modified) saveWarning();
        else
        {
            currentSurfId = db.getNext10SurfIdInPub(currentSurfId);
                  //stays put if already within 10 of last record
            newSurf();
        }
    }
}

void SurfaceWindow::advance100()
{
   {
        if(modified) saveWarning();
        else
        {
            currentSurfId = db.getNext100SurfIdInPub(currentSurfId);
                  //stays put if already within 100 of last record
            newSurf();
        }
    }
}

void SurfaceWindow::back()
{
        if(modified) saveWarning();
        else
        {
            currentSurfId = db.getPreviousSurfIdInPub(currentSurfId);
            newSurf();
        }
}

void SurfaceWindow::back10()
{
   if(modified) saveWarning();
   else
   {
       currentSurfId = db.getPrevious10SurfIdInPub(currentSurfId);
       newSurf();
   }
}

void SurfaceWindow::back100()
{
   if(modified) saveWarning();
   else
   {
       currentSurfId = db.getPrevious100SurfIdInPub(currentSurfId);
       newSurf();
   }
}

void SurfaceWindow::save()
{
//TODO    removeTrailingNulls; (This is probably not necessary - check)
    db.writeSurface(currentSurfId, surf, trans);
    modified = false;
    statusUpdate();
}

void SurfaceWindow::discardChanges()
{
    if(modified)
    {
        //confirm
        QMessageBox msgBox(this);
        msgBox.setText("This will discard all changes made to this surface.");
        msgBox.setInformativeText("OK to proceed?");
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Warning);
        if(msgBox.exec() == QMessageBox::Ok)
            newSurf();
    }
}

void SurfaceWindow::unlock()
{
    //unlock imageLabel and transcription window by emitting unlock signal
    emit unlockSignal();
    locked = false;
    statusUpdate();
}

void SurfaceWindow::toggleFullScreen()
{
    setWindowState(windowState() ^ Qt::WindowFullScreen);
}

void SurfaceWindow::editTranscription()
{
    int transcriptionIndex = transcriptionPane->getCurrentTranscriptionIndex();
    int imageIndex = transcriptionPane->getCurrentImageIndex();
    if(locked ||
       transcriptionIndex == trans.count()) //index set to the "append" position)
        return;
    //pass to the dialog
    // ITEM - one pointer to inscription transcription (read/write)
    InscriptionTranscription* const pInscrTrans = &trans[transcriptionIndex];
    // ITEM - list of graph images (read only)
    QList<QImage> imgList;
    imagePane->getGraphImageList(imageIndex, imgList, QSize(100, 100));

    EditTranscriptionDialog dialog(this, pInscrTrans, imgList);
//    dialog.setFontTo(font);

    if(dialog.exec())
    {
        modified = true;
        statusUpdate();
        transcriptionPane->refresh();
    }
}

void SurfaceWindow::statusUpdate()
{
    //TODO - some of these are metadata and not status items - move to metadata pane.
    QString statusText;
    statusText += QString(
            "surface: %1, surface type: %2 | zoom = x%3, rotation = %4 | mode = %5 | %6LOCKED%7")
            .arg(trans.getPubName() + trans.getPubNumber()) //TODO move
            .arg(trans.getSurfaceType())  //TODO move
            .arg(imagePane->getZoom())
            .arg(imagePane->getRotation())
            .arg(imagePane->getModeName())
            .arg(locked ? "" : "UN")
            .arg(modified ? " | MODIFIED" : "");
    statusText += QString("font: %1").arg(font);
    statusBar()->showMessage(statusText);
}

void SurfaceWindow::setModified()
{
    modified = true;
    statusUpdate();
}

void SurfaceWindow::createActions()
{
   // ISSUE: Qt5 doesn't recognised shortcuts unless actions are added to a toolbar
   // This is due to a bug in the current version of appmenu-qt5, so I have uninstalled that
   // package. It breaks the window theme, but seem to have no serious practical consequences.
   // The alternative is to add addAction(<ActionName>) for all actons.
   // https://bugs.launchpad.net/ubuntu/+source/appmenu-qt5/+bug/1313248


    //TODO navigate list, as well as publication
    //TODO append current to list
    //TODO remove current item from list (if it is a list item)
    //TODO move by 10 and 100, and move last, first.

    //TODO new surface image

    //TODO open various other windows.

    //TODO dump list to html/pdf/rtf

    //TODO dump current to html/pdf/rtf

    //TODO save image thumbnails for surface
    //	saveThumbnailsAction = new QAction(tr("&Save thumbnails"), this);
    //	saveThumbnailsAction->setShortcut(tr("Ctrl+S"));
    //	connect(saveThumbnailsAction, SIGNAL(triggered()), imageLabel, SLOT(saveThumbnails()));

    exitAction = new QAction(QIcon(":/icons/window-close.png"), tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    advanceAction = new QAction(tr("&Advance"), this);
    advanceAction->setShortcut(tr("a"));
    connect(advanceAction, SIGNAL(triggered()), this, SLOT(advance()));

    advance10Action = new QAction("Advance 10", this);
    advance10Action->setShortcut(tr("Shift+a"));
    connect(advance10Action, SIGNAL(triggered()), this, SLOT(advance10()));

    advance100Action = new QAction("Advance 100", this);
    advance100Action->setShortcut(tr("Ctrl+a"));
    connect(advance100Action, SIGNAL(triggered()), this, SLOT(advance100()));

    backAction = new QAction(tr("&Back"), this);
    backAction->setShortcut(tr("b"));
    connect(backAction, SIGNAL(triggered()), this, SLOT(back()));

    back10Action = new QAction(tr("Back 10"), this);
    back10Action->setShortcut(tr("Shift+b"));
    connect(back10Action, SIGNAL(triggered()), this, SLOT(back10()));

    back100Action = new QAction(tr("Back 100"), this);
    back100Action->setShortcut(tr("Ctrl+b"));
    connect(back100Action, SIGNAL(triggered()), this, SLOT(back100()));

    modeDownAction = new QAction(tr("Mode Down"), this);
    modeDownAction->setShortcut(tr("Space"));
    connect(modeDownAction, SIGNAL(triggered()), imagePane, SLOT(modeDown()));
    connect(modeDownAction, SIGNAL(triggered()), this, SLOT(statusUpdate()));

    modeUpAction = new QAction(tr("Mode Up"), this);
    modeUpAction->setShortcut(tr("Escape"));
    connect(modeUpAction, SIGNAL(triggered()), imagePane, SLOT(modeUp()));
    connect(modeUpAction, SIGNAL(triggered()), this, SLOT(statusUpdate()));

    unlockAction = new QAction(tr("&Unlock"), this);
    unlockAction->setShortcut(tr("U"));
    connect(unlockAction, SIGNAL(triggered()), this, SLOT(unlock()));

    boxForwardAction = new QAction(tr("Box &Forward"), this);
    boxForwardAction->setShortcut(tr("]"));
    connect(boxForwardAction, SIGNAL(triggered()), imagePane, SLOT(advanceCurrentBoxIndex()));

    boxBackAction = new QAction(tr("Box &Backward"), this);
    boxBackAction->setShortcut(tr("["));
    connect(boxBackAction, SIGNAL(triggered()), imagePane, SLOT(reverseCurrentBoxIndex()));

    deleteCurrentBoxAction = new QAction(tr("&Delete box"), this);
    deleteCurrentBoxAction->setShortcut(tr("Backspace"));
    connect(deleteCurrentBoxAction, SIGNAL(triggered()), imagePane, SLOT(deleteCurrentBox()));

    zoomInAction = new QAction(tr("&Zoom In"), this);
    zoomInAction->setShortcut(tr("+"));
    connect(zoomInAction, SIGNAL(triggered()), imagePane, SLOT(zoomIn()));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(statusUpdate()));

    zoomOutAction = new QAction(tr("Zoom &Out"), this);
    zoomOutAction->setShortcut(tr("-"));
    connect(zoomOutAction, SIGNAL(triggered()), imagePane, SLOT(zoomOut()));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(statusUpdate()));

    zoomRestoreAction = new QAction(tr("&Restore"), this);
    zoomRestoreAction->setShortcut(tr("0"));
    connect(zoomRestoreAction, SIGNAL(triggered()), imagePane, SLOT(zoomRestore()));
    connect(zoomRestoreAction, SIGNAL(triggered()), this, SLOT(statusUpdate()));

    rotateClockwiseAction = new QAction(tr("&Clockwise 5 deg"), this);
    rotateClockwiseAction->setShortcut(tr(">"));
    connect(rotateClockwiseAction, SIGNAL(triggered()), imagePane, SLOT(rotateClockwise()));
    connect(rotateClockwiseAction, SIGNAL(triggered()), this, SLOT(statusUpdate()));

    rotateAntiClockwiseAction = new QAction(tr("&Anticlockwise 5 deg"), this);
    rotateAntiClockwiseAction->setShortcut(tr("<"));
    connect(rotateAntiClockwiseAction, SIGNAL(triggered()), imagePane, SLOT(rotateAntiClockwise()));
    connect(rotateAntiClockwiseAction, SIGNAL(triggered()), this, SLOT(statusUpdate()));

    rotateRestoreAction = new QAction(tr("&Restore"), this);
    rotateRestoreAction->setShortcut(tr("^"));
    connect(rotateRestoreAction, SIGNAL(triggered()), imagePane, SLOT(rotateRestore()));
    connect(rotateRestoreAction, SIGNAL(triggered()), this, SLOT(statusUpdate()));

    toggleIndexNumbersAction = new QAction(tr("&Toggle Index Numbers"), this);
    toggleIndexNumbersAction->setShortcut(tr("i"));
    connect(toggleIndexNumbersAction, SIGNAL(triggered()), imagePane, SLOT(toggleIndexNumbers()));

    toggleCanHaveImageAction = new QAction(tr("Toggle &No Image"), this);
    toggleCanHaveImageAction->setShortcut(Qt::Key_N + Qt::ShiftModifier);
    //connect must follow creation of transWindow (et seq.)

    nextTransAction = new QAction(tr("Next transcription"), this);
    nextTransAction->setShortcut(Qt::Key_Down + Qt::ShiftModifier);
    //ditto

    prevTransAction = new QAction(tr("Previous transcription"), this);
    prevTransAction->setShortcut(Qt::Key_Up + Qt::ShiftModifier);
    //ditto

    deleteTransAction = new QAction(tr("Delete transcription"), this);
    deleteTransAction->setShortcut(Qt::ShiftModifier + Qt::Key_Backspace);
    //ditto

    insertTransAction = new QAction(tr("Insert transcription"), this);
    insertTransAction->setShortcut(Qt::Key_Return + Qt::ShiftModifier);
    //ditto

    raiseTransAction = new QAction(tr("Raise current transcription index"), this);
    raiseTransAction->setShortcut(Qt::Key_Down + Qt::ControlModifier);
    //ditto

    lowerTransAction = new QAction(tr("Lower current transcription index"), this);
    lowerTransAction->setShortcut(Qt::Key_Up + Qt::ControlModifier);
    //ditto

    allCanHaveImageAction = new QAction(tr("All can have images"), this);
    allCanHaveImageAction->setShortcut(tr("Ctrl+n"));
    //ditto

    editTranscriptionAction = new QAction(tr("&Edit inscription"), this);
    editTranscriptionAction->setShortcut(tr("e"));
    connect(editTranscriptionAction, SIGNAL(triggered()), this, SLOT(editTranscription()));

    toggleFullScreenAction = new QAction("&Full screen", this);
    toggleFullScreenAction->setShortcut(Qt::Key_F11);
    connect(toggleFullScreenAction, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));

    copyTransAction = new QAction("Du&plicate transcription", this);
    copyTransAction->setShortcut(Qt::Key_C + Qt::ShiftModifier);

    raiseBoxAction = new QAction("&Raise bounding box index", this);
    raiseBoxAction->setShortcut(tr("Ctrl+]"));
    connect(raiseBoxAction, SIGNAL(triggered()), imagePane, SLOT(raiseBoxIndex()));

    lowerBoxAction = new QAction("&Lower bounding box index", this);
    lowerBoxAction->setShortcut(tr("Ctrl+["));
    connect(lowerBoxAction, SIGNAL(triggered()), imagePane, SLOT(lowerBoxIndex()));

    discardChangesAction = new QAction("&Discard changes and reload", this);
    discardChangesAction->setShortcut(tr("Ctrl+Shift+X"));
    connect(discardChangesAction, SIGNAL(triggered()), this, SLOT(discardChanges()));

    saveAction = new QAction("&Save changes to database", this);
    saveAction->setShortcut(tr("Ctrl+S"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    foreach (QString fontName, fonts)
    {
       QAction* setFontAction = new QAction(fontName, this);
       setFontAction->setData(fontName);
 //    NO!!  connect(setFontAction, SIGNAL(triggered()), this, SLOT(changeFont()));
 //   confusingly, it is the submenu, not the action that needs to be connected. WHY??
       setFontActionList.append(setFontAction);
    }

    showNavigationDockAction = new QAction("Show navigation dock", this);
    connect(showNavigationDockAction, SIGNAL(triggered()),
            navigationDock, SLOT(show()));
    showMetadataDockAction = new QAction("Show metadata dock", this);
    connect(showMetadataDockAction, SIGNAL(triggered()),
            metadataDock, SLOT(show()));
    showTranscriptionsDockAction = new QAction("Show transcriptions dock", this);
    connect(showTranscriptionsDockAction, SIGNAL(triggered()),
            transcriptionsDock, SLOT(show()));
}

void SurfaceWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);
    fileMenu->addAction(discardChangesAction);
    fileMenu->addAction(saveAction);
    //	fileMenu->addAction(saveThumbnailsAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(boxForwardAction);
    editMenu->addAction(boxBackAction);
    editMenu->addAction(deleteCurrentBoxAction);
    editMenu->addAction(advanceAction);
    editMenu->addAction(advance10Action);
    editMenu->addAction(advance100Action);
    editMenu->addAction(unlockAction);
    editMenu->addAction(backAction);
    editMenu->addAction(back10Action);
    editMenu->addAction(back100Action);
    editMenu->addAction(modeUpAction);
    editMenu->addAction(modeDownAction);
    editMenu->addAction(toggleCanHaveImageAction);
    editMenu->addAction(nextTransAction);
    editMenu->addAction(prevTransAction);
    editMenu->addAction(deleteTransAction);
    editMenu->addAction(insertTransAction);
    editMenu->addAction(raiseTransAction);
    editMenu->addAction(lowerTransAction);
    editMenu->addAction(allCanHaveImageAction);
    editMenu->addAction(editTranscriptionAction);
    editMenu->addAction(copyTransAction);
    editMenu->addAction(raiseBoxAction);
    editMenu->addAction(lowerBoxAction);

    zoomMenu = menuBar()->addMenu(tr("&Zoom"));
    zoomMenu->addAction(zoomInAction);
    zoomMenu->addAction(zoomOutAction);
    zoomMenu->addAction(zoomRestoreAction);

    rotateMenu = menuBar()->addMenu(tr("&Rotate"));
    rotateMenu->addAction(rotateClockwiseAction);
    rotateMenu->addAction(rotateAntiClockwiseAction);
    rotateMenu->addAction(rotateRestoreAction);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(toggleIndexNumbersAction);
    viewMenu->addAction(toggleFullScreenAction);
    fontSubmenu = viewMenu->addMenu("Set &font");
    foreach (QAction* fontAction, setFontActionList)
    {
       fontSubmenu->addAction(fontAction);
    }
    connect(fontSubmenu, SIGNAL(triggered(QAction *)),
            this, SLOT(changeFont(QAction *)));
    viewMenu->addAction(showNavigationDockAction);
    viewMenu->addAction(showMetadataDockAction);
    viewMenu->addAction(showTranscriptionsDockAction);
}

void SurfaceWindow::changeFont(QAction *fontAction)
{
   font = fontAction->data().toString();
   config->setFont(font);
   QMessageBox msgBox;
   msgBox.setText("The new font will be applied on the next surface load.");
   msgBox.exec();
}

void SurfaceWindow::setSurf(int newSurfId)
{
   //NEED TO CONTROL FOR MODIFIED INSCRIPTIONS
   currentSurfId = newSurfId;
   newSurf();
}
