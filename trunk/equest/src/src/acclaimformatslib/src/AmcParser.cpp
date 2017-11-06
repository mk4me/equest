#include <acclaimformatslib/AmcParser.h>
#include <utils/Debug.h>
#include <fstream>
#include <sstream>
#include "utils/Debug.h"

using namespace acclaim;

void AmcParser::initSerialize(std::ostream& stream, const kinematicUtils::AngleUnitType angleType)
{
	stream << ":FULLY-SPECIFIED" << std::endl;
	switch (angleType)
	{
	case kinematicUtils::AngleUnitType::Rad:
		stream << ":RADIANS";
		break;
	case kinematicUtils::AngleUnitType::Deg:
		stream << ":DEGREES";
		break;	
	}
	stream << std::endl;
}

void AmcParser::serialize(const MotionData::FrameData & frame, std::ostream& stream)
{
	stream << frame.id << std::endl;
	for (unsigned int j = 0; j < frame.bonesData.size(); ++j) {
		stream << frame.bonesData[j].name;
		for (unsigned int k = 0; k < frame.bonesData[j].channelValues.size(); ++k) {
			stream << " " << frame.bonesData[j].channelValues[k];
		}

		stream << std::endl;
	}
}

void AmcParser::serialize(const MotionData & data,
	const kinematicUtils::AngleUnitType angleType, std::ostream& out)
{
	initSerialize(out, angleType);

	for (unsigned int i = 0; i < data.frames.size(); ++i) {
		serialize(data.frames[i], out);
    }
}

MotionData::FramesData AmcParser::parse(std::istream & stream)
{
    std::string::size_type loc;    
    std::string line;

    // HACK: z braku dokładnej dokumentacji nie wiem jakie moga wystapic nagłówki pliku *.amc
    // wszystkie do tej pory spotkane pliki konczyly nagłówek linijka :DEGREES,
	// rozwiązanie stosowane jest w innych parserach
    while (std::getline(stream, line)) {
        loc = line.find(":DEGREES");
        if (loc != std::string::npos) {
            break;
        }

		loc = line.find(":RADIANS");
		if (loc != std::string::npos) {
			break;
		}
    }

	MotionData::FramesData frames;
	MotionData::FrameData* frame = nullptr;	

    while (std::getline(stream, line)) {
        std::istringstream iss(line);
        unsigned int frameNo = 0;
        if (iss >> frameNo) {
			frames.push_back(MotionData::FrameData());
			frame = &frames.back();
            frame->id = frameNo;
        } else {
            iss.clear();
			MotionData::BoneData boneData;
			iss >> boneData.name;
			if (boneData.name.empty() == false) {
				boneData.channelValues.clear();

				double val = 0.0;
				while (iss >> val) {
					boneData.channelValues.push_back(val);
				}

				frame->bonesData.push_back(boneData);
			}
        }
    }

	return frames;
}
