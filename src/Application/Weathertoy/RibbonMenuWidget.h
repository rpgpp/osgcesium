#ifndef TESTRIBBON_H
#define TESTRIBBON_H

#include <QtWidgets>
#include <QtXml/QDomElement>

class LabViewer;
class CRibbonMenuWidget : public QWidget
{
	Q_OBJECT
public:
	CRibbonMenuWidget(LabViewer *createor, QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~CRibbonMenuWidget();
public:
private:
	void loadFileXml();
	void loadToolXml();
private:
	QString			mApplicationDir;
	LabViewer*      mCreator;
};

#endif // TESTRIBBON_H
