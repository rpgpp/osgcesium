#include "WeatherLabMenu.h"
#include "LabViewer.h"
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QObject>
#include <QProcess>
#include <QThread>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

#include "OCMain/osg.h"

#ifdef HAS_UI
#include "OCUI/PrepareHandler.h"
#include "OCUI/CoreGUI.h"
#include "OCUI/GUIArchive.h"
#endif

#include "OCesium/gltfConfig.h"

using namespace OC;

#include "ThunderKML.h"
#include "RadarStationKML.h"

#define CONFIG_actionShowCompass "WeatherLab::actionShowCompass"
#define CONFIG_actionShowFrameRate "WeatherLab::actionFrameRate"
#define CONFIG_actionShowLengthDistance "WeatherLab::actionShowLengthDistance"
#define CONFIG_actionConvertKML "WeatherLab::actionConvertKML"
#define CONFIG_OConv "WeatherLab::OConv"
#define CONFIG_OPENDIR "WeatherLab::OpenDir"

#define ADD_ACTION(name)connect(AddQAction(#name), SIGNAL( triggered() ), this, SLOT( name() ) )
#define ADD_ACTION_P(name,func)connect(AddQAction(#name), &QAction::triggered, this, [this]{func(#name);})

CWeatherLabMenu::CWeatherLabMenu()
{
    init();
}

CWeatherLabMenu::~CWeatherLabMenu()
{
}

QString CWeatherLabMenu::getAppend()
{
    QString js;
    {
        QFile file;
        file.setFileName(":/main/qwebchannel.js");
        file.open(QIODevice::ReadOnly);
        js.append(file.readAll());
        file.close();
    }

    {
        QFile file;
        file.setFileName(":/main/cplusplus.js");
        file.open(QIODevice::ReadOnly);
        js.append(file.readAll());
        file.close();
    }

    QString imagePath = TOQSTRING(Singleton(OC::Environment).getAppDir()) + "/image/";
    mCheckIcon[true] = QIcon(imagePath + "icon/done [#1477]");
    mCheckIcon[false] = QIcon(imagePath + "icon/uncheck");
    mPickIcon[false] = QIcon(imagePath + "icon/mouse_pointer [#6]");
    mPickIcon[true] = QIcon(imagePath + "icon/g_mouse_pointer [#6]");
    mDebugIcon[true] = QIcon(imagePath + "icon/bug_plus [#829]");
    mDebugIcon[false] = QIcon(imagePath + "icon/bug [#828]");
    

    GetAction("actionShowCompass")->setIcon(mCheckIcon[mShowCompass]);
    GetAction("actionShowLengthDistance")->setIcon(mCheckIcon[mShowLengthDistance]);
    GetAction("actionShowFrameRate")->setIcon(mCheckIcon[mShowFrameRate]);

    QString showCompass = "window.initWe.showNavigationWidget = ";
    showCompass += mShowCompass ? "true;" : "false;";
    QString showLengthDistance = "window.initWe.showDistanceLengthWidget = ";
    showLengthDistance += mShowLengthDistance ? "true;" : "false;";
    QString showDebugShowFramesPerSecond = "window.initWe.debugShowFramesPerSecond = ";
    showDebugShowFramesPerSecond += mShowFrameRate ? "true;" : "false;";

    js += showCompass;
    js += showLengthDistance;
    js += showDebugShowFramesPerSecond;

    //{
    //    QFile file;
    //    file.setFileName(":/main/WeatherEarth.js");
    //    file.open(QIODevice::ReadOnly);
    //    js.append(file.readAll());
    //    file.close();
    //}
    //{
    //    QFile file;
    //    file.setFileName(":/main/app.js");
    //    file.open(QIODevice::ReadOnly);
    //    js.append(file.readAll());
    //    file.close();
    //}

    return js;
}

QAction* CWeatherLabMenu::AddQAction(QString name)
{
    QAction* active = new QAction(name, this);
    active->setObjectName(name);
    mQActionMap[name] = active;
    return active;
}

