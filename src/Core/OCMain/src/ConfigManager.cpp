#include "ConfigManager.h"
#include "Environment.h"

namespace OC
{
#if SingletonConfig == SingletonNormal
	template<>ConfigManager* CSingleton<ConfigManager>::msSingleton = NULL;
#elif SingletonConfig == SingletonProcess
	template<>std::map<int,ConfigManager*> CSingleton<ConfigManager>::msPidton;
#endif
	//////////////////////////////////////////////////////////////////////////
	ConfigManager::ConfigManager(String filename)
		:mConfigFileName(filename)
		,mDotINI(NULL)
	{
		fromFile(filename);
	}

	ConfigManager::~ConfigManager(void)
	{
		CORE_SAFE_DELETE(mDotINI);
	}

	String ConfigManager::NameFont = "DefaultFont";
	String ConfigManager::NameKdTree = "KdTree";
	String ConfigManager::NameFontSize = "FontSize";
	String ConfigManager::NameShowLogo = "ShowLogo";
	String ConfigManager::NameUseSkyLight = "UseSkyLight";
	String ConfigManager::NameProjectedHome = "Projected";
	String ConfigManager::NameLayerThreadNum = "LayerThreadNum";
	String ConfigManager::NameUseSingleThread = "UseSingleThread";
	String ConfigManager::NameModelThreadNum = "ModelThreadNum";
	String ConfigManager::NameDrawablePolicy = "DrawablePolicy";
	String ConfigManager::NameTerrainOpacity = "TerrainOpacity";
	String ConfigManager::NameScrollNagertive = "ScrollNagertive";
	String ConfigManager::NameTerrainLighting = "terrainLigting";
	String ConfigManager::NameMouseSensitivity = "MouseSensitivity";
	String ConfigManager::NameAllowUnderGround = "AllowUnderGround";
	String ConfigManager::NameShowTerrainToolbar = "TerrainUI";
	String ConfigManager::NameScaleAnimationParam = "ScaleAnimeParams";
	String ConfigManager::NameAlwaysDownloadCache = "AlwaysDownloadCache";
	String ConfigManager::NameTerrainPolygonOffset = "TerrainOffset";
	String ConfigManager::NameMaximumNumberOfPageLOD = "MaxinumLod";
	String ConfigManager::NameDefaultVisibleDistance = "FeatureRange";
	String ConfigManager::NameVectorDrawPolygonOffset = "Vector::MinMaxBias_MinMaxRange";
	
	int ConfigManager::getDefaultFontSize()
	{
		return getIntValue(NameFontSize,16);
	}

	double ConfigManager::getMouseSensitivity()
	{
		return getDoubleValue(NameMouseSensitivity,1.0);
	}

	String ConfigManager::getDefaultFont()
	{
		String fontName = getStringValue(NameFont,String("msyh.ttf"));

		String file,ext,path;
		StringUtil::splitFullFilename(mConfigFileName,file,ext,path);

		String innerFont = path + "/../datafiles/fonts/" + fontName;

		if (FileUtil::FileExist(innerFont))
		{
			return innerFont;
		}

		return String("fonts/") + fontName;
	}

	ColourValue ConfigManager::getAmbient()
	{
		return getColourValue("Ambient",ColourValue(1.0f,1.0f,1.0f,1.0f));
	}

	ColourValue ConfigManager::getDiffuse()
	{
		return getColourValue("Diffuse",ColourValue(0.8f,0.8f,0.8f,0.8f));
	}

	ColourValue ConfigManager::getSpecular()
	{
		return getColourValue("Specular",ColourValue(1.0f,1.0f,1.0f,1.0f));
	}

	int ConfigManager::getDefaultVisibleDistance()
	{
		return getIntValue(NameDefaultVisibleDistance,5000);
	}

	int ConfigManager::getSamples()
	{
#ifdef _DEBUG
		return 1;
#else
		return getIntValue("MultiSamples",1);
#endif
	}

	CVector4 ConfigManager::getVectorDrawPolygonOffset()
	{
		return getVector4Value(NameVectorDrawPolygonOffset,CVector4(0.08,1e+3,1e+4,1e+6));
	}

	void ConfigManager::setServiceAddress(String address)
	{
		if (!StringUtil::startsWith(address,"http://"))
		{
			address = "http://" + address;
		}

		setConfig("Server",address);
	}
	
	String ConfigManager::getServiceAddress()
	{
		String server = getStringValue("Server",String("http://127.0.0.1:8080"));
		if (!StringUtil::startsWith(server,"http://"))
		{
			server = "http://" + server;
		}
		return server;
	}

	CCoreConfig& ConfigManager::ApplicationINI()
	{
		if (mDotINI == NULL)
		{
			mDotINI = new CCoreConfig;
			String file = "oc.ini";
			if (SingletonPtr(Environment))
			{
				file = Singleton(Environment).getTempDir() + file;
			}
			mDotINI->fromFile(file);
		}
		return *mDotINI;
	}

	CVector3 ConfigManager::getScaleAnimeParams()
	{
		return getVector3Value(NameScaleAnimationParam,CVector3(1.6,2.0,0));
	}
}
