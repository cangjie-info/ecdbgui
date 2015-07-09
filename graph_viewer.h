#ifndef GRAPH_VIEWER_H
#define GRAPH_VIEWER_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
//#include <QVBoxLayout>
#include "flow_layout.h"

//TODO reconceive as a graph window & a inscr_graphs window, the latter potentially one of several, that can
// hold a list of graphs and a current graph.
// the graph window should be able to assign lingvals, to particular
// inscr_graphs or to all. It should be able to send its current id to a named inscr_graphs window.
// The inscr_graphs window shoudl be able to allow selections, and to send the selection to another named widow. etc.

class GraphViewer : public QWidget
{
    Q_OBJECT
public:
    explicit GraphViewer(QWidget *parent = 0);

signals:

public slots:
private slots:
    void processInput();

private:
    QLineEdit* inputBox;
    FlowLayout* mainLayout;
    QList<QLabel*> labelList;
};

#endif // GRAPH_VIEWER_H
