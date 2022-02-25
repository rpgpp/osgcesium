#include "RadarStationKML.h"
#include "OCMain/osg.h"
#include "OCMain/Common.h"
#include "OCUtility/tinyxml.h"
#include "OCUtility/StringUtil.h"
#include "OCUtility/StringConverter.h"

inline TiXmlElement* initStyleKML(TiXmlDocument& document,String icon)
{
    document.LinkEndChild(new TiXmlDeclaration("1.0", "UTF-8", ""));
    TiXmlElement* kmlElem = new TiXmlElement("kml");
    document.LinkEndChild(kmlElem);
    TiXmlElement* documentElem = new TiXmlElement("Document");
    kmlElem->LinkEndChild(documentElem);

    TiXmlElement* hotSpotElem = new TiXmlElement("hotSpot");
    hotSpotElem->SetAttribute("yunits", "pixels");
    hotSpotElem->SetAttribute("y", "0.5");

    TiXmlElement* styleElem = new TiXmlElement("Style");
    documentElem->LinkEndChild(styleElem);
    TiXmlElement* iconStyleElem = new TiXmlElement("IconStyle");
    styleElem->LinkEndChild(iconStyleElem);
    TiXmlElement* labelStyleElem = new TiXmlElement("LabelStyle");
    styleElem->LinkEndChild(labelStyleElem);

    TiXmlElement* iconElem = new TiXmlElement("Icon");
    iconStyleElem->LinkEndChild(iconElem);
    TiXmlElement* hrefElem = new TiXmlElement("href");
    iconElem->LinkEndChild(hrefElem);

    styleElem->SetAttribute("id", "style");
    TiXmlText* text = new TiXmlText(icon);
    hrefElem->LinkEndChild(text);

    {
        TiXmlElement* nearFarScalarElem = new TiXmlElement("scaleByDistance");
        nearFarScalarElem->SetAttribute("near", "5000000");
        nearFarScalarElem->SetAttribute("far", "15000000");
        iconStyleElem->LinkEndChild(hotSpotElem);
        iconStyleElem->LinkEndChild(nearFarScalarElem);
        TiXmlElement* distanceDisplayConditionElem = new TiXmlElement("distanceDisplayCondition");
        distanceDisplayConditionElem->SetAttribute("near", "0");
        distanceDisplayConditionElem->SetAttribute("far", "15000000");
        iconStyleElem->LinkEndChild(distanceDisplayConditionElem);
    }

    //label
    {
        TiXmlElement* nearFarScalarElem = new TiXmlElement("scaleByDistance");
        nearFarScalarElem->SetAttribute("near", "5000000");
        nearFarScalarElem->SetAttribute("far", "15000000");
        labelStyleElem->LinkEndChild(nearFarScalarElem);
        TiXmlElement* distanceDisplayConditionElem = new TiXmlElement("distanceDisplayCondition");
        distanceDisplayConditionElem->SetAttribute("near", "0");
        distanceDisplayConditionElem->SetAttribute("far", "15000000");
        labelStyleElem->LinkEndChild(distanceDisplayConditionElem);
    }

    return documentElem;
}

