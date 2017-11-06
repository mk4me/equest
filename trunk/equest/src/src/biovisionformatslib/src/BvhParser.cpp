#include <biovisionformatslib/BvhParser.h>
#include <kinematicUtils/RotationConverter.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

using namespace biovision;
//----------------------------------------------------------------------------------
std::string BvhParser::getChannelName(const kinematicUtils::Channel channel) {
    switch (channel) {
	case kinematicUtils::ChannelType::TX: return "Xposition";
	case kinematicUtils::ChannelType::TY: return "Yposition";
	case kinematicUtils::ChannelType::TZ: return "Zposition";
	case kinematicUtils::ChannelType::RX: return "Xrotation";
	case kinematicUtils::ChannelType::RY: return "Yrotation";
	case kinematicUtils::ChannelType::RZ: return "Zrotation";
    }
    
    throw std::runtime_error("Unsupported biovision channel");
}

kinematicUtils::Channel BvhParser::getChannel(const std::string & channelName)
{
	kinematicUtils::Channel ret = kinematicUtils::ChannelType::CUSTOM_CHANNEL_BASE;

	if (boost::iequals(channelName, "Xposition ")){
		ret = kinematicUtils::ChannelType::TX;
	}
	else if (boost::iequals(channelName, "Yposition ")){
		ret = kinematicUtils::ChannelType::TY;
	}
	else if (boost::iequals(channelName, "Zposition ")){
		ret = kinematicUtils::ChannelType::TZ;
	}
	else if (boost::iequals(channelName, "Xrotation ")){
		ret = kinematicUtils::ChannelType::RX;
	}
	else if (boost::iequals(channelName, "Yrotation ")){
		ret = kinematicUtils::ChannelType::RY;
	}
	else if (boost::iequals(channelName, "Zrotation ")){
		ret = kinematicUtils::ChannelType::RZ;
	}
	else {
		throw std::runtime_error("Unsupported biovision channel name");
	}

	return ret;
}

//-------------------------------------------------------------------------------------------
std::string BvhParser::spaceL(int lvl) const {
    // podobno niektóre parsery nie potrafia sobie poradzic z plikiem *.bvh
    // jeśli będzie miał wciecia zrobione ze spacji
    if (tabs == false) {
        return std::string((lvl + 1) * 2, ' ');
    } else {
        return std::string(lvl, '\t');
    }
}
//----------------------------------------------------------------------------------
bool BvhParser::hasTranslation(const std::vector<kinematicUtils::Channel>& channels ) {
	
	return std::find_if(channels.begin(), channels.end(), [](const kinematicUtils::Channel channel)
	{
		if (channel == kinematicUtils::ChannelType::TX ||
			channel == kinematicUtils::ChannelType::TY ||
			channel == kinematicUtils::ChannelType::TZ) {
			return true;
		}

		return false;
	}) != channels.end();
}
//----------------------------------------------------------------------------------
void BvhParser::saveChannelsInHierarchy( std::ostream& out, JointConstPtr joint, int lvl) const
{
    if (forceZXY) {
		out << spaceL(lvl) << "CHANNELS 3 Zrotation Xrotation Yrotation" << std::endl;
    } else {
        auto count = joint->channels.size();
        if (count > 0) {
            out << spaceL(lvl) << "CHANNELS " << count;
            for (decltype(count) i = 0; i < count; ++i) {
				out << " " << getChannelName(joint->channels[i]);
            }
			out << std::endl;
        }
    }
}
//----------------------------------------------------------------------------------
//void BvhParser::saveJoint(const Skeleton & model, std::ostream& out, JointConstPtr joint, 
//		JointConstPtr parent,  int lvl, std::list<std::string>& jointList) const 
//{
	/*
    if (parent && parent->length == 0.0 && joint->children.size() == 1) {
        // jeśli przetwarzana kość to 'dummy bone' to zostanie ona zignorowana przy zapisie do bvh        
		saveJoint(model, out, joint->joints.front(), JointPtr(), lvl, jointList);
    } else {
        // dodanie kości do listy z kolejnością wystapienia (ważne przy zapisie danych z kanałów)
        jointList.push_back(joint->name);
		int childrenCount = joint->joints.size();
		out << spaceL(lvl) << "JOINT " << joint->name << std::endl;
		out << spaceL(lvl) << "{" << std::endl;
        ++lvl; // zwiekszenie wciecia
        out << spaceL(lvl) << "OFFSET ";
        // kość rodzica jest potrzebna aby obliczyć długość,
        // dlaczego parent nie jest brany z argumentu funkcji? poniewaz argument moze być nullem
        // (sztuczka robiona w przypadku gdy parent to dummy bone)
		JointConstPtr parent = utils::const_pointer_cast<const Joint>(joint->parent.lock());
        osg::Vec3 offset = parent->direction * parent->length * model->getLength();
		out << offset[0] << " " << offset[1] << " " << offset[2] << std::endl;
        saveChannelsInHierarchy(out, joint, lvl);
        // rekurencja
        for (int i = 0; i < childrenCount; ++i) {
            saveJoint(model, out, joint->children[i], joint, lvl, jointList);
        }
        // lisc jest traktowany inaczej
        if (childrenCount == 0) {
			out << spaceL(lvl) << "End Site" << std::endl;
			out << spaceL(lvl) << "{" << std::endl;
            out << spaceL(lvl + 1) << "OFFSET ";
            osg::Vec3 offset = joint->direction * joint->length;
			out << offset[0] << " " << offset[1] << " " << offset[2] << std::endl;
			out << spaceL(lvl) << "}" << std::endl;
        }

        lvl--;
		out << spaceL(lvl) << "}" << std::endl;
    }
	*/
