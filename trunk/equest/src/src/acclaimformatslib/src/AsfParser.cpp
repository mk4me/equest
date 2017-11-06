#include <acclaimformatslib/AsfParser.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <utils/StreamTools.h>
#include <queue>

#define PARSER_VERSION "0.13"

using namespace acclaim;

std::string removeComments(const std::string& txt)
{
	std::string result;
	std::string line;
	std::istringstream is(txt);	

	while (std::getline(is, line)) {
		const auto position = line.find("#");
		if (position == std::string::npos) {
			result += line;
			result += '\n';
		}else if(position > 0) {
			result += line.substr(0, position);
			result += '\n';
		}		
	}

	return result;
}

std::string * getSectionContainter(const std::string& token,
	std::string & name, std::string & version, std::string & documentation,
	std::string & units, std::string & root, std::string & bonedata,
	std::string & hierarchy, std::string & unknown)
{
	auto localToken = boost::to_lower_copy(token);

	if (localToken.compare(":name") == 0)
		return &name;
	else if (localToken.compare(":documentation") == 0)
		return &documentation;
	else if (localToken.compare(":version") == 0)
		return &version;
	else if (localToken.compare(":units") == 0)
		return &units;
	else if (localToken.compare(":root") == 0)
		return &root;
	else if (localToken.compare(":bonedata") == 0)
		return &bonedata;
	else if (localToken.compare(":hierarchy") == 0)
		return &hierarchy;
	else
		return &unknown;
}

// Dzieli wejściowy plik na poszczegolne sekcje
bool splitAsf(const std::string& asf, std::string & name,
	std::string & documentation, std::string & version,
	std::string & units, std::string & root, std::string & bonedata,
	std::string & hierarchy, std::string & unknown)
{
	std::istringstream is(asf);
	std::string line;
	std::string token;

	auto section = &unknown;
	while (std::getline(is, line)) {
		std::istringstream linestream(line);
		linestream >> token;
		if (token[0] == ':') {
			section = getSectionContainter(token, name, version, documentation, units, root,
				bonedata, hierarchy, unknown);
			while (linestream >> token) *section += token;
			*section += '\n';
			continue;
		}

		*section += line;
		*section += '\n';
	}

	auto size = name.size();
	if (size > 0) {
		name.resize(size - 1, ' ');
	}
	size = documentation.size();
	if (size > 0) {
		documentation.resize(size - 1, ' ');
		while (documentation[0] == '\n') { documentation.erase(documentation.begin()); }
	}
	size = version.size();
	if (size > 0) {
		version.resize(size - 1, ' ');
	}

	return true;
}

void parseLimit(const std::string& token, std::vector<double>& limitValues)
{
	std::string limit;	

	// na pewno jest jakas funkcja do tego...
	for(const auto & c : token)
	{		
		if (c != '(' && c != ')') {
			limit += (c != ',' ? c : ' ');
		}
	}

	std::istringstream is(limit);

	for (char i = 0; i < 2; ++i)
	{
		is >> limit;
		if (limit.find("inf") != std::string::npos) {
			limitValues.push_back(limit.find("-") != std::string::npos ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity());
		}
		else {
			limitValues.push_back(boost::lexical_cast<double>(limit));
		}
	}
}

