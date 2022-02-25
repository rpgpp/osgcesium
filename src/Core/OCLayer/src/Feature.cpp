#include "Feature.h"
#include "FeatureLayer.h"

namespace OC
{
	String CFeature::ClassGUID = "OC::Feature";

	CFeature::CFeature() 
	{

	}

	CFeature::~CFeature(void)
	{
		
	}

	void CFeature::prepareImpl(void)
	{
	}

	void CFeature::loadImpl(void)
	{
		load();
	}

	bool CFeature::load()
	{
		return true;
	}

}//namespace
