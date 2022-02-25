#ifndef OCAPP_H
#define OCAPP_H

#include <QMainWindow>
#include <QSplashScreen>
#include <QtWebEngineWidgets/QWebEngineView>
#include "WeatherLabMenu.h"
#include "ServerThread.h"
#include "SARibbonBar/SARibbonMainWindow.h"

class LabViewer : public SARibbonMainWindow
{
	Q_OBJECT
public:
	LabViewer(bool simple = false,QWidget *parent = 0);
	~LabViewer();

	OC::String mHostUrl;
	void setSize();
	void openRadar(OC::String filename);
	void openKML(OC::String filemane);
	void open3DTile(OC::String filename);
	void openVTK(OC::String filename);
	OC::String localToUrl(OC::String filename);
	void loadPNG(OC::String filename);
	void showMessageBox(QString text);
	QSplashScreen* mSplash;
public slots:
	QWebEngineView* getWebView();
	CWeatherLabMenu* getWeatherLabMenu();
	static LabViewer* instance();
	void openFolder(OC::String filename);
	void openExtenalFile(OC::String filename);
	bool isSimpleMode(){return mSimpleMode;}
	virtual bool eventFilter(QObject* obj, QEvent* e);
	void loadProgress(int progress);
	void loadFinished(bool ok);
protected:

	virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result);
private:
	int									mPort;
	bool								mSimpleMode;
	QString								mAppendJS;
	QWidget*							mRibbonMenuWidget;
	ServerThread*						mServerThread;
	QWebEngineView*						mWebView;
	CWeatherLabMenu*					mWeatherLabMenu;
	static LabViewer*					mInstrance;
	std::map<OC::String,ServerThread*>	mPathServerMap;
};

#endif // OCAPP_H