bool parseSingleBone(const std::string& singleBone, Bone& bone, int & id,
	const bool createUniqueIndicies) {
	std::istringstream is(singleBone);	
	kinematicUtils::AxisOrder::Type order = kinematicUtils::AxisOrder::XYZ;

	std::string token;
	// todo sprawdzic kopiowanie wektorow
	std::vector<kinematicUtils::Channel> channels;
	std::vector<double> limitValues;
	std::string axis;

	while (is >> token) 
	{
		auto localToken = boost::to_lower_copy(token);
		if (localToken.compare("id") == 0) {
			if (createUniqueIndicies == true){
				// niektóre pliki maja powtorzony unikalny id..
				bone.id = ++id;
			}
			else{
				is >> bone.id;
			}
		}
		else if (localToken.compare("name") == 0) {
			is >> bone.name;
		}
		else if (localToken.compare("bodymass") == 0) {
			//is >> bone.bodymass;
		}
		else if (localToken.compare("cofmass") == 0) {
			//is >> bone.cofmass;
		}
		else if (localToken.compare("direction") == 0) {
			is >> bone.direction._v[0];
			is >> bone.direction._v[1];
			is >> bone.direction._v[2];
		}
		else if (localToken.compare("length") == 0) {
			is >> bone.length;
		}
		else if (localToken.compare("axis") == 0) {
			is >> bone.axis._v[0];
			is >> bone.axis._v[1];
			is >> bone.axis._v[2];
			is >> axis;
			order = Axis::getAxisOrder(axis);
			bone.axisOrder = order;
		}
		else if (localToken.compare("dof") == 0) {
			std::getline(is, token);
			std::istringstream dofs(token);			
			//unsigned int ti = 0;
			//unsigned int ri = 0;
			while (dofs >> token) {
				auto channel = DegreeOfFreedom::getChannel(boost::to_upper_copy(token));
				channels.push_back(channel);
				//TODO - czemu ta weryfikacja jest wykomentowana?
				/*if (channel == kinematicUtils::ChannelType::TX || channel == kinematicUtils::ChannelType::TY
				|| channel == kinematicUtils::ChannelType::TZ)
				{
				++ti;
				}
				else if (channel == kinematicUtils::ChannelType::RX || channel == kinematicUtils::ChannelType::RY
				|| channel == kinematicUtils::ChannelType::RZ)
				{
				rotOrder += tokenToAxis(token);
				++ri;
				}*/
			}

			//if (!(ti == 0 || ti == 3) || !(ri == 0 || ri == 3))
			//{
			//	throw std::runtime_error("Invalid acclaim degrees of freedom configuration. Incomplete rotation or translation");
			//}
			//
			//bone.rotationOrder = Axis::getAxisOrder(rotOrder);

		}
		else if (localToken.compare("limits") == 0) {
			for (decltype(channels.size()) i = 0; i < channels.size(); ++i) {
				getline(is, token);
				parseLimit(token, limitValues);
			}
		}
	}
	auto count = channels.size();
	auto limitsCount = limitValues.size();
	if (limitsCount == (2 * count)) {
		for (decltype(count) i = 0; i < count; ++i) {			
			bone.dofs.push_back(DegreeOfFreedom(channels[i], limitValues[2 * i], limitValues[2 * i + 1]));
		}
	}
	else if (limitsCount == 0) {		
		for (decltype(count) i = 0; i < count; ++i) {
			bone.dofs.push_back(DegreeOfFreedom(channels[i]));
		}
	}
	else {
		throw std::runtime_error("dofs size != limits size");
	}
	return true;
}

bool parseBones(const std::string& bones, Skeleton & model, const bool createUniqueIndicies) {
	static const std::string::size_type shift = 5; //std::string("begin").size();	
	std::string boneData;	
	std::string::size_type begin = 0;
	std::string::size_type end = 0;
	int id = 0;
	while(true) {
		begin = bones.find("begin", end);
		end = bones.find("end", begin);
		begin += shift;
		if (begin != std::string::npos && end != std::string::npos && end > begin) {
			boneData = bones.substr(begin, end - begin);
			Bone bone;
			bool boneParse = parseSingleBone(boneData, bone, id, createUniqueIndicies);

			if (boneParse) {
				if (model.bones.find(bone.id) == model.bones.end()) {
					model.bones.insert({ bone.id, bone });
				}
				else {
					throw std::runtime_error("Bone : " + bone.name + " already added");
				}
			}
		}
		else {
			break;
		}
	}

	if (model.bones.empty() == true) {
		throw std::runtime_error("No bones defined");
	}

	return true;
}

