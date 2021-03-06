#include "LabViewer.h"
#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QGridLayout>
#include <QScreen>
#include <QKeyEvent>
#include <QtWebChannel/QWebChannel>
#include <qbitmap>
#include <QThread>
#include <QtWebEngineCore/QWebEngineUrlRequestInterceptor>
#include <QtNetwork/QNetworkProxyFactory>
#include "LabWebEngineView.h"
#include "webuihandler.h"
#include "Bridge.h"

#include "OCServer/OCServer.h"
#include "ServerThread.h"
#include "SARibbonBar/SARibbonBar.h"
#include "RibbonMenuWidget.h"

class RequestInterceptor : public QWebEngineUrlRequestInterceptor
{
public:
    explicit RequestInterceptor(QObject* parent = Q_NULLPTR) : QWebEngineUrlRequestInterceptor(parent) {}
    virtual void interceptRequest(QWebEngineUrlRequestInfo& info) Q_DECL_OVERRIDE;
};

void RequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo& info)
{
    // Intercepting the requested URL
    QUrl url = info.requestUrl();
    qDebug() << "Request URL: " << url;

    // Optionally redirect the request URL but it only works for requests 
    // without payload data such as GET ones
    info.redirect(QUrl("https://www.baidu.com"));

    // Set HTTP header
    QByteArray httpHeaderName = "SomeHeaderName";
    QByteArray httpHeaderValue = "SomeHeaderValue";
    info.setHttpHeader(httpHeaderName, httpHeaderValue);

}
using namespace OC;

LabViewer* LabViewer::mInstrance = NULL;


#define ADD_ACTION(name)connect(AddQAction(#name), SIGNAL( triggered() ), this, SLOT( name() ) )

