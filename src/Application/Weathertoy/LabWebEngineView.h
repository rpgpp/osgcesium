#pragma once

#include <QtWebEngineWidgets/QWebEngineView>
#include <QOpenGLWidget>
#include <QPointer>

class LabWebEngineView : public QWebEngineView
{
    Q_OBJECT

public:
    LabWebEngineView(QWidget *parent);
    ~LabWebEngineView(); 
protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

    bool event(QEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event); 
    bool eventFilter(QObject* obj, QEvent* ev);
private:
    bool onMove = false;
    QPoint last = QPoint(0, 0);
    QPointer<QWidget> child_;
};