bool parseUnits(const std::string& units, Skeleton & model) {
	std::istringstream is(units);
	std::string token;
	std::string value;
	double number;

	while (is >> token) {

		is >> value;
		boost::to_lower(token);
		try{
			number = boost::lexical_cast<double>(value);
			model.units.defaultValues[token] = number;
		}
		catch (...){
			model.units.typeValueUnits[token] = boost::to_lower_copy(value);
		}
	}

	return true;
}

bool parseRoot(const std::string& root, Skeleton& skeleton)
{
	std::istringstream lines(root);

	skeleton.position = osg::Vec3d(0, 0, 0);
	skeleton.orientation = osg::Vec3d(0, 0, 0);
	skeleton.axisOrder = kinematicUtils::AxisOrder::XYZ;

	std::string line;
	
	while (std::getline(lines, line)) {
		if (line.empty() == false){
			std::istringstream is(line);
			std::string token;
			is >> token;
			boost::to_lower(token);

			if (token.compare("axis") == 0) {
				is >> token;
				skeleton.axisOrder = Axis::getAxisOrder(token);
			}
			else if (token.compare("order") == 0) {
				std::vector<kinematicUtils::Channel> dataOrder;
				while (is >> token) {
					dataOrder.push_back(DegreeOfFreedom::getChannel(token));
				}

				skeleton.dataOrder = dataOrder;
			}
			else if (token.compare("position") == 0) {
				is >> skeleton.position[0] >> skeleton.position[1] >> skeleton.position[2];
			}
			else if (token.compare("orientation") == 0) {
				is >> skeleton.orientation[0] >> skeleton.orientation[1] >> skeleton.orientation[2];
			}
			else if (token.size() != 0) {
				//TODO - co sie tutaj powinno dziać?
			}
		}
	}

	return true;
}

bool parseHierarchy(const std::string& hierarchyString, Skeleton & model)
{
	using children = std::vector<std::string>;
	using hierarchyLine = std::pair<std::string, children>;
	std::vector<hierarchyLine> hierarchy;

	std::istringstream stream(hierarchyString);
	std::string l;

	while (std::getline(stream, l)) {
		std::stringstream ss(l);
		std::vector<std::string> tokens;
		std::string buffer;
		while (ss >> buffer) {
			tokens.push_back(buffer);
		}

		hierarchyLine line;
		auto count = tokens.size();
		if (count > 1) {
			line.first = tokens[0];
			for (decltype(count) i = 1; i < count; ++i) {
				line.second.push_back(tokens[i]);
			}
			hierarchy.push_back(line);
		}
	}

	Bone root;
	root.id = model.bones.begin()->first - 1;
	root.name = "root";
	root.axis = model.orientation;
	root.axisOrder = model.axisOrder;
	root.length = 0.0;
	root.direction = model.position;
	root.direction.normalize();

	for (const auto & dO : model.dataOrder) {
		root.dofs.push_back(DegreeOfFreedom(dO));
	}

	model.root = root.id;
	model.bones.insert({ root.id, root });

	for (const auto& line : hierarchy) {
		//TODO - weryfikacja czy coś znaleziono
		auto pit = std::find_if(model.bones.begin(), model.bones.end(), [&line](const Skeleton::Bones::value_type & val)
		{
			return val.second.name == line.first;
		});

		if (pit != model.bones.end()) {			

			for (const auto& name : line.second) {

				auto bit = std::find_if(model.bones.begin(), model.bones.end(), [&name](const Skeleton::Bones::value_type & val)
				{
					return val.second.name == name;
				});

				if (bit != model.bones.end()) {
					
					model.hierarchy.insert({ pit->first, bit->first });
				}
			}
		}
	}

	if (model.bones.size() < 2) {
		throw std::runtime_error("root has no children in hierarchy");
	}

	return true;
}