//}
//----------------------------------------------------------------------------------
void BvhParser::save(const Skeleton & model, const MotionData & data, const std::string& filename ) {
		
 //   std::ofstream out;
 //   // tutaj trafiaja jointy zgodnie z kolejnością w której sa zapisywane
 //   std::list<std::string> jointList;
 //   const Skeleton& skeleton = model->getSkeleton();
 //   out.open(filename.c_str());
 //   int lvl = 0;
	//out << "HIERARCHY" << std::endl;
	//out << "ROOT " << skeleton.getRootName() << std::endl;
	//out << "{" << std::endl;
 //   out << spaceL(lvl) << "OFFSET ";
 //   // pozycja roota w globalnym układzie odniesienia
 //   osg::Vec3 pos = skeleton.getPosition();
 //   out << pos[0] << " ";
 //   out << pos[1] << " ";
 //   out << pos[2];
 //   // todo flaga forceZXY
	//out << std::endl << spaceL(lvl) << "CHANNELS 6";// << channels.size();
	//out << " Xposition Yposition Zposition Zrotation Xrotation Yrotation" << std::endl;
 //   /*for (unsigned int i = 0; i < channels.size(); ++i) {
 //       out << " " << getChannelName(channels[i]);
 //   }*/
	//out << std::endl;
 //   JointConstPtr root = skeleton.getRoot();
 //   jointList.push_back(root->name);
 //   int count = root->children.size();
 //   // rekurencja
 //   for (int i = 0; i < count; ++i) {
 //       saveJoint(model, out, root->children[i], root, lvl, jointList);
 //   }
	//out << "}" << std::endl;

 //   // zapis danych animacji
	//const std::vector<SkeletalData::singleFramePtr>& frames = data->getFrames();
	//out << "MOTION" << std::endl;
	//out << "Frames:\t" << frames.size() << std::endl;
	//out << "Frame Time:\t" << data->getFrameTime() << std::endl;

 //   int framesCount = frames.size();
 //   // dla wszystkich klatek...
 //   for (int i = 0; i < framesCount; ++i) {
 //       std::list<std::string>::iterator it;
 //       // zgodnie z kolejnością zapisu jointów w pliku...
 //       for (it = jointList.begin(); it != jointList.end(); ++it) {
 //           SkeletalData::singleJointStatePtr jointState = getSingleJointState(data, i, *it);
 //           JointConstPtr bone = model->getJointByName(jointState->name);

 //           if (DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::L, bone->dofs) != -1) {
 //               
 //           }
 //           // nastepuje zapis danych
 //           if (forceZXY) {
 //               int index = 0;
 //               if (hasTranslation(bone->dofs)) {
	//				index = DegreeOfFreedom::getChannelIndex(kinematicUtils::TX, bone->dofs);
 //                   out << " " << (index != -1 ? jointState->channelValues[index] : 0.0);
	//				index = DegreeOfFreedom::getChannelIndex(kinematicUtils::TY, bone->dofs);
 //                   out << " " << (index != -1 ? jointState->channelValues[index] : 0.0);
	//				index = DegreeOfFreedom::getChannelIndex(kinematicUtils::TZ, bone->dofs);
 //                   out << " " << (index != -1 ? jointState->channelValues[index] : 0.0);
 //               }
 //               double rx, ry, rz;
	//			index = DegreeOfFreedom::getChannelIndex(kinematicUtils::RZ, bone->dofs);
 //               rz = (index != -1 ? jointState->channelValues[index] : 0.0);
	//			index = DegreeOfFreedom::getChannelIndex(kinematicUtils::RX, bone->dofs);
 //               rx = (index != -1 ? jointState->channelValues[index] : 0.0);
	//			index = DegreeOfFreedom::getChannelIndex(kinematicUtils::RY, bone->dofs);
 //               ry = (index != -1 ? jointState->channelValues[index] : 0.0);
 //               osg::Vec3 v; v[0] = rx; v[1] = ry; v[2] = rz;
 //               osg::Matrix mat;
 //               double mul = model->getAngle() == SkeletalModel::radians ? 1.0 : osg::DegreesToRadians(1.0);
 //               osg::Quat mX; mX.makeRotate(mul * (bone->axis[0]), osg::Vec3(1, 0, 0));
 //               osg::Quat mY; mY.makeRotate(mul * (bone->axis[1]), osg::Vec3(0, 1, 0));
 //               osg::Quat mZ; mZ.makeRotate(mul * (bone->axis[2]), osg::Vec3(0, 0, 1));
 //               osg::Quat m = mX * mY * mZ;
 //               v = m * v;
 //               rx = v[0]; ry = v[1]; rz = v[2];
 //               out << " " << rz << " " << rx << " " << ry;
 //           }
 //           // todo rozwiązać ten balagan
 //           /*else {
 //               for (unsigned int k = 0; k < frames[i].bonesData[j].channelValues.size(); k++) {
 //                   out << frames[i].bonesData[j].channelValues[k] << " ";
 //               }
 //           }*/
 //           
 //       }
	//	out << std::endl;
 //   }

 //   out.close();
}
//----------------------------------------------------------------------------------
BVHData BvhParser::parse(std::istream & in ) {

	BVHData bvh;

    std::string line;
    while (in >> line)
    {
        if (boost::iequals(line, "HIERARCHY")) {
            // rekurencyjne przejście przez cala hierarchie w pliku
            readSingleJoint(bvh.model, in, bvh.model.sourceOrderJoints, JointPtr());
		}
		else if (boost::iequals(line, "MOTION")) {
            // odczyt danych animacji
            std::size_t noOfFrames = -1;
            in >> line >> noOfFrames;            
			in >> line >> line >> bvh.data.frameTime;
            // wartości dla kolejnych klatek
            for (decltype(noOfFrames) m=0; m < noOfFrames; ++m) {
                std::getline(in, line);
                MotionData::FrameJointData frame;                
                // zgodnie z kolejnością wystapienia jointów w pliku
                for (const auto & joint : bvh.model.sourceOrderJoints) {
                    auto channelsCount = joint->channels.size();
                    double number;
					MotionData::ChannelData channelData;
                    for (decltype(channelsCount) k = 0; k < channelsCount; ++k ) {
                        in >> number;
						channelData.push_back(number);
                    }
					frame.push_back(std::move(channelData));
                }

				bvh.data.frames.push_back(frame);
            }
        }
    }

	return bvh;
}
//----------------------------------------------------------------------------------
BvhParser::BvhParser()
: forceZXY(true),
 tabs(true)
{

}
//----------------------------------------------------------------------------------
BvhParser::~BvhParser()
{

}
//----------------------------------------------------------------------------------
void BvhParser::readSingleJoint(Skeleton & model, std::istream& in, std::vector<JointPtr>& jointList, JointPtr parent)
{
    std::string token;
    int intBuffer;

	do{

		in >> token;
		while (token.size() == 0 && (in >> token)) {}

		if (token == "}"){
			return;
		}

		if (token == "{"){
			throw std::runtime_error("Invalid BVH file format - segment section begin marker found before ROOT or JOINT keyword");
		}

		if (boost::iequals(token, "End")) {

			if (parent == nullptr){
				throw std::runtime_error("Invalid BVH file format - End Site with no parent");
			}

			auto j = utils::make_shared<Joint>();

			// natrafiono na lisc
			j->name = parent->name + "_effector";
			j->parent = parent;
			while (token.size() == 0 && (in >> token)) {}
			while (token.size() == 0 && (in >> token)) {}
			while (token.size() == 0 && (in >> token)) {}
			in >> token >> token >> token >> j->offset[0] >> j->offset[1] >> j->offset[2];

			while (token.size() == 0 && (in >> token)) {}
			if (token != "}"){
				throw std::runtime_error("Invalid BVH file format - segment section ending not found, unknown keyword: " + token);
			}

			parent->joints.push_back(j);
		}
		else if (boost::iequals(token, "ROOT") || boost::iequals(token, "JOINT")) {

			if (boost::iequals(token, "JOINT") && parent == nullptr){
				throw std::runtime_error("Invalid BVH file format - joint with no parent, root missing");
			}

			auto j = utils::make_shared<Joint>();

			j->parent = parent;
			std::string name;
			while (in >> token) {
				if (token != "{") {
					name += token + ' ';
				}
				else {
					break;
				}
			}

			const int size = name.size();
			if (size > 0) {
				name.resize(size - 1);
			}

			if (model.joints.find(name) != model.joints.end()){
				throw std::runtime_error("Invalid BHV file format - joint name duplicate: " + name);
			}

			j->name = name;

			in >> token >> j->offset[0] >> j->offset[1] >> j->offset[2];
			in >> token >> intBuffer;

			kinematicUtils::ChannelTriplet rt;
			unsigned int transi = 0;
			unsigned int roti = 0;

			for (int i = 0; i < intBuffer; ++i)
			{
				in >> token;
				auto channel = getChannel(token);

				if (channel == kinematicUtils::ChannelType::TX || channel == kinematicUtils::ChannelType::TY
					|| channel == kinematicUtils::ChannelType::TZ)
				{
					++transi;
				}
				else if (channel == kinematicUtils::ChannelType::RX || channel == kinematicUtils::ChannelType::RY
					|| channel == kinematicUtils::ChannelType::RZ)
				{
					rt[roti++] = channel;
				}				

				j->channels.push_back(channel);
			}

			if (!(transi == 0 || transi == 3) || !(roti == 0 || roti == 3))
			{
				throw std::runtime_error("Invalid BVH channels description - incomplete translation or rotation description");
			}

			j->rotationOrder = kinematicUtils::AxisOrder::fromChannelTriplet(rt);

			jointList.push_back(j);

			if (parent) {
				parent->joints.push_back(j);
			}

			model.joints.insert({ name, j });
			readSingleJoint(model, in, jointList, j);
		}
		else{
			throw std::runtime_error("Invalid BVH file format - unsupported keyword: " + token);
		}
	} while (token != "}");
}
//----------------------------------------------------------------------------------
