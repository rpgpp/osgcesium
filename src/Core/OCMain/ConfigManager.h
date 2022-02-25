#ifndef _CONFIG_MANAGER_H__
#define _CONFIG_MANAGER_H__

#include "Singleton.h"
#include "CoreConfig.h"
#include "BaseDefine.h"

namespace OC
{
	class _MainExport ConfigManager : public CSingleton<ConfigManager>, public CCoreConfig
	{
	public:
		ConfigManager(String filename);
		~ConfigManager(void);

		int getSamples();
		int getDefaultFontSize();
		int getDefaultVisibleDistance();
		double getMouseSensitivity();
		String getDefaultFont();
		CVector3 getScaleAnimeParams();
		CVector4 getVectorDrawPolygonOffset();

		ColourValue getAmbient();
		ColourValue getDiffuse();
		ColourValue getSpecular();

		void setServiceAddress(String address);
		String getServiceAddress();

		
		static String NameFont;
		static String NameKdTree;
		static String NameFontSize;
		static String NameShowLogo;
		static String NameUseSkyLight;
		static String NameProjectedHome;
		static String NameDrawablePolicy;
		static String NameTerrainOpacity;
		static String NameModelThreadNum;
		static String NameLayerThreadNum;
		static String NameUseSingleThread;
		static String NameScrollNagertive;
		static String NameTerrainLighting;
		static String NameMouseSensitivity;
		static String NameAllowUnderGround;
		static String NameShowTerrainToolbar;
		static String NameAlwaysDownloadCache;
		static String NameScaleAnimationParam;
		static String NameTerrainPolygonOffset;
		static String NameDefaultVisibleDistance;
		static String NameMaximumNumberOfPageLOD;
		static String NameVectorDrawPolygonOffset;

		CCoreConfig& ApplicationINI();
	private:
		String			mConfigFileName;
		CCoreConfig*	mDotINI;
	};
}

#endif
