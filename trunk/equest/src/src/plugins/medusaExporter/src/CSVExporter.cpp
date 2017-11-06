#include "MedusaExporterPCH.h"
#include "CSVExporter.h"
#include <fstream>
#include "AnnotationData.h"
#include <plugins/dicom/Annotations.h>
#include <plugins/dicom/ValueLayer.h>
#include <plugins/dicom/ILayer.h>

using namespace std;
using namespace medusaExporter;

bool isIdentical(const std::vector<dicom::ILayerItemConstPtr>& layers) 
{
	for (auto it = layers.begin(); it != layers.end(); ++it) {
		auto val = boost::dynamic_pointer_cast<const dicom::IValueLayer>(*it);
		if (val) {
			if (val->valueAsString() == dicom::annotations::annotationValueAsString(dicom::annotations::identical).toStdString()){
				return true;
			}
		}
	}

	return false;
}

dicom::ILayerItemConstPtr tryGetAnnotation(const std::vector<dicom::ILayerItemConstPtr>& layers, dicom::annotations::annotationsIdx annotationsIdx)
{
    for (auto it = layers.begin(); it != layers.end(); ++it) {
        if ((*it)->getAdnotationIdx() == (int)annotationsIdx) {
            return *it;
        }
    }
    return dicom::ILayerItemConstPtr();
}

void medusaExporter::CSVExporter::exportAnnotations(const utils::Filesystem::Path& path, const AnnotationData& data, const ExportConfig& config) const
{
    exportMeta(path, data, config);
    exportData(path, data, config);
}

void medusaExporter::CSVExporter::exportMeta(const utils::Filesystem::Path& path, const AnnotationData& data, const ExportConfig& config) const
{
    std::string outFile = (path / "meta.csv").string();
    ofstream file;
    file.open(outFile, ios::out);
    auto names = dicom::annotations::instance()->left;
    const auto& layers = data.getAnnotations();

    file << "fileName, powerDoppler, inflammatoryLevel, jointType, imageStatus" << std::endl;
    for (auto itLayer = layers.begin(); itLayer != layers.end(); ++itLayer) {
        if (config.skipIdentical && isIdentical(itLayer->second)) {
            continue;
        }
        file << itLayer->first.imageName << ", ";
        
        auto pd = boost::dynamic_pointer_cast<const dicom::IValueLayer>(tryGetAnnotation(itLayer->second, dicom::annotations::bloodLevel));
        std::string doppler = pd ? pd->valueAsString() : std::string("-1");

        auto infl = boost::dynamic_pointer_cast<const dicom::IValueLayer>(tryGetAnnotation(itLayer->second, dicom::annotations::inflammatoryLevel));
        std::string inflammatory = infl ? infl->valueAsString() : std::string("-1");
        
		auto joint = boost::dynamic_pointer_cast<const dicom::IValueLayer>(tryGetAnnotation(itLayer->second, dicom::annotations::jointType));
		auto finger = boost::dynamic_pointer_cast<const dicom::IValueLayer>(tryGetAnnotation(itLayer->second, dicom::annotations::fingerType));
        std::string jointType = "-1";
        if (joint && finger) {
            auto v1 = joint->valueAsString();
            auto v2 = finger->valueAsString();
            if (v1 != "-1" && v2 != "-1") {
                jointType = v1 + v2;
            }
        }

		auto img = boost::dynamic_pointer_cast<const dicom::IValueLayer>(tryGetAnnotation(itLayer->second, dicom::annotations::imageType));
        std::string imageType = img ? img->valueAsString() : "-1";

        file << doppler << ", " << inflammatory << ", " << jointType << ", " << imageType << ", " << std::endl;
    }

    file.close();
}


int getMaxPointsCount(const AnnotationData::Layers& layers, int density)
{
    int maxCount = 0;
    for (auto itLayer = layers.begin(); itLayer != layers.end(); ++itLayer) {
        for (auto it = itLayer->second.begin(); it != itLayer->second.end(); ++it) {
			dicom::ILayerGraphicItemConstPtr graphic = boost::dynamic_pointer_cast<const dicom::ILayerGraphicItem>(*it);
            if (graphic) {
                int points = graphic->getPointsCloud(density).size();
                if (points > maxCount) {
                    maxCount = points;
                }
            }
        }
    }

    return maxCount;
}

int getGraphicsLayersCount(const std::vector<dicom::ILayerItemConstPtr>& layers)
{
    int counter = 0;
    for (auto it = layers.begin(); it != layers.end(); ++it) {
		auto val = boost::dynamic_pointer_cast<const dicom::ILayerGraphicItem>(*it);
        if (val) {
            ++counter;
        }
    }
    return counter;
}

void medusaExporter::CSVExporter::exportData(const utils::Filesystem::Path& path, const AnnotationData& data, const ExportConfig& config) const
{
    std::string outFile = (path / "data.csv").string();
    ofstream file;
    file.open(outFile, ios::out);
    auto names = dicom::annotations::instance()->left;
    const auto& layers = data.getAnnotations();

    int maxCount = getMaxPointsCount(layers, config.pointsDensity);
    for (auto itLayer = layers.begin(); itLayer != layers.end(); ++itLayer) {
        if (config.skipIdentical && isIdentical(itLayer->second)) {
            continue;
        }
        
        int graphicsCount = getGraphicsLayersCount(itLayer->second);
        if (graphicsCount == 0) {
            continue;
        }

        file << itLayer->first.imageName << ", " << graphicsCount << std::endl;
        for (auto it = itLayer->second.begin(); it != itLayer->second.end(); ++it) {
            dicom::ILayerItemConstPtr itm = *it;

			dicom::ILayerGraphicItemConstPtr graphic = boost::dynamic_pointer_cast<const dicom::ILayerGraphicItem>(itm);
            if (graphic) {
                file << names.at((dicom::annotations::annotationsIdx)itm->getAdnotationIdx()).toStdString(); // << ", " << itm->getName().toStdString();

				std::vector<QPointF> points;
				if (config.pixelByPixel) {
					points = graphic->getPixelPointsCloud();
				} else {
					points = config.normalizePointVectorsLenght ? graphic->getPointsCloud(config.pointsDensity, maxCount) : graphic->getPointsCloud(config.pointsDensity);
				}
                file << ", " << points.size();
                for (auto it = points.begin(); it != points.end(); ++it) {
                    file << ", " << it->x() << ", " << it->y();
                }
                file << std::endl;
            }
        }
    }

    file.close();
}
