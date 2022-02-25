#ifndef _OC_LAYER_DEFINE_H__
#define _OC_LAYER_DEFINE_H__

#if defined(_MSC_VER)
#ifndef OCLayer_EXPORTS
#define _LayerExport  __declspec(dllimport)
#else
#define _LayerExport  __declspec(dllexport)
#endif
#else
#define _LayerExport
#endif

#include "OCUtility/StringConverter.h"
#include "OCUtility/FileUtil.h"
#include "OCUtility/tinyxml.h"
#include "OCMain/osg.h"
#include "OCMain/ConfigManager.h"
#include "OCMain/Environment.h"
#include "OCMain/DefaultWorkQueueStandard.h"
#include "OCMain/Exception.h"

namespace OC
{
	class CacheManager;
	class CEventLayer;
	class CEffectLayer;
	class CFeature;
	class CFeatureInfo;
	class CFeatureLod;
	class CFeatureLayer;
	class CGroupLayer;	
	class OCProject;
	class CImageLayer;	
	class CLabelLayer;
	class CLayerConfig;
	class CLayerCacher;
	class CMaskLayer;
	class CMovable;
	class CMovableTracer;
	class CMovableRouteManager;
	class CNormalWaveNode;
	class COELayer;
	class COceanLayer;
	class COverLayLayer;
	class CQuadTree;
	class CRoleNode;
	class CRegionEvent;
	class CSkyBox;
	class CSceneNode;
	class CTerrainLayer;
	class CTileFeatureNode;
	class CVectorLayer;
	class CVectorNode;
	class CVolumeLayer;
	class FeatureManager;
	class GlobalFiledFilter;
	class JsonHelper;
	class IArchive;
	class ILayer;
	class LayerManager;
	class LayerManagerListener;
	class LayerManagerWorkqueue;
	class ShadowedSceneLayerNode;
	
	class CFeatureEffect_AttachNode;
	class CFeatureEffectFactory;
	class CFeatureEffect_Flash;
	class CFeatureEffect_Transparent;
	class CFeatureEffect_ScaleAnimation;
	class CFeatureEffect_TextureAnimation;

	class CoreTranslateDragger;
	class CoreTranslate1DDragger;
	class CoreTranslate2DDragger;
	class CoreRotateDragger;
	class CoreRotateCylinderDragger;
	class CoreDraggerTransformCallback;
	
	class ViewInfo;

	class IScene;
	class ISceneImp;


	struct LabelParamFromJson
	{
		int			FontSize;
		bool		Pickable;
		bool		DeclutteringEnable;
		String		LabelID;
		String		LayerID;
		String		Url;
		String		Text;
		String		Type;
		String		BubbleTipStyle;
		double		IconScale;
		double		Opacity;
		double		VisibleDistance;
		CVector3	Offset;
		ColourValue	FontColor;
		ColourValue	BorderColor;
		ColourValue	BackGroundColor;
	};
}

#define OC_LAYER_MASK "MaskLayer"
#define OC_LAYER_GROUP "DB"
#define OC_LAYER_LABEL "LabelLayer"
#define OC_LAYER_IMAGE "ImageLayer"
#define OC_LAYER_OCEAN "OceanLayer"
#define OC_LAYER_VECTOR "VectorLayer"
#define OC_LAYER_FEATURE "ModelLayer"
#define OC_LAYER_TERRAIN "DEMLayer"
#define OC_LAYER_OVERLAY "OverlayLayer"
#define OC_LAYER_OE "OE"
#define OC_LAYER_VOLUME "VolumeLayer"

#endif