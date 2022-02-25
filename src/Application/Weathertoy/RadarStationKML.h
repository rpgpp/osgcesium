#pragma once

void stationToKML(String filename, String outFilename,bool isStation)
{
    ifstream ifs(filename.c_str());
    if (ifs)
    {
        TiXmlDocument document;
        document.LinkEndChild(new TiXmlDeclaration("1.0", "UTF-8", ""));
        TiXmlElement* kmlElem = new TiXmlElement("kml");
        document.LinkEndChild(kmlElem);
        TiXmlElement* documentElem = new TiXmlElement("Document");
        kmlElem->LinkEndChild(documentElem);

        if(!isStation)
        {
            TiXmlElement* styleElem = new TiXmlElement("Style");
            styleElem->SetAttribute("id", "style1");
            documentElem->LinkEndChild(styleElem);
            TiXmlElement* iconStyleElem = new TiXmlElement("IconStyle");
            TiXmlElement* iconElem = new TiXmlElement("Icon");
            iconStyleElem->LinkEndChild(iconElem);
            TiXmlElement* hrefElem = new TiXmlElement("href");
            iconElem->LinkEndChild(hrefElem);
            styleElem->LinkEndChild(iconStyleElem);
            TiXmlText* text = new TiXmlText("radar-station.png");
            hrefElem->LinkEndChild(text);
        }
        else
        {
            TiXmlElement* styleElem = new TiXmlElement("Style");
            styleElem->SetAttribute("id", "style2");
            documentElem->LinkEndChild(styleElem);
            TiXmlElement* iconStyleElem = new TiXmlElement("IconStyle");
            TiXmlElement* iconElem = new TiXmlElement("Icon");
            iconStyleElem->LinkEndChild(iconElem);
            TiXmlElement* hrefElem = new TiXmlElement("href");
            iconElem->LinkEndChild(hrefElem);
            styleElem->LinkEndChild(iconStyleElem);
            TiXmlText* text = new TiXmlText("radar-stationA.png");
            hrefElem->LinkEndChild(text);
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
                TiXmlText* coordElemText = new TiXmlText(coord);
                coordElem->LinkEndChild(coordElemText);
                pointElem->LinkEndChild(coordElem);
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

        LabViewer::instance()->showMessageBox("Íê³É");
    }
}