#include <QApplication>
#include "surface_window.h"
#include "graph_viewer.h"
#include <QDebug>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    SurfaceWindow sw;
    sw.show();

   //TODO rename GraphWindow
   //TODO make it a child of and created by SurfaceWindow
   //TODO create other window types - archeology, inscriptions, words. etc.
/*
    GraphViewer gv;
    gv.show();

    qDebug() << "sizeof(int)" << sizeof(int);
*/
	return app.exec(); 
}