QAction* CWeatherLabMenu::GetAction(const QString objectName)
{
    QString name = objectName;
    return mQActionMap[name];
}

void CWeatherLabMenu::init()
{
#ifdef HAS_UI
    new CCoreGUI(NULL);
#endif
    ADD_ACTION(actionOpen);
    ADD_ACTION(actionExit);
    ADD_ACTION(actionConvertKML);
    ADD_ACTION(actionConvertObj);
    ADD_ACTION(actionObj23dtiles);
    ADD_ACTION(actionShowCompass);
    ADD_ACTION(actionShowLengthDistance);
    ADD_ACTION(actionShowFrameRate);
    ADD_ACTION(actionClear);
    ADD_ACTION(actionPick);
    ADD_ACTION(actionShowTimeline);
    ADD_ACTION(actionTravelAround);
    ADD_ACTION_P(debugShowBoundingVolume, actionDebugTiles);
    ADD_ACTION_P(debugShowRenderingStatistics, actionDebugTiles);
    ADD_ACTION_P(debugShowMemoryUsage, actionDebugTiles); 
    ADD_ACTION_P(debugShowUrl, actionDebugTiles);
    ADD_ACTION_P(debugColorizeTiles, actionDebugTiles); 
    ADD_ACTION_P(debugShowGeometricError, actionDebugTiles);
    ADD_ACTION(actionShowGlobe);
    ADD_ACTION(actionTerrainLight);
    ADD_ACTION(actionTerrainDepthTest);
    ADD_ACTION(actionSun);
    ADD_ACTION(actionSnow);
    ADD_ACTION(actionRain);
    ADD_ACTION(actionFog);
    ADD_ACTION(actionBookmark);
    ADD_ACTION(actionLocate);
    ADD_ACTION(actionBuilding);
    ADD_ACTION(actionBuildingShadow);
    ADD_ACTION(actionStyle);

    //
    ADD_ACTION(actionLoadWind);
    ADD_ACTION(actionLoadCloud);
    ADD_ACTION(actionLoadRadar);
    ADD_ACTION(actionLoadThunder);

    //
    ADD_ACTION(actionRectVolume);
    ADD_ACTION(actionLineVolume);

    //
    ADD_ACTION(actionDrawPin);
    ADD_ACTION(actionDrawDivPin);
    ADD_ACTION(actionDrawLine);
    ADD_ACTION(actionDrawCircle);
    ADD_ACTION(actionDrawRect);


    //
    ADD_ACTION(actionGifWall);
    ADD_ACTION(actionGifLine);
    ADD_ACTION(actionGifSurf);
    ADD_ACTION(actionODLine);
    ADD_ACTION(actionVideo);

    ADD_ACTION(actionNightVision);
    ADD_ACTION(actionMosic);
    ADD_ACTION(actionDoubleSide);
    ADD_ACTION(actionTerrainShader);

    //
    ADD_ACTION(actionConvertGltf);
    ADD_ACTION(actionConvertShp);
    ADD_ACTION(actionConvertOblique);
    ADD_ACTION(actionConvertPipe);
    ADD_ACTION(actionConvertI3dm);
    ADD_ACTION(actionConvertPnts);
    ADD_ACTION(actionConvertCommand);

    //
    ADD_ACTION(actionLink);
    ADD_ACTION(actionAbout);


    mShowCompass = AppCfg.getBoolValue(CONFIG_actionShowCompass, true);
    mShowLengthDistance = AppCfg.getBoolValue(CONFIG_actionShowLengthDistance, true);
    mShowFrameRate = AppCfg.getBoolValue(CONFIG_actionShowFrameRate, false);

    QMenuBar* mb = LabViewer::instance()->menuBar();
    QMenu* menu = new QMenu("文件(&F)");
    mb->addMenu(menu);

    QAction* actionOpen = new QAction("打开(&O)", this);
    menu->addAction(actionOpen);

    menu->addSeparator();
    QAction* actionExit = new QAction("退出(&X)", this);
    menu->addAction(actionExit);

    connect(actionOpen, SIGNAL(triggered()), GetAction("actionOpen"), SIGNAL(triggered()));
    connect(actionExit, SIGNAL(triggered(bool)), GetAction("actionExit"), SIGNAL(triggered()));

    //options
    {
        QMenu* menu = new QMenu("选项(&O)");
        mb->addMenu(menu);

        QMenu* menuDisplay = new QMenu("显示");
        QAction* actionShowCompass = new QAction("导航条", this);
        actionShowCompass->setCheckable(true);
        menuDisplay->addAction(actionShowCompass);
        actionShowCompass->setChecked(mShowCompass);
        connect(actionShowCompass, SIGNAL(triggered()), this, SLOT(actionShowCompass()));

        QAction* actionShowLengthDistance = new QAction("比例尺", this);
        actionShowLengthDistance->setCheckable(true);
        menuDisplay->addAction(actionShowLengthDistance);
        actionShowLengthDistance->setChecked(mShowLengthDistance);
        connect(actionShowLengthDistance, SIGNAL(triggered()), this, SLOT(actionShowLengthDistance()));

        QAction* actionShowFrameRate = new QAction("帧率", this);
        actionShowFrameRate->setCheckable(true);
        menuDisplay->addAction(actionShowFrameRate);
        actionShowFrameRate->setChecked(mShowFrameRate);
        connect(actionShowFrameRate, SIGNAL(triggered()), GetAction("actionShowFrameRate"), SIGNAL(triggered()));

        
        QAction* actionFullScreen = new QAction("全屏", this);
        menuDisplay->addAction(actionFullScreen);
        connect(actionFullScreen, SIGNAL(triggered()), this, SLOT(actionFullScreen()));

        menu->addMenu(menuDisplay);
    }

    //tools
    {
        QMenu* menu = new QMenu("工具(&O)");
        mb->addMenu(menu);

        QAction* actionConvertKML = new QAction("转KML", this);
        menu->addAction(actionConvertKML);
        connect(actionConvertKML, SIGNAL(triggered(bool)), GetAction("actionConvertKML"), SIGNAL(triggered()));

        QAction* actionObj23dtiles = new QAction("转3dtiles", this);
        menu->addAction(actionObj23dtiles);
        connect(actionObj23dtiles, SIGNAL(triggered(bool)), GetAction("actionObj23dtiles"), SIGNAL(triggered()));

        QAction* actionConvertObj = new QAction("模型偏移", this);
        menu->addAction(actionConvertObj);
        connect(actionConvertObj, SIGNAL(triggered(bool)), GetAction("actionConvertObj"), SIGNAL(triggered()));
    }
}

