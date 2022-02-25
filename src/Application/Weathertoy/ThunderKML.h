#pragma once

void thunderToKML(String filename,String outFilename)
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
        TiXmlElement* styleElem = new TiXmlElement("Style");
        styleElem->SetAttribute("id", "style1");
        documentElem->LinkEndChild(styleElem);
        TiXmlElement* iconStyleElem = new TiXmlElement("IconStyle");
        TiXmlElement* iconElem = new TiXmlElement("Icon");
        iconStyleElem->LinkEndChild(iconElem);
        TiXmlElement* hrefElem = new TiXmlElement("href");
        iconElem->LinkEndChild(hrefElem);
        styleElem->LinkEndChild(iconStyleElem);
        TiXmlText* text = new TiXmlText("thunder-red.png");
        hrefElem->LinkEndChild(text);
        String line;
        while (getline(ifs, line))
        {
            StringVector sv = StringUtil::split(line, " ");
            if (sv.size() == 12)
            {
                String lonStr = StringUtil::split(sv[4], "=")[1];
                String latStr = StringUtil::split(sv[3], "=")[1];
                StringUtil::trim(lonStr);
                StringUtil::trim(latStr);
                double lon = StringConverter::parseReal(lonStr);
                double lat = StringConverter::parseReal(latStr);

                TiXmlElement* placeElem = new TiXmlElement("Placemark");
                TiXmlElement* styleUrlElem = new TiXmlElement("styleUrl");
                TiXmlText* styleUrlElemText = new TiXmlText("#style1");
                placeElem->LinkEndChild(styleUrlElem);
                styleUrlElem->LinkEndChild(styleUrlElemText);

                String coord = StringConverter::formatDoubletoString(lon, 3) + "," + StringConverter::formatDoubletoString(lat, 3);
                TiXmlElement* pointElem = new TiXmlElement("Point");
                TiXmlElement* coordElem = new TiXmlElement("coordinates");
                TiXmlText* coordElemText = new TiXmlText(coord);
                coordElem->LinkEndChild(coordElemText);
                pointElem->LinkEndChild(coordElem);
                placeElem->LinkEndChild(pointElem);

                //ExtendedData
                {
                    TiXmlElement* extendedData = new TiXmlElement("ExtendedData");

                    for (int i = 5; i < sv.size(); i++)
                    {
                        StringVector attrVec = StringUtil::split(sv[i], "=:");
                        if (attrVec.size() == 2)
                        {
                            String strName = attrVec[0];
                            String strValue = attrVec[1];

                            StringUtil::trim(strName);
                            StringUtil::trim(strValue);

                            TiXmlElement* data = new TiXmlElement("Data");
                            data->SetAttribute("name", strName);

                            TiXmlElement* value = new TiXmlElement("value");
                            TiXmlText* valueElemText = new TiXmlText(strValue);
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