LabViewer::LabViewer(bool simple, QWidget* parent)
    : SARibbonMainWindow(parent)
    , mSplash(NULL)
    , mSimpleMode(simple)
    , mWeatherLabMenu(NULL)
    , mServerThread(NULL)
{
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING","10125");
    mInstrance = this;
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/res/app.png"), QSize(), QIcon::Normal, QIcon::Off);
    setWindowIcon(icon);

    mWebView = new LabWebEngineView(this);
    mWebView->setContextMenuPolicy(Qt::NoContextMenu);
    QWebChannel* channel = new QWebChannel(this);
    channel->registerObject("Bridge", (QObject*)Bridge::instance());

    setSize();

    String url;

    QWebEngineProfile* profile = NULL;
    String title = "Weathertoy";

    //HttpServer
    QTime current_time = QTime::currentTime();
    int hour = current_time.hour();//当前的小时
    int minute = current_time.minute();//当前的分
    int second = current_time.second();//当前的秒
    mPort = (80 + hour + minute) * 100 + second + 10000;
    mServerThread = new ServerThread(mPort);
    String disk = "cdefghijkl";
    for (int i = 0; i < 10; i++)
    {
        char d = disk[i];
        String uriPrefix = String("/") + d + String("/");
        mServerThread->server().registerALocation(uriPrefix, d + String(":"));
    }
    mServerThread->start();
    while (!mServerThread->server().isRunning())
    {
        Sleep(100);
    }

    if (mSimpleMode)
    {
        WebUiHandler::registerUrlScheme();
        profile = new QWebEngineProfile(profile);
        profile->installUrlSchemeHandler(WebUiHandler::schemeName, new WebUiHandler);
        url = "webui:main";
    }
    else
    {
        //profile = new QWebEngineProfile;
        WebUiHandler::registerUrlScheme();
        profile = new QWebEngineProfile(profile);
        profile->installUrlSchemeHandler(WebUiHandler::schemeName, new WebUiHandler);

        title += ":" + StringConverter::toString(mPort);
        mHostUrl = "http://localhost:" + StringConverter::toString(mPort);
        url = mHostUrl + "/Weathertoy/index.html#/" + Singleton(ConfigManager).getStringValue("submodule");
    }

    QWebEnginePage* page = new QWebEnginePage(profile, mWebView);
    connect(page, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    connect(page, SIGNAL(loadProgress(int)), this, SLOT(loadProgress(int)));
    mWebView->setPage(page);
    mWebView->page()->setWebChannel(channel);
    //page->setUrlRequestInterceptor(new RequestInterceptor());
    page->load(QUrl(url.c_str()));

    setCentralWidget(mWebView);
    setWindowTitle(TOQSTRING(title));

    this->centralWidget()->setContentsMargins(0, 0, 0, 0);
    this->layout()->setContentsMargins(0, 0, 0, 0);
    mWeatherLabMenu = new CWeatherLabMenu;
    mRibbonMenuWidget = new CRibbonMenuWidget(this);

    
    String resuri = localToUrl(Singleton(Environment).getAppDir());

    QString init3d = QString("window.ResUri = '") + TOQSTRING(resuri) + "';";
    mAppendJS = mWeatherLabMenu->getAppend() + init3d;
    String script = Singleton(ConfigManager).getStringValue("script");
    if (!script.empty())
    {
        String filename = Singleton(Environment).getAppDir() + "config/" + script + ".script";
        if (FileUtil::FileExist(filename))
        {
            QFile file;
            file.setFileName(TOQSTRING(filename));
            file.open(QIODevice::ReadOnly);
            mAppendJS.append(file.readAll());
            file.close();
        }
    }
    mAppendJS += " init3d();";

    if (Singleton(OC::ConfigManager).getStringValue("MapCacheDir").empty())
    {
        Singleton(OC::ConfigManager).setConfig("MapCacheDir", Singleton(Environment).getTempDir());
    }
}


LabViewer::~LabViewer()
{
   CORE_SAFE_DELETE(mWeatherLabMenu);
   if (mServerThread)
   {
       mServerThread->server().quit();
       mServerThread->exit();
       int count = 0;
       while (mServerThread->isRunning() && count < 25)
       {
           Sleep(200);
       }
       CORE_SAFE_DELETE(mServerThread);
   }
}

void LabViewer::loadFinished(bool ok)
{
    if (mSplash)
    {
        mSplash->finish(this);
        CORE_SAFE_DELETE(mSplash);
    }

    LabViewer::instance()->getWebView()->page()->runJavaScript(mAppendJS);
    this->setVisible(true);
}

void LabViewer::loadProgress(int progress)
{
}

bool LabViewer::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    if (mSimpleMode)
    {
        return false;
    }

    Q_UNUSED(eventType);
    const int HIT_BORDER = 5;
    const int HIT_HEADER = 30;
    const MSG* msg = static_cast<MSG*>(message);
    switch(msg->message)
    {
    case WM_NCHITTEST:
    {
        int xPos = ((int)(short)LOWORD(msg->lParam)) - this->frameGeometry().x();
        int yPos = ((int)(short)HIWORD(msg->lParam)) - this->frameGeometry().y();
        if (yPos > -5 && yPos < HIT_BORDER) {
            *result = HTTOP;
            return true;
        }
        if (yPos < HIT_HEADER && xPos < this->width() - 80) {
            *result = HTCAPTION;
            return true;
        }
    }
        break;
    case WM_NCLBUTTONDBLCLK:
    {
        int xPos = ((int)(short)LOWORD(msg->lParam)) - this->frameGeometry().x();
        int yPos = ((int)(short)HIWORD(msg->lParam)) - this->frameGeometry().y();
        if (yPos < 25) {
            this->isMaximized()?this->showNormal():this->showMaximized();
            return true;
        }
    }
        break;
    default:
        break;
    }

    return false;
}

QWebEngineView* LabViewer::getWebView()
{
    return mWebView;
}

CWeatherLabMenu* LabViewer::getWeatherLabMenu()
{
    return mWeatherLabMenu;
}

LabViewer* LabViewer::instance()
{
    return mInstrance;
}

void LabViewer::openFolder(OC::String filename)
{
    StringVector sv;
    FileUtil::FindFiles(filename, sv);
    int size = (int)sv.size();
    if (size < 1)
    {
        return;
    }
    StringStream sstream;
    String file = sv[0];
    String ext = FileUtil::getLowerCaseFileExtension(file);
    sstream << "const data = ['" << file;
    for (int i = 1; i < size; i++)
    {
        file = sv[i];
        String fext = FileUtil::getLowerCaseFileExtension(file);
        if (fext != ext)
        {
            showMessageBox("异常:文件夹包含不同类型文件");
            return;
        }
        sstream << "','" << file;
    }
    sstream << "'];";

    String evalFunction = sstream.str() + String("loadSome_" + ext + "(data);");
    mWebView->page()->runJavaScript(TOQSTRING(evalFunction));
}

