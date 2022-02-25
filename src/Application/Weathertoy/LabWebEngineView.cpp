#include "LabWebEngineView.h"
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QtWebEngineWidgets/QWebEngineSettings>

#include "LabViewer.h"
#include "OCUtility/StringUtil.h"
#include "OCMain/osg.h"

using namespace OC;

LabWebEngineView::LabWebEngineView(QWidget *parent)
    : QWebEngineView(parent)
{
    settings()->setAttribute(QWebEngineSettings::WebGLEnabled, true);
}

LabWebEngineView::~LabWebEngineView()
{
}

bool LabWebEngineView::eventFilter(QObject* obj, QEvent* ev)
{
    //if (obj == child_)
    //{
    //    switch (ev->type())
    //    {
    //    case QEvent::MouseButtonPress:
    //        mousePressEvent(static_cast<QMouseEvent*>(ev));
    //        break;
    //    case QEvent::MouseButtonRelease:
    //        mouseReleaseEvent(static_cast<QMouseEvent*>(ev));
    //        break;
    //    case QEvent::MouseMove:
    //        mouseMoveEvent(static_cast<QMouseEvent*>(ev));
    //        break;
    //    case QEvent::MouseButtonDblClick:
    //        if (this->parentWidget()->isMaximized()) 
    //        {
    //            this->parentWidget()->showNormal();
    //        }
    //        else {
    //            this->parentWidget()->showMaximized();
    //        }
    //        break;
    //    case QEvent::Leave:
    //        onMove = false;
    //        break;
    //    default:
    //        break;
    //    }
    //}

    return QWebEngineView::eventFilter(obj, ev);
}

bool LabWebEngineView::event(QEvent* event)
{
    //if (event->type() == QEvent::ChildAdded)
    //{
    //    QChildEvent* childEvent = (QChildEvent*)event;
    //    if (childEvent)
    //    {
    //        QObject* child = childEvent->child();

    //        const char* name = child->metaObject()->className();

    //        QWidget* w = qobject_cast<QWidget*>(child);
    //        if (w) {
    //            child_ = w;
    //            w->installEventFilter(this);
    //        }
    //    }
    //}
    return QWebEngineView::event(event);
}

void LabWebEngineView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void LabWebEngineView::dropEvent(QDropEvent* event)//放下事件
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
    {
        return;
    }
    foreach(QUrl url, urls)
    {
        QString qfilename = url.toLocalFile();

        OC::String filename = ToStdString(qfilename);
        String ext = osgDB::getLowerCaseFileExtension(filename);

        if (ext == "bz2")
        {
            LabViewer::instance()->openRadar(filename);
        }
        else if (ext == "kml")
        {
            LabViewer::instance()->openKML(filename);
        }
        else if (ext == "json")
        {
            LabViewer::instance()->open3DTile(filename);
        }
        else if (ext == "vtk")
        {
            LabViewer::instance()->openExtenalFile(filename);
        }
        else if (ext == "png" || ext == "jpg")
        {
            LabViewer::instance()->loadPNG(filename);
        }
        else if (FileUtil::FileType(filename) == FileUtil::DIRECTORY)
        {
            LabViewer::instance()->openFolder(filename);
        }
        
        break;
    }
}

const int TITLE_HEIGHT = 20;    //这里也可以使用宏定义，保存标题高度，也就是鼠标点击区域的高度
void LabWebEngineView::mousePressEvent(QMouseEvent* event)
{
    //if (event->y() < TITLE_HEIGHT)
    //{
    //    onMove = true;
    //    last.setX(event->globalX() - this->parentWidget()->x());
    //    last.setY(event->globalY() - this->parentWidget()->y());
    //}
}

void LabWebEngineView::mouseMoveEvent(QMouseEvent* event)
{
    //if (onMove)
    //{
    //    int dx = event->globalX() - last.x();
    //    int dy = event->globalY() - last.y();
    //    this->parentWidget()->move(dx, dy);
    //}
}

void LabWebEngineView::mouseReleaseEvent(QMouseEvent* event)
{
    onMove = false;
}