void stationToKML(String filename, String outFilename,bool isStation)
{
    std::ifstream ifs(filename.c_str());
    if (ifs)
    {
        TiXmlDocument document;
        document.LinkEndChild(new TiXmlDeclaration("1.0", "UTF-8", ""));
        TiXmlElement* kmlElem = new TiXmlElement("kml");
        document.LinkEndChild(kmlElem);
        TiXmlElement* documentElem = new TiXmlElement("Document");
        kmlElem->LinkEndChild(documentElem);


        TiXmlElement* hotSpotElem = new TiXmlElement("hotSpot");
        hotSpotElem->SetAttribute("yunits", "pixels");
        hotSpotElem->SetAttribute("y", "0.5");

        TiXmlElement* styleElem = new TiXmlElement("Style");
        documentElem->LinkEndChild(styleElem);
        TiXmlElement* iconStyleElem = new TiXmlElement("IconStyle");
        styleElem->LinkEndChild(iconStyleElem);
        TiXmlElement* labelStyleElem = new TiXmlElement("LabelStyle");
        styleElem->LinkEndChild(labelStyleElem);

        TiXmlElement* iconElem = new TiXmlElement("Icon");
        iconStyleElem->LinkEndChild(iconElem);
        TiXmlElement* hrefElem = new TiXmlElement("href");
        iconElem->LinkEndChild(hrefElem);
        if(!isStation)
        {
            styleElem->SetAttribute("id", "style1");
            TiXmlText* text = new TiXmlText("radar-station.png");
            hrefElem->LinkEndChild(text);
        }
        else
        {
            styleElem->SetAttribute("id", "style2");
            TiXmlText* text = new TiXmlText("radar-stationA.png");
            hrefElem->LinkEndChild(text);
        }

        {
            TiXmlElement* nearFarScalarElem = new TiXmlElement("scaleByDistance");
            nearFarScalarElem->SetAttribute("near", "500000");
            nearFarScalarElem->SetAttribute("far", "1500000");
            iconStyleElem->LinkEndChild(hotSpotElem);
            iconStyleElem->LinkEndChild(nearFarScalarElem);
            TiXmlElement* distanceDisplayConditionElem = new TiXmlElement("distanceDisplayCondition");
            distanceDisplayConditionElem->SetAttribute("near", "0");
            distanceDisplayConditionElem->SetAttribute("far", "1500000");
            iconStyleElem->LinkEndChild(distanceDisplayConditionElem);
        }

        //label
        {
            TiXmlElement* nearFarScalarElem = new TiXmlElement("scaleByDistance");
            nearFarScalarElem->SetAttribute("near", "500000");
            nearFarScalarElem->SetAttribute("far", "1500000");
            labelStyleElem->LinkEndChild(nearFarScalarElem);
            TiXmlElement* distanceDisplayConditionElem = new TiXmlElement("distanceDisplayCondition");
            distanceDisplayConditionElem->SetAttribute("near", "0");
            distanceDisplayConditionElem->SetAttribute("far", "1500000");
            labelStyleElem->LinkEndChild(distanceDisplayConditionElem);
        }

        StringVector headers;

        String line;
        while (getline(ifs, line))
        {
            StringVector sv = StringUtil::split(line, ",");
            if (sv.size() == 7)
            {
                if (headers.empty())
                {
                    headers = sv;
                    continue;
                }


                String code = sv[0];
                String station = sv[1];


                String lonStr = sv[2];
                String latStr = sv[3];
                String elevStr = sv[4];
                StringUtil::trim(lonStr);
                StringUtil::trim(latStr);
                StringUtil::trim(elevStr);
                double lon = StringConverter::parseReal(lonStr);
                double lat = StringConverter::parseReal(latStr);
                double hei = StringConverter::parseReal(elevStr);

                bool styleB = (code[0] > '0' && code[0] < '9');

                if (isStation != styleB)
                {
                    continue;
                }

                TiXmlElement* placeElem = new TiXmlElement("Placemark");
                TiXmlElement* styleUrlElem = new TiXmlElement("styleUrl");
                TiXmlElement* nameElem = new TiXmlElement("name");
                TiXmlText* styleUrlElemText = new TiXmlText(isStation ? "#style2" : "#style1");
                TiXmlText* nameElemText = new TiXmlText(A2U8(station));
                placeElem->LinkEndChild(nameElem);
                nameElem->LinkEndChild(nameElemText);
                placeElem->LinkEndChild(styleUrlElem);
                styleUrlElem->LinkEndChild(styleUrlElemText);

                String coord = StringConverter::formatDoubletoString(lon, 3) + "," + StringConverter::formatDoubletoString(lat, 3) + "," + StringConverter::formatDoubletoString(hei, 1);
                TiXmlElement* pointElem = new TiXmlElement("Point");
                TiXmlElement* coordElem = new TiXmlElement("coordinates");
                TiXmlElement* altitudeModeElem = new TiXmlElement("altitudeMode");
                TiXmlText* coordElemText = new TiXmlText(coord);
                coordElem->LinkEndChild(coordElemText);
                pointElem->LinkEndChild(coordElem);
                TiXmlText* altitudeModeElemText = new TiXmlText(String("clampToGround"));
                altitudeModeElem->LinkEndChild(altitudeModeElemText);
                pointElem->LinkEndChild(altitudeModeElem);
                placeElem->LinkEndChild(pointElem);
                
                //ExtendedData
                {
                    TiXmlElement* extendedData = new TiXmlElement("ExtendedData");

                    for (int i = 0; i < sv.size(); i++)
                    {
                        {
                            String strName = headers[i];
                            String strValue = sv[i];

                            StringUtil::trim(strName);
                            StringUtil::trim(strValue);

                            TiXmlElement* data = new TiXmlElement("Data");
                            data->SetAttribute("name", osgDB::convertStringFromCurrentCodePageToUTF8(strName));

                            TiXmlElement* value = new TiXmlElement("value");
                            TiXmlText* valueElemText = new TiXmlText(osgDB::convertStringFromCurrentCodePageToUTF8(strValue));
                            value->LinkEndChild(valueElemText);

                            data->LinkEndChild(value);
                            extendedData->LinkEndChild(data);
                        }
                    }

                    placeElem->LinkEndChild(extendedData);
                }

                documentElem->LinkEndChild(placeElem);
            }
        }
        ifs.close();

        document.SaveFile(outFilename);
    }
}

