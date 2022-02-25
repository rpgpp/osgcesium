#include "Bridge.h"
#include <QMessageBox>
#include <QApplication>
#include <QAction>
#include "LabViewer.h"
#include "WeatherLabMenu.h"

Bridge::Bridge()
    : QObject(NULL)
{
}

Bridge::~Bridge()
{
}

Bridge* Bridge::instance()
{
    static Bridge s_obj;
    return &s_obj;
}

void Bridge::eval(QString method, QString json)
{
    if (method == "showMinimized")
    {
        LabViewer::instance()->showMinimized();
    }
    else if (method == "showMaximized")
    {
        if (LabViewer::instance()->isMaximized()) 
        {
            LabViewer::instance()->showNormal();
        }
        else {
            LabViewer::instance()->showMaximized();
        }
    }
    else
    {
        if (QAction* action = LabViewer::instance()->getWeatherLabMenu()->GetAction(method))
        {
            action->trigger();
        }

    }
}

void Bridge::showMsgBox()
{
    QMessageBox::aboutQt(0, tr("Qt"));
}