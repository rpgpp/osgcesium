#include "gltfAttributeCallback.h"
#include "OCUtility/StringConverter.h"

namespace OC
{
	namespace Cesium
	{
		gltfAttributeCallback::gltfAttributeCallback()
		{

		}

		gltfAttributeCallback::~gltfAttributeCallback()
		{

		}

		void gltfAttributeCallback::readFile(String filename)
		{
			ifstream ifs(filename.c_str());
			if(ifs)
			{
				String line;
				while (getline(ifs, line))
				{
					if (line.empty())
					{
						break;
					}

					StringVector sv = StringUtil::split(line,",");
					
					if (sv.size() > 1)
					{
						if (!header.valid())
						{
							header = new OCHeaderInfo();
							for (int i = 1; i < sv.size(); i++)
							{
								String hd = sv[i];

								OC_DATATYPE type = OC_DATATYPE_TEXT;
								StringVector hsv = StringUtil::split(hd,":");
								if (hsv.size() == 2)
								{
									hd = hsv[1];
									if(hsv[0] == "int")
										type = OC_DATATYPE_INTEGER;
									else if (hsv[0] == "float")
										type = OC_DATATYPE_FLOAT;
								}
								header->add(new OCFieldInfo(hd, type));
							}
						}
						else if(header->getFieldNum() == sv.size() - 1)
						{
							String key = sv[0];
							if (!mAttributeMap[key].valid())
							{
								mAttributeMap[key] = new TemplateRecord(header);
							}
							for (int i = 1; i < sv.size(); i++)
							{
								String fn = header->getFieldInfo(i-1)->Name;
								mAttributeMap[key]->setFieldValue(fn,sv[i]);
							}
						}
					}
				}
				ifs.close();
			}
		}

		void gltfAttributeCallback::onSetAttirubte(String name, Cesium::FeatureBatchTable* table)
		{
			if (header.valid())
			{
				for (int i = 0; i < header->getFieldNum(); i++)
				{
					OCFieldInfo* filedinfo = header->getFieldInfo(i);
					String fn = filedinfo->Name;
					String fv = "";

					if (mAttributeMap[name].valid())
					{
						fv = mAttributeMap[name]->getFieldValue(fn);
					}

					if (fn == "[id]")
					{
						fn = fn.substr(1,2);
						fv = StringUtil::GenGUID().substr(0, 6);
					}
					else if (fn == "[name]")
					{
						fn = fn.substr(1, 4);
						fv = name;
					}

					if (filedinfo->TYPE == OC_DATATYPE_INTEGER)
					{
						table->pushAttribute(fn, (int)StringConverter::parseInt(fv));
					}
					else if (filedinfo->TYPE == OC_DATATYPE_FLOAT)
					{
						table->pushAttribute(fn, (float)StringConverter::parseReal(fv));
					}
					else
					{
						table->pushAttribute(fn, fv);
					}
				}
			}
		}

}}//namespcae