void saveRoot(std::ostream& out, const Skeleton & model)
{
	out << ":root" << std::endl;
	out << "   order";

	for (const auto & dO : model.dataOrder) {
		out << " " << DegreeOfFreedom::getChannelName(dO, true);
	}
	
	out << std::endl << "   axis " << Axis::getAxisOrderName(model.axisOrder) << std::endl;
	
	out << "   position "
		<< utils::correctNegativeZero(model.position[0]) << " "
		<< utils::correctNegativeZero(model.position[1]) << " "
		<< utils::correctNegativeZero(model.position[2]) << std::endl;

	out << "   orientation "
		<< utils::correctNegativeZero(model.orientation[0]) << " "
		<< utils::correctNegativeZero(model.orientation[1]) << " "
		<< utils::correctNegativeZero(model.orientation[2]) << std::endl;
}

void saveSingleBone(std::ostream& out, const Bone& bone, const Skeleton & model)
{
	out << "  begin" << std::endl;
	out << "     id " << bone.id << std::endl;
	out << "     name " << bone.name << std::endl;
	out << "     direction " <<
		utils::correctNegativeZero(bone.direction[0]) << " " <<
		utils::correctNegativeZero(bone.direction[1]) << " " <<
		utils::correctNegativeZero(bone.direction[2]) << std::endl;

	out << "     length " << bone.length << std::endl;
	out << "     axis " <<
		utils::correctNegativeZero(bone.axis[0]) << " " <<
		utils::correctNegativeZero(bone.axis[1]) << " " <<
		utils::correctNegativeZero(bone.axis[2]) << " " <<
		Axis::getAxisOrderName(bone.axisOrder) <<
		std::endl;	

	if (bone.dofs.empty() == false) {	
		out << std::right << "     dof";
		for (const auto & dof : bone.dofs) {
			out << " " << DegreeOfFreedom::getChannelName(dof.channel, false);
		}
		out << std::endl;
		out << "     limits ";

		auto count = bone.dofs.size();

		for (decltype(count) i = 0; i < count; ++i) {
			i != 0 ? out << "          " : out;
			out << "(";
			if (bone.dofs[i].minLimit == -std::numeric_limits<double>::infinity()) {
				out << "-inf";
			}
			else {
				out << utils::correctNegativeZero(bone.dofs[i].minLimit);
			}
			out << ", ";
			if (bone.dofs[i].maxLimit == std::numeric_limits<double>::infinity()) {
				out << "inf";
			}
			else {
				out << utils::correctNegativeZero(bone.dofs[i].maxLimit);
			}
			out << ")" << std::endl;
		}
	}
	out << "  end" << std::endl;
}

void saveBones(std::ostream& out, const Skeleton & model)
{
	out << ":bonedata" << std::endl;

	for (const auto & b : model.bones) {
		if (b.second.id != model.root) {
			saveSingleBone(out, b.second, model);
		}
	}
}

void saveHierarchy(std::ostream& out, const Skeleton & model)
{
	std::queue<int> childrenBonesIDs;	

	if (model.hierarchy.left.find(model.root) == model.hierarchy.left.end()){
		throw std::runtime_error("root not found in hierarchy");
	}

	std::deque<Bone::ID> bonesID;

	out << ":hierarchy" << std::endl;
	out << "  begin" << std::endl;	
	out << "    root";

	auto range = model.hierarchy.left.equal_range(model.root);

	if (range.first != range.second){

		std::vector<Bone::ID> childrenIDs;

		for (; range.first != range.second; ++range.first)
		{
			auto childID = range.first->get_right();
			auto it = model.bones.find(childID);

			if (it == model.bones.end()){
				throw std::runtime_error("Bone with given id from hierarchy not found in bones map");
			}

			childrenIDs.push_back(childID);
			out << " " << it->second.name;
		}

		std::reverse(childrenIDs.begin(), childrenIDs.end());
		for(const auto & id : childrenIDs) { bonesID.push_front(id); }

		out << std::endl;
	}

	while (bonesID.empty() == false)
	{
		auto boneID = bonesID.front();
		bonesID.pop_front();
		range = model.hierarchy.left.equal_range(boneID);

		if (range.first != range.second){

			out << "    " << model.bones.find(boneID)->second.name;

			std::vector<Bone::ID> childrenIDs;

			for (; range.first != range.second; ++range.first)
			{
				auto childID = range.first->get_right();
				auto it = model.bones.find(childID);

				if (it == model.bones.end()){
					throw std::runtime_error("Bone with given id from hierarchy not found in bones map");
				}

				childrenIDs.push_back(childID);
				out << " " << it->second.name;
			}

			std::reverse(childrenIDs.begin(), childrenIDs.end());
			for(auto id : childrenIDs) { bonesID.push_front(id); }

			out << std::endl;
		}
	}

	out << "  end" << std::endl;
}

