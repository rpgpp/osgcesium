#ifndef _OC_CORE_EVENTS_H__
#define _OC_CORE_EVENTS_H__

#include "OCommon/osg_headers.h"
#include "BaseDefine.h"

namespace OC
{
	class CSceneChangeListener : public osg::Referenced
	{
	public:
		CSceneChangeListener(){}
		virtual ~CSceneChangeListener(){}
		virtual void onSceneChange(String scene) = 0;
	};

	class CTerrainBlenderListener : public osg::Referenced
	{
	public:
		CTerrainBlenderListener(){}
		virtual ~CTerrainBlenderListener(){}
		virtual void onValueChange(double value) = 0;
	};

	class SceneInfoBarListener : public osg::Referenced 
	{
	public:
		SceneInfoBarListener(){}
		virtual ~SceneInfoBarListener(){}
		virtual void OnMove(){}
		virtual void OnThroughGround(bool underground){}
	};
}



#endif