QRect normalRect;
void CWeatherLabMenu::actionFullScreen()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect geom = LabViewer::instance()->geometry();
    QRect mm = screen->availableGeometry();

    if (geom.width() == mm.width() + 1
    || geom.height() == mm.height() + 1) {
        LabViewer::instance()->showFullScreen();
        LabViewer::instance()->showNormal();
        LabViewer::instance()->setGeometry(normalRect);
    }
    else
    {
        normalRect = LabViewer::instance()->geometry();
        LabViewer::instance()->showFullScreen();
        LabViewer::instance()->setGeometry(0, 0, mm.width() + 1, mm.height() + 1);
    }
}


void CWeatherLabMenu::invokeOConv(String param)
{
#ifdef HAS_UI
    class objConvertHandler : public CPrepareHandler
    {
    public:
        String param;
        objConvertHandler()
        {}
        virtual bool ready()
        {
            QProcess process(NULL);
            QString filename = QCoreApplication::applicationDirPath() + "/OConv.exe";

            QStringList argv;
            argv.push_back(TOQSTRING(param));
            process.start(filename, argv);
            if (process.waitForFinished())
            {
                return true;
            }

            return false;
        }
    };

    osg::ref_ptr<objConvertHandler> worker = new objConvertHandler();
    worker->param = param;
    worker->mPrepareMessage = U2A(L"正在执行，请稍后...");
    if (IGUIArchive* guiarchive = Singleton(CCoreGUI).get_GUIArcvhie("commonui"))
    {
        guiarchive->show_loading(worker);
    }
    else
    {
        worker->ready();
    }
#endif
}

void CWeatherLabMenu::actionClear()
{
    String evalFunction = String("clearAll();");
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::actionShowCompass()
{
    mShowCompass = !mShowCompass;

    GetAction("actionShowCompass")->setIcon(mCheckIcon[mShowCompass]);

    AppCfg.setConfig(CONFIG_actionShowCompass, mShowCompass);
    AppCfg.save();

    String func = "function(){\
        const { WE } = window;\
        WE.showNavigationWidget = " + StringConverter::toString(mShowCompass) + ";\
    }";

    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::actionShowFrameRate()
{
    mShowFrameRate = !mShowFrameRate;

    GetAction("actionShowFrameRate")->setIcon(mCheckIcon[mShowFrameRate]);

    AppCfg.setConfig(CONFIG_actionShowFrameRate, mShowFrameRate);
    AppCfg.save();

    String func = "function(){\
        const { WE } = window;\
        WE.debugShowFramesPerSecond = " + StringConverter::toString(mShowFrameRate) + ";\
    }";

    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::actionShowLengthDistance()
{
    mShowLengthDistance = !mShowLengthDistance;

    GetAction("actionShowLengthDistance")->setIcon(mCheckIcon[mShowLengthDistance]);

    AppCfg.setConfig(CONFIG_actionShowLengthDistance, mShowLengthDistance);
    AppCfg.save();

    String func = "function(){\
        const { WE } = window;\
        WE.showDistanceLengthWidget = " + StringConverter::toString(mShowLengthDistance) + ";\
    }";

    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::actionDebugTiles(QString actionName)
{
    String func = "function(){\
        const { WE } = window;\
        var layername = '3dtiles';\
        var layer = WE.layerManager.getById(layername);\
        if (Cesium.defined(layer)) {\
            layer." + ToStdString(actionName) + " = !layer." + ToStdString(actionName) + ";"\
        "}"\
    "}";

    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::tryCheck(OC::String property, QString actionName, QMap<bool, QIcon>& iconMap)
{
    String func = "function(){\
        const { WE } = window;\
            return WE." + property + ";\
        }";

    String evalFunction = "callFromC(" + func + ");";
    QWebEngineCallback<const QVariant&> resultCallback;
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction), [this, property, actionName, iconMap](const QVariant& v) {
        bool isDebug = !v.toBool();
        GetAction(actionName)->setIcon(iconMap[isDebug]);
        String func = "function(){\
            const { WE } = window;\
            WE." + property + " = " + StringConverter::toString(isDebug) + "; \
        }";
        String evalFunction = "callFromC(" + func + ");";
        LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
    });
}

void CWeatherLabMenu::actionPick()
{
    mIsPicking = !mIsPicking;
    GetAction("actionPick")->setIcon(mPickIcon[mIsPicking]);

    String func;
    if (mIsPicking)
    {
        func = "function(){\
        const { WE } = window;\
        WE.handlerManager.startPick({\
        handleType:'CommonPick',\
        color : Cesium.Color.RED,\
        });\
        }";
    }
    else
    {
        func = "function(){\
        const { WE } = window;\
        WE.handlerManager.stop();\
        }";
    }


    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::actionShowTimeline()
{
    String func = "function(){\
        const { WE } = window;\
        WE.showTimeLine = !WE.showTimeLine;\
    }";

    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::actionSnow()
{
    String func = "function(){\
        const { WE } = window;\
        WE.weatherSystem.snow = !WE.weatherSystem.snow;\
    }";

    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::actionRain()
{
    String func = "function(){\
        const { WE } = window;\
        WE.weatherSystem.rain = !WE.weatherSystem.rain;\
    }";

    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::actionFog()
{
    String func = "function(){\
        const { WE } = window;\
        WE.weatherSystem.fog = !WE.weatherSystem.fog;\
    }";

    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::actionSun()
{
    String func = "function(){\
        const { WE } = window;\
        WE.weatherSystem.rain = false;\
        WE.weatherSystem.fog = false;\
        WE.weatherSystem.snow = false;\
    }";

    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}


void CWeatherLabMenu::actionTerrainLight()
{
    String func = "function(){\
        const { WE } = window;\
        WE.viewer.scene.globe.enableLighting = !WE.viewer.scene.globe.enableLighting;\
    }";

    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::actionTravelAround()
{
    String func = "function(){\
        const { WE } = window;\
        WE.travelWorld();\
    }";

    String evalFunction = "callFromC(" + func + ");";
    LabViewer::instance()->getWebView()->page()->runJavaScript(TOQSTRING(evalFunction));
}

void CWeatherLabMenu::actionConvertKML()
{
    String lastOpenDir = AppCfg.getStringValue(CONFIG_actionConvertKML, ToStdString(QCoreApplication::applicationDirPath()));
    QStringList qfilename = QFileDialog::getOpenFileNames(LabViewer::instance(), "选择文件", TOQSTRING(lastOpenDir), tr("radarstation thunder files(*.*)"));
    if (qfilename.isEmpty())
    {
        return;
    }

    String filename = ToStdString(qfilename[0]);
    AppCfg.setConfig(CONFIG_actionConvertKML, filename);
    AppCfg.save();

    String ext = osgDB::getLowerCaseFileExtension(filename);
    String outFilename = StringUtil::replaceAll(filename,"." + ext,".kml");
    if (ext == "thunder")
    {
        thunderToKML(filename, outFilename);
    }
    else if (ext == "radarstation")
    {
        QStringList::iterator it = qfilename.begin();
        for (; it != qfilename.end(); it++)
        {
            QString qfile = *it;
            String filename = ToStdString(qfile);
            outFilename = StringUtil::replaceAll(filename, "." + ext, "_local.kml");
            stationToKML(filename, outFilename, false);
            outFilename = StringUtil::replaceAll(filename, "." + ext, "_nation.kml");
            stationToKML(filename, outFilename, true);
        }

    }
}

void CWeatherLabMenu::actionConvertObj()
{
    String lastOpenDir = AppCfg.getStringValue(CONFIG_OConv, ToStdString(QCoreApplication::applicationDirPath()));
    QString qfilename = QFileDialog::getOpenFileName(LabViewer::instance(), "选择文件", TOQSTRING(lastOpenDir), tr("OSGB,OBJ,DAE,IVE,IFC files(*.*)"));
    if (qfilename.isNull())
    {
        return;
    }

    String filename = ToStdString(qfilename);
    AppCfg.setConfig(CONFIG_OConv, filename);
    AppCfg.save();

    String param = "{\"function\":\"offset2origin\",\"filename\":\"" + filename;

#ifdef HAS_UI
    if (IGUIArchive* guiarchive = Singleton(CCoreGUI).get_GUIArcvhie("commonui"))
    {
        CVector3 LonLatHei;
        if (guiarchive->getVector3(LonLatHei, U2A(L"偏移(整数)"), "X", "Y", "Z"))
        {
            param += "\",\"offset\":\"" + StringConverter::toString(LonLatHei);
        }
    }
#endif

    param += "\"}";

    invokeOConv(param);

    LabViewer::instance()->showMessageBox("完成");
}

inline String appendGltfConfig(OC::Cesium::gltfConfig templateConfig)
{
    String ret = "";
    if (templateConfig.enableDracoCompress)
    {
        ret += ",\"enableDracoCompress\":true";
    }
    if (templateConfig.enableWebpCompress)
    {
        ret += ",\"enableWebpCompress\":true";
    }
    if (templateConfig.mergeRepeatTexture)
    {
        ret += ",\"mergeRepeatTexture\":true";
    }

    return ret;
}

void CWeatherLabMenu::actionObj23dtiles()
{
#ifdef HAS_UI
    String lastOpenDir = AppCfg.getStringValue(CONFIG_OConv, ToStdString(QCoreApplication::applicationDirPath()));
    QString qfilename = QFileDialog::getOpenFileName(LabViewer::instance(), "选择文件", TOQSTRING(lastOpenDir), tr("OSGB,OBJ,DAE,IVE,IFC files(*.*)"));
    if (qfilename.isNull())
    {
        return;
    }

    String filename = ToStdString(qfilename);
    AppCfg.setConfig(CONFIG_OConv, filename);
    AppCfg.save();

    String param = "{\"function\":\"obj23dtiles\",\"filename\":\"" + filename;

    if (IGUIArchive* guiarchive = Singleton(CCoreGUI).get_GUIArcvhie("commonui"))
    {
        CVector3 LonLatHei;
        if (guiarchive->getVector3(LonLatHei, U2A(L"经纬度"), "X", "Y", "Z"))
        {
            param += "\",\"offset\":\"" + StringConverter::toString(LonLatHei) + "\"";
        }

        if(osgDB::getLowerCaseFileExtension(filename) != "ifc")
            param += ",\"localOffset\":true";
    }
    
    OC::Cesium::gltfConfig templateConfig;
    templateConfig.enableWebpCompress = true;
    templateConfig.enableDracoCompress = osgDB::getLowerCaseFileExtension(filename) != "ifc";
    templateConfig.mergeRepeatTexture = true;
    param += appendGltfConfig(templateConfig);

    param += "}";

    invokeOConv(param);

    LabViewer::instance()->showMessageBox("完成");
#endif
}

void CWeatherLabMenu::actionConvertOblique()
{
#ifdef HAS_UI
    String _ini = CONFIG_OConv;
    _ini += "_oblique";
    String lastOpenDir = AppCfg.getStringValue(_ini, ToStdString(QCoreApplication::applicationDirPath()));
    QString qfilename = QFileDialog::getExistingDirectory(LabViewer::instance(), "选择文件夹", TOQSTRING(lastOpenDir));
    if (qfilename.isNull())
    {
        return;
    }

    String filename = ToStdString(qfilename);
    AppCfg.setConfig(_ini, filename);
    AppCfg.save();


    String param = "{\"function\":\"oblique23dtiles\",\"filename\":\"" + filename + "\"";

    if (IGUIArchive* guiarchive = Singleton(CCoreGUI).get_GUIArcvhie("commonui"))
    {
        CVector3 LonLatHei;
        if (guiarchive->getVector3(LonLatHei, U2A(L"经纬度"), "X", "Y", "Z"))
        {
            param += ",\"offset\":\"" + StringConverter::toString(LonLatHei) + "\"";
        }

        param += ",\"maxTileNodeNum\":32";
        param += ",\"maxRootTileNodeNum\":128";
        param += ",\"mergeTop\":true";
        param += ",\"disableLight\":true";
    }


    OC::Cesium::gltfConfig templateConfig;
    templateConfig.enableWebpCompress = true;
    templateConfig.enableDracoCompress = false;
    templateConfig.mergeRepeatTexture = false;
    param += appendGltfConfig(templateConfig);

    param += "}";

    invokeOConv(param);

    LabViewer::instance()->showMessageBox("完成");
#endif
}

void CWeatherLabMenu::actionConvertGltf()
{
    String _ini = CONFIG_OConv;
    _ini += "_gltf";
    String lastOpenDir = AppCfg.getStringValue(_ini, ToStdString(QCoreApplication::applicationDirPath()));
    QString qfilename = QFileDialog::getOpenFileName(LabViewer::instance(), "选择文件", TOQSTRING(lastOpenDir), tr("OSGB,OBJ,DAE,IVE,IFC files(*.*)"));
    if (qfilename.isNull())
    {
        return;
    }

    String filename = ToStdString(qfilename);
    AppCfg.setConfig(_ini, filename);
    AppCfg.save();


    String param = "{\"function\":\"obj2gltf\",\"filename\":\"" + filename + "\"";

    OC::Cesium::gltfConfig templateConfig;
    templateConfig.enableWebpCompress = true;
    templateConfig.enableDracoCompress = false;
    templateConfig.mergeRepeatTexture = false;
    param += appendGltfConfig(templateConfig);

    param += "}";

    invokeOConv(param);

    LabViewer::instance()->showMessageBox("完成");
}


void CWeatherLabMenu::actionConvertShp()
{
    //String _ini = CONFIG_OConv;
    //_ini += "_shp";
    //String lastOpenDir = AppCfg.getStringValue(_ini, ToStdString(QCoreApplication::applicationDirPath()));
    //QString qfilename = QFileDialog::getOpenFileName(LabViewer::instance(), "选择文件", TOQSTRING(lastOpenDir), tr("shp files(*.shp)"));
    //if (qfilename.isNull())
    //{
    //    return;
    //}

    //String filename = ToStdString(qfilename);
    //AppCfg.setConfig(_ini, filename);
    //AppCfg.save();
}

void CWeatherLabMenu::actionConvertPipe()
{
    String _ini = CONFIG_OConv;
    _ini += "_pipe";
    String lastOpenDir = AppCfg.getStringValue(_ini, ToStdString(QCoreApplication::applicationDirPath()));
    QString qfilename = QFileDialog::getOpenFileName(LabViewer::instance(), "选择文件", TOQSTRING(lastOpenDir), tr("shp files(*.shp)"));
    if (qfilename.isNull())
    {
        return;
    }

    String filename = ToStdString(qfilename);
    AppCfg.setConfig(_ini, filename);
    AppCfg.save();

    filename = osgDB::getNameLessExtension(filename);

    if (!(StringUtil::endsWith(filename, "_NODE", false)
        || StringUtil::endsWith(filename, "_LINE", false)))
    {
        LabViewer::instance()->showMessageBox("文件名不正确");
        return;
    }

    filename = filename.substr(0, filename.length() - 5);

    String param = "{\"function\":\"pipe23dtiles\",\"filename\":\"" + filename + "\""
    + ",\"output\":\"" + filename + "\"";

    OC::Cesium::gltfConfig templateConfig;
    templateConfig.enableWebpCompress = false;
    templateConfig.enableDracoCompress = true;
    templateConfig.mergeRepeatTexture = false;
    param += appendGltfConfig(templateConfig);

    param += "}";

    invokeOConv(param);

    LabViewer::instance()->showMessageBox("完成");
}

void CWeatherLabMenu::actionExit()
{
    QApplication::exit();
}

void CWeatherLabMenu::actionOpen()
{
    String lastOpenDir = AppCfg.getStringValue(CONFIG_OPENDIR, QCoreApplication::applicationDirPath().toLocal8Bit().toStdString());

    QString qfilename = QFileDialog::getOpenFileName(LabViewer::instance(), "选择文件", TOQSTRING(lastOpenDir), tr("WeatherLab files(*.bz2;*.kml;*.json;*.vtk)"));
    if (qfilename.isNull())
    {
        return;
    }

    String filename = ToStdString(qfilename);

    AppCfg.setConfig(CONFIG_OPENDIR, filename);
    AppCfg.save();

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
        LabViewer::instance()->openVTK(filename);
    }
}