void saveUnits(std::ostream& out, const Skeleton & model)
{
	out << ":units" << std::endl;

	for (const auto & u : model.units.typeValueUnits)
	{
		out << "  " << u.first << " " << u.second << std::endl;
	}

	for (const auto & u : model.units.defaultValues)
	{
		out << "  " << u.first << " " << utils::correctNegativeZero(u.second) << std::endl;
	}	
}

std::string tokenToAxis(const std::string& token)
{
	if (token.size() != 2){
		throw std::runtime_error("Wrong token passed");
	}

	auto localToken = boost::to_lower_copy(token);

	switch (localToken[1])
	{
	case 'X':
	case 'Y':
	case 'Z':
		break;
	default:
		throw std::runtime_error("Wrong token passed");
		break;
	}

	return std::string(localToken[1],1);
}

Skeleton AsfParser::parse(std::istream & stream, const bool createUniqueIndicies)
{
	auto flags = stream.flags();
	utils::Cleanup cleanup([&stream, flags] { stream.setf(flags, std::ios::skipws); });
	// nie omijaj bialych znakow
	stream.unsetf(std::ios::skipws);
	// skopiowanie zawartości pliku do pojedynczego stringa
	auto storage = utils::StreamTools::read(stream);
    // usun komentarze z pliku
    storage = removeComments(storage);

	std::string name;           //!< nazwa zapisana w pliku
	std::string documentation;  //!< komentarz do modelu
	std::string version;        //!< wersja wg której został zapisany plik
	std::string units;          //!< sekcja z jednostkami
	std::string root;           //!< sekcja z rootem
	std::string bonedata;       //!< sekcja z wszystkimi kości
	std::string hierarchy;      //!< sekcja z hierarchia
	std::string unknown;        //!< nieznane / nieobslugiwane tokeny

	Skeleton model;

    // podziel plik na sekcje
	bool result = splitAsf(storage, name, documentation, version, units, root,
		bonedata, hierarchy, unknown);
    if (result == true) {
		model.name = name;
		model.version = version;
		model.documentation = documentation;
        result &= parseUnits(units, model);
        result &= parseRoot(root, model);
		result &= parseBones(bonedata, model, createUniqueIndicies);
        result &= parseHierarchy(hierarchy, model);
    }
    if (result == false) {
		throw std::runtime_error("Error in stream");
    }

	return model;
}

std::string serialize(const Skeleton & model)
{
	std::stringstream out;
	out << "# Created by PJATK hmAnimation Parser (v. " << PARSER_VERSION << ")" << std::endl;
	out << "# -----------------------------------------------" << std::endl;
	out << ":version " << model.version << std::endl;
	out << ":name " << model.name << std::endl;
	saveUnits(out, model);
	out << ":documentation" << std::endl << model.documentation << std::endl;
	saveRoot(out, model);
	saveBones(out, model);
	saveHierarchy(out, model);

	return out.str();
}

void AsfParser::serialize(std::ostream & stream, const Skeleton & model) {
	stream << ::serialize(model);
}
