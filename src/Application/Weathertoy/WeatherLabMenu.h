#ifndef _WEATHER_LAB_MENU_H__
#define _WEATHER_LAB_MENU_H__

#include "OCMain/ConfigManager.h"
#include "OCMain/Environment.h"
#include "OCUtility/StringConverter.h"
#include <qwidget.h>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>

#pragma execution_character_set("utf-8")
#define TOQSTRING(s) QString::fromLocal8Bit(s.c_str())
#define ToStdString(s)s.toLocal8Bit().toStdString()

class CWeatherLabMenu : public QWidget
{
    Q_OBJECT
public:
    CWeatherLabMenu();
    ~CWeatherLabMenu();

public slots:
    void actionOpen();
    void actionExit();
    void actionShowCompass();
    void actionShowFrameRate();
    void actionShowLengthDistance();
    void actionConvertKML();
    void actionConvertObj();
    void actionObj23dtiles();
    void actionFullScreen();
    void actionTravelAround(); 
    void actionTerrainLight();
    void actionShowTimeline();
    void actionSnow();
    void actionRain();
    void actionSun();
    void actionFog();
    void actionPick();
    void actionDebugTiles(QString actionName);
    void tryCheck(OC::String property, QString actionName, QMap<bool, QIcon>& iconMap);
    void actionClear();
    void actionConvertPipe();
    void actionConvertGltf();
    void actionConvertOblique();
    void actionConvertShp();
    
    void invokeOConv(OC::String param);
    QAction* GetAction(const QString objectName);
    QString getAppend();
protected:
    void init();
    QAction* AddQAction(QString name);
    bool mShowCompass = true;
    bool mShowFrameRate = false;
    bool mShowLengthDistance = true;
    bool mIsPicking = false;
    friend class LabViewer;
    QMap<QString, QAction*> mQActionMap;
    QMap<bool, QIcon> mPickIcon;
    QMap<bool, QIcon> mCheckIcon;
    QMap<bool, QIcon> mDebugIcon;
};



#endif // !_WEATHER_LAB_MENU_H__