struct StationStru
{
    String code;
    String name;
    CVector3 position;
    NameValuePairList extentData;
};

typedef std::vector<StationStru> StationStruList;

TiXmlDocument writekml(StationStruList& list,String styleIcon)
{
    TiXmlDocument document;
    TiXmlElement* documentElem = initStyleKML(document, styleIcon);
    for (auto d : list)
    {
        TiXmlElement* placeElem = new TiXmlElement("Placemark");
        TiXmlElement* styleUrlElem = new TiXmlElement("styleUrl");
        TiXmlElement* nameElem = new TiXmlElement("name");
        TiXmlText* styleUrlElemText = new TiXmlText("#style");
        TiXmlText* nameElemText = new TiXmlText(A2U8(d.name));
        placeElem->LinkEndChild(nameElem);
        nameElem->LinkEndChild(nameElemText);
        placeElem->LinkEndChild(styleUrlElem);
        styleUrlElem->LinkEndChild(styleUrlElemText);

        String coord = StringConverter::formatDoubletoString(d.position.x, 3) +
            "," + StringConverter::formatDoubletoString(d.position.y, 3) +
            "," + StringConverter::formatDoubletoString(d.position.z, 1);
        TiXmlElement* pointElem = new TiXmlElement("Point");
        TiXmlElement* coordElem = new TiXmlElement("coordinates");
        TiXmlElement* altitudeModeElem = new TiXmlElement("altitudeMode");
        TiXmlText* coordElemText = new TiXmlText(coord);
        coordElem->LinkEndChild(coordElemText);
        pointElem->LinkEndChild(coordElem);
        TiXmlText* altitudeModeElemText = new TiXmlText(String("clampToGround"));
        altitudeModeElem->LinkEndChild(altitudeModeElemText);
        pointElem->LinkEndChild(altitudeModeElem);
        placeElem->LinkEndChild(pointElem);

        //ExtendedData
        {
            TiXmlElement* extendedData = new TiXmlElement("ExtendedData");

            for (auto& extraData : d.extentData)
            {
                {
                    String strName = extraData.first;
                    String strValue = extraData.second;

                    StringUtil::trim(strName);
                    StringUtil::trim(strValue);

                    TiXmlElement* data = new TiXmlElement("Data");
                    data->SetAttribute("name", osgDB::convertStringFromCurrentCodePageToUTF8(strName));

                    TiXmlElement* value = new TiXmlElement("value");
                    TiXmlText* valueElemText = new TiXmlText(osgDB::convertStringFromCurrentCodePageToUTF8(strValue));
                    value->LinkEndChild(valueElemText);

                    data->LinkEndChild(value);
                    extendedData->LinkEndChild(data);
                }
            }

            placeElem->LinkEndChild(extendedData);
        }

        documentElem->LinkEndChild(placeElem);
    }

    return document;
}

void stationToKML2(String filename)
{
    StationStruList listA;
    StationStruList listB;
    std::ifstream ifs(filename.c_str());
    if(ifs)
    {
        String line;
        getline(ifs, line);//skip first line
        line = osgDB::convertStringFromUTF8toCurrentCodePage(line);
        StringVector header = StringUtil::splitEx(line, ",");
        while (getline(ifs, line))
        {
            line = osgDB::convertStringFromUTF8toCurrentCodePage(line);
            StringVector sv = StringUtil::splitEx(line, ",");
            int size = (int)sv.size();
            if (size > 4)
            {
                StationStru stru;

                stru.code = sv[0];
                stru.name = sv[1];
                stru.position.x = StringConverter::parseReal(sv[8]);
                stru.position.y = StringConverter::parseReal(sv[9]);
                stru.position.z = 0.0;

                for (int i = 0; i < size; i++)
                {
                    stru.extentData[header[i]] = sv[i];
                }
                if (stru.extentData["站点类型"] == "国家站")
                {
                    listA.push_back(stru);
                }
                else
                {
                    listB.push_back(stru);
                }
            }
        }

        ifs.close();
    }

    String name, ext, path;
    StringUtil::splitFullFilename(filename, name, ext, path);
   
    {
        TiXmlDocument document = writekml(listA, "radar-stationA.png");
        String outFilename = path + "nation.kml";
        document.SaveFile(outFilename);
    }
    {
        TiXmlDocument document = writekml(listB, "radar-station.png");
        String outFilename = path + "local.kml";
        document.SaveFile(outFilename);
    }
}