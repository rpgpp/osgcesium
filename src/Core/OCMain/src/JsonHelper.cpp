#include "JsonHelper.h"

namespace OC
{
	JsonHelper::JsonHelper(void)
	{
	}

	JsonHelper::~JsonHelper(void)
	{
	}

	osg::MixinVector<osg::Vec3d> JsonHelper::parseGeometryJson(String jsonStr)
	{
		return osg::MixinVector<osg::Vec3d>();
	}

	std::vector<std::pair<String,String> > JsonHelper::parseStringPairList(String jsonStr)
	{
		std::vector<std::pair<String,String> > list;

		try
		{
			Json::Reader reader;
			Json::Value root;
			if(reader.parse(jsonStr,root))
			{
				Json::Value ar = root["nameandvalues"];

				if (!ar.isNull())
				{
					int routeNum = ar.size();
					for (int i=0;i<routeNum;i++)
					{
						String n = JsonPackage::getString(ar[i],"n");	
						String v = JsonPackage::getString(ar[i],"v");	
						list.push_back(make_pair(n,v));
					}
				}
			}
		}
		catch(...)
		{

		}

		return list;
	}

	NameValuePairList JsonHelper::parseNameValuePairList(String jsonStr)
	{
		NameValuePairList list;

		try
		{
			Json::Reader reader;
			Json::Value root;
			if(reader.parse(jsonStr,root))
			{
				Json::Value ar = root["nameandvalues"];

				if (!ar.isNull())
				{
					int routeNum = ar.size();
					for (int i=0;i<routeNum;i++)
					{
						String n = JsonPackage::getString(ar[i],"n");	
						String v = JsonPackage::getString(ar[i],"v");	
						list[n] = v;
					}
				}
			}
		}
		catch(...)
		{
			
		}

		return list;
	}

	StringMap JsonHelper::parseStyleString(String styleStr)
	{
		StringMap sm;

		StringVector sv = StringUtil::split(styleStr,";");

		for(StringVector::iterator it = sv.begin();
			it!=sv.end();it++)
		{
			StringVector nv = StringUtil::split(*it,":");

			if (nv.size() == 2)
			{
				String n = nv[0];
				String v = nv[1];
				StringUtil::trim(n);
				StringUtil::toLowerCase(n);
				if( n != "text")
				{
					StringUtil::trim(v);
				}
				sm[n] = v;
			}
			else if (nv.size() > 2)
			{
				String n = nv[0];
				String v = nv[1];
				for (int i=2;i<(int)nv.size();i++)
				{
					v += ":" + nv[i];
				}
				StringUtil::toLowerCase(n);
				StringUtil::toLowerCase(v);
				sm[n] = v;
			}
		}

		return sm;
	}
}


