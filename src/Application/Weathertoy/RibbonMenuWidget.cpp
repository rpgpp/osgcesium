#include "RibbonMenuWidget.h"
#include <QFile>
#include <qdir>
#include <QMenu>
#include <QWidgetAction>
#include "OCMain/Environment.h"
#include "SARibbonBar/SARibbonBar.h"
#include "SARibbonBar/SARibbonCategory.h"
#include "SARibbonBar/SARibbonPannel.h"
#include "SARibbonBar/SARibbonToolButton.h"
#include "SARibbonBar/SARibbonMenu.h"
#include "SARibbonBar/SARibbonComboBox.h"
#include "SARibbonBar/SARibbonLineEdit.h"
#include "SARibbonBar/SARibbonGallery.h"
#include "SARibbonBar/SARibbonCheckBox.h"
#include "SARibbonBar/SARibbonQuickAccessBar.h"
#include "SARibbonBar/SARibbonButtonGroupWidget.h"
#include "LabViewer.h"
#include "WeatherLabMenu.h"

CRibbonMenuWidget::CRibbonMenuWidget(LabViewer *creator ,QWidget *parent, Qt::WindowFlags flags)
	: QWidget(parent, flags)
{
	mCreator = creator;
	mApplicationDir = TOQSTRING(Singleton(OC::Environment).getAppDir());
	SARibbonBar* ribbon = mCreator->ribbonBar();
	ribbon->setFont(QFont("Î¢ÈíÑÅºÚ"));
	ribbon->setApplitionButton(NULL);
	loadFileXml();
	loadToolXml();
}

CRibbonMenuWidget::~CRibbonMenuWidget()
{
}

void CRibbonMenuWidget::loadFileXml()
{
	
}

void CRibbonMenuWidget::loadToolXml()
{
	QString imagePath = mApplicationDir + "/image/";
	QFile file(":/res/menu.xml");
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		return;
	}

	QDomDocument document;
	if (!document.setContent(&file, false))
	{
		return;
	}

	if (document.isNull())
	{
		return;
	}

	QDomElement rootElement = document.documentElement();
	QDomElement pagesElement = rootElement.firstChildElement();
	if (pagesElement.isNull())
	{
		return;
	}

	SARibbonBar* ribbon = mCreator->ribbonBar();

	QDomElement page_element = pagesElement.firstChildElement("page");
	while (!page_element.isNull())
	{
		QString page_caption = page_element.attribute("caption");

		if (LabViewer::instance()->isSimpleMode() && page_caption != "¹¤¾ß")
		{
			page_element = page_element.nextSiblingElement();
			continue;
		}

		SARibbonCategory* page_category = ribbon->addCategoryPage(page_caption);
		/// 
		QDomElement group_element = page_element.firstChildElement();
		while (!group_element.isNull())
		{
			QString group_caption = group_element.attribute("caption");
			SARibbonPannel* group_pannel = page_category->addPannel(group_caption);
			group_pannel->setStyleSheet(QString::fromUtf8("font: 9pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));

			/// 
			QDomElement tool_element = group_element.firstChildElement();
			while (!tool_element.isNull())
			{
				QString tool_caption = tool_element.attribute("caption");
				QString iconPath = tool_element.attribute("icon");
				QString funcName = tool_element.attribute("function");
				QString enable = tool_element.attribute("enable");
				QString type = tool_element.attribute("type");

				QAction* pAction = mCreator->getWeatherLabMenu()->GetAction(funcName);
				if (pAction)
				{
					pAction->setIcon(QIcon(imagePath + iconPath));
					pAction->setText(tool_caption);
					if (type == "s")
					{
						group_pannel->addSmallAction(pAction);
					}
					else
					{
						group_pannel->addLargeAction(pAction);
					}
					if (enable == "false")
					{
						pAction->setEnabled(false);
					}
				}

				tool_element = tool_element.nextSiblingElement();
			}

			group_element = group_element.nextSiblingElement();
		}

		page_element = page_element.nextSiblingElement();
	}
}
