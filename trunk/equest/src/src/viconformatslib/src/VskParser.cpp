#include <viconformatslib/VskParser.h>
#include <tinyxml2.h>
#include <iostream>
#include <sstream>
#include <utils/Debug.h>
#include <utils/StreamTools.h>

using namespace vicon;

void readSticks(tinyxml2::XMLElement* sticksElement, std::vector<Stick> & sticks)
{
	// odczyt wczystkich połączeń
	for (auto child = sticksElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		if (child != nullptr && strcmp(child->Value(), "Stick") == 0) {
			Stick s;
			auto attrib = child->FirstAttribute();
			while (attrib != nullptr) {
				if (strcmp(attrib->Name(), "MARKER1") == 0) {
					s.name1 = attrib->Value();
				}
				else if (strcmp(attrib->Name(), "MARKER2") == 0) {
					s.name2 = attrib->Value();
				}
				else if (strcmp(attrib->Name(), "RGB") == 0) {
					std::istringstream iss(attrib->Value());
					int r = 0;
					int g = 0;
					int b = 0;
					iss >> s.color[0] >> s.color[1] >> s.color[2];
					s.color /= 256.0f;
					s.color[3] = 1.0f;
				}

				attrib = attrib->Next();
			}
			if (s.name1.empty() == false && s.name2.empty() == false) {
				sticks.push_back(s);
			}
		}
	}
}

void readMarkers(tinyxml2::XMLElement* markersElement, std::vector<Marker> & markers)
{
	// odczyt wszystkich markerow
	for (auto child = markersElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		if (child != nullptr && strcmp(child->Value(), "Marker") == 0) {
			Marker m;
			auto attrib = child->FirstAttribute();
			while (attrib != nullptr) {
				if (strcmp(attrib->Name(), "NAME") == 0) {
					m.name = attrib->Value();
				}
				else if (strcmp(attrib->Name(), "RADIUS") == 0) {
					m.radius = attrib->DoubleValue();
				}
				else if (strcmp(attrib->Name(), "RGB") == 0) {
					std::istringstream iss(attrib->Value());
					int r = 0;
					int g = 0;
					int b = 0;
					iss >> m.color[0] >> m.color[1] >> m.color[2];
					m.color /= 256.0f;					
					m.color[3] = 1.0f;
				}

				attrib = attrib->Next();
			}

			if (m.name.empty() == false) {
				markers.push_back(m);
			}
		}
	}
}

Vsk VskParser::parse(std::istream& in)
{
	Vsk ret;

	std::string content = utils::StreamTools::read(in);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError error = doc.Parse(content.c_str(), content.size());
    tinyxml2::XMLElement* rootElement = nullptr;
    // jeśli plik jest plikiem *xml ...
    if (error == tinyxml2::XML_NO_ERROR) {
        auto docElement = doc.FirstChildElement();
        while (docElement != nullptr) {
            // szukanie głównego elementu - "KinematicModel"
            if (strcmp(docElement->Value(), "KinematicModel") == 0) {
                rootElement = docElement;
                break;
            }
            docElement = docElement->NextSiblingElement();
        }
    } else {
        // oczyt nie udał się z jakiegos powodu
        throw std::runtime_error("Failed to parse vsk stream");
    }
        
    // jeśli plik zawiera odpowiedni korzeń (root)
    if (rootElement != nullptr) {
        for (auto element = rootElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
            // element Markers set zawiera dane dla połączeń i pojedynczych markerow
            if (element != nullptr && strcmp(element->Value(), "MarkerSet") == 0) {
				for (auto markersElement = element->FirstChildElement();
				markersElement != nullptr; markersElement = markersElement->NextSiblingElement()) {

					if (markersElement != nullptr) {
						if (strcmp(markersElement->Value(), "Markers") == 0) {
							// znaleziono element z markerami!
							readMarkers(markersElement, ret.markers);
						}
						else if (strcmp(markersElement->Value(), "Sticks") == 0) {
							// znaleziono element z połączeniami markerow
							readSticks(markersElement, ret.sticks);
						}
					}
                }
            }
        }

		if (ret.sticks.empty() == true || ret.markers.empty() == true) {
            throw std::runtime_error("Stream has no sticks or markers");
        }
    } else {
        throw std::runtime_error("Stream does not have root element");
    }

	return ret;
}