void LabViewer::openExtenalFile(OC::String filename)
{
    String ext = FileUtil::getLowerCaseFileExtension(filename);
    filename = osgDB::convertFileNameToUnixStyle(filename);
    if (FileUtil::FileExist(filename))
    {
        String evalFunction = String("load_") + ext + String("('");
        evalFunction += filename;
        evalFunction += "');";
        mWebView->page()->runJavaScript(TOQSTRING(evalFunction));
    }
}

void LabViewer::openVTK(OC::String filename)
{
    filename = osgDB::convertFileNameToUnixStyle(filename);
    if (FileUtil::FileExist(filename))
    {
        String evalFunction = String("loadvtk('");
        evalFunction += filename;
        evalFunction += "');";
        mWebView->page()->runJavaScript(TOQSTRING(evalFunction));
    }
}

OC::String LabViewer::localToUrl(String filename)
{
    filename = osgDB::convertFileNameToUnixStyle(filename);
    StringUtil::toLowerCase(filename);
    filename = StringUtil::replaceAll(filename, ":", "");
    return mHostUrl + "/" + filename;
}

void LabViewer::loadPNG(OC::String filename)
{
    filename = osgDB::convertFileNameToUnixStyle(filename);
    if (FileUtil::FileExist(filename))
    {
        String evalFunction = String("loadPNG('");
        evalFunction += localToUrl(filename);
        evalFunction += "');";
        mWebView->page()->runJavaScript(TOQSTRING(evalFunction));
    }
}

void LabViewer::openRadar(OC::String filename)
{
    filename = osgDB::convertFileNameToUnixStyle(filename);
    String name = osgDB::getNameLessExtension(osgDB::getSimpleFileName(filename));
    String varname = "DBZ";
    StringVector names = StringUtil::split(osgDB::getNameLessAllExtensions(name), "_");
    for (auto str : names)
    {
        if (str == "SA" || str == "SB" || str == "CA" || str == "FMT")
        {
            varname = "EF6A0A74";
            break;
        }
    }

    if (FileUtil::FileExist(filename))
    {
        String json = "{";
        json += "\"name\":\"" + name + "\",";
        json += "\"filename\":\"" + filename + "\",";
        json += "\"varname\":\"" + varname + "\",";
        json += "\"ext\":\"" + osgDB::getFileExtension(filename) + "\"";
        json += "}";

        String evalFunction = String("loadRadar('");
        evalFunction += json;
        evalFunction += "');";

        mWebView->page()->runJavaScript(TOQSTRING(evalFunction));
    }
}

void LabViewer::openKML(OC::String filename)
{
    String param1 = mHostUrl;
    String param2 = filename;
    String evalFunction = String("loadKML(");
    evalFunction += "'" + param1 + "'";
    evalFunction += ",'" + param2 + "'";
    evalFunction += ");";
    mWebView->page()->runJavaScript(TOQSTRING(evalFunction));
}

void LabViewer::open3DTile(OC::String filename)
{
    filename = osgDB::convertFileNameToUnixStyle(filename);
    StringUtil::toLowerCase(filename);
    filename = StringUtil::replaceAll(filename,":","");
    String param = localToUrl(filename);
    String evalFunction = String("load3DTiles(");
    evalFunction += "'" + param + "'";
    evalFunction += ");";
    mWebView->page()->runJavaScript(TOQSTRING(evalFunction));
}

bool LabViewer::eventFilter(QObject* target, QEvent* e)
{
    //if (target == getWebView())
    {
        if (e->type() == QEvent::KeyPress) 
        {

            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);
            if (keyEvent->key() == Qt::Key_Space) 
            {
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(target, e);
}

void LabViewer::setSize()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect mm = screen->availableGeometry();
    int screen_width = mm.width() * 0.7;
    int screen_height = mm.height() * 0.8;

    if (mSimpleMode)
    {
        screen_width *= 0.5;
        screen_height *= 0.5;
    }

    resize(screen_width, screen_height);
}

void LabViewer::showMessageBox(QString text)
{
    if (isSimpleMode())
    {
        QMessageBox message;
        message.setWindowFlags(Qt::Drawer);
        message.setWindowTitle("WeatherLab");
        message.setText(text);
        message.exec();
    }
    else
    {
        String evalFunction = String("showMessageBox('");
        evalFunction += ToStdString(text);
        evalFunction += "');";
        mWebView->page()->runJavaScript(TOQSTRING(evalFunction));
    }

}
