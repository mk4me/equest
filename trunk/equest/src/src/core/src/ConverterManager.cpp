#include <CorePCH.h>
#include <ConverterManager.h>
#include <algorithm>

using namespace core;

ConverterManager::ConverterManager()
{

}

ConverterManager::~ConverterManager()
{

}

void ConverterManager::convert(const utils::ConstObjectsList & input, utils::ConstObjectsList & output, bool recursive) const
{
	
}

void ConverterManager::convert(const utils::ConstObjectsList & input, utils::ConstObjectsList & output, const utils::TypeInfoSet & requestedTypes, bool recursive) const
{

}

void ConverterManager::converters(Converters & converters) const
{	
	//converters.insert(converters.end(), converters_.begin(), converters_.end());
}

void ConverterManager::convertersForInputs(const utils::TypeInfoList & inTypes, plugin::IConverterManager::Converters & converters) const
{
	std::set<plugin::IConverterConstPtr> conv;
	for(auto it = convertersByInputs_.begin(); it != convertersByInputs_.end(); ++it){
		if(std::find(inTypes.begin(), inTypes.end(), it->first) != inTypes.end()){
			conv.insert(it->second.begin(), it->second.end());
		}
	}

	converters.insert(converters.end(), conv.begin(), conv.end());
}

void ConverterManager::convertersWithOutputs(const utils::TypeInfoSet & outTypes, plugin::IConverterManager::Converters & converters) const
{
	std::set<plugin::IConverterConstPtr> conv;
	for(auto it = convertersByOutputs_.begin(); it != convertersByOutputs_.end(); ++it){
		if(outTypes.find(it->first) != outTypes.end()){
			conv.insert(it->second.begin(), it->second.end());
		}
	}

	converters.insert(converters.end(), conv.begin(), conv.end());
}

void ConverterManager::registerConverter(const plugin::IConverterPtr & converter)
{
	for(auto it = converters_.begin(); it != converters_.end(); ++it){
		if(it->first->getID() == converter->getID()){
			throw std::runtime_error("Converter with given ID already registered.");
		}
	}

	ConverterData data;

	fillConverterData(converter, data);

	for(auto it = converters_.begin(); it != converters_.end(); ++it){
		if(it->second.inputTypesIndexes.size() == data.inputTypesIndexes.size() &&
			std::equal(data.inputTypesIndexes.begin(), data.inputTypesIndexes.end(),
			it->second.inputTypesIndexes.begin(), compareInputs)){

			CORE_LOG_INFO("Converter " << converter->getID() << " registers same inputs as converter " << it->first->getID());
		}
	}

	for(auto it = data.inputTypes.begin(); it != data.inputTypes.end(); ++it){
		convertersByInputs_[*it].push_back(converter);
	}

	for(auto it = data.outputTypes.begin(); it != data.outputTypes.end(); ++it){
		convertersByOutputs_[*it].push_back(converter);
	}

	converters_.insert(ConvertersDescription::value_type(converter, data));

	CORE_LOG_INFO("Converter " << converter->getID() << " registered.");
}

void ConverterManager::fillConverterData(const plugin::IConverterPtr & converter, ConverterData & data)
{
	utils::TypeInfoVector inputs;
	utils::TypeInfoVector outputs;

	converter->description(inputs, outputs);

	if(inputs.empty() == true){
		throw std::runtime_error("Converter with no input description.");
	}

	if(outputs.empty() == true){
		throw std::runtime_error("Converter with no output description.");
	}

	for(int i = 0; i < inputs.size(); ++i){
		auto type = inputs[i];
		data.inputTypesIndexes[type].insert(i);
		data.inputTypes.insert(type);
	}

	for(int i = 0; i < outputs.size(); ++i){
		auto type = outputs[i];
		data.outputTypesIndexes[type].insert(i);
		data.outputTypes.insert(type);
	}

	data.inputSize = inputs.size();
}

const bool ConverterManager::compareInputs(const DataTypeToIndex::value_type & first, const DataTypeToIndex::value_type & second)
{
	if(first.first == second.first && first.second.size() == second.second.size()){
		return true;
	}

	return false;
}

void ConverterManager::generateConverterInputs(const ConverterData & converterDescription, const utils::ConstObjectsList & data, std::list<utils::ConstObjectsVector> & conversionData)
{
	//! Mapowanie typów do typów kompatybilnych wynikaj¹ce z opisu wejœc i danych
	typedef std::map<utils::TypeInfo, utils::TypeInfoSet> TypesCompatibilityMap;

	// Czy mamy doœæ typów danych by zasiliæ wejœcia?
	if(converterDescription.inputSize <= data.size()){

		// Mapujemy dane wejœciowe wg typów ¿eby ³atwiej dopasowaæ do oczekiwanego wejœcia i zweryfikowaæ czy ich typy
		// mog¹ faktycznie zasiliæ konwerter
		DataByTypes objectsByTypes;
		utils::TypeInfoSet avaiableTypes;
		for(auto it = data.begin(); it != data.end(); ++it){
			objectsByTypes[(*it)->getTypeInfo()].push_back(*it);
			avaiableTypes.insert((*it)->getTypeInfo());
		}

		TypesCompatibilityMap dataCompatybilityTypes;

		// Budujemy mape typow wejscia i kompatybilnych typow w danych - bedziemy z tego generowac mozliwe wejsica konwertera
		// iterujemy po typach danych
		for(auto aIT = avaiableTypes.begin(); aIT != avaiableTypes.end(); ++aIT){
			// iterujemy po typach wejscia
			for(auto cIT = converterDescription.inputTypes.begin(); cIT != converterDescription.inputTypes.end(); ++cIT){
				// sprawdzamy czy kompatybilne
				if(getDataHierarchyManager()->isTypeCompatible(*cIT, *aIT) == true){
					// zapamietujemy kompatybilne pary wejscie -> dane
					dataCompatybilityTypes[*cIT].insert(*aIT);
				}
			}
		}

		// czy mamy dosc typow danych by zasilic wejscie konwertera?
		if(dataCompatybilityTypes.size() == converterDescription.inputTypes.size()){

			DataByTypes dataForInput;

			// Buduje mapowanie danych do wejsc, gwarantujac ze dany obiekt na danym wejsciu bedzie dostarczal wymaganego typu
			for(auto cIT = dataCompatybilityTypes.begin(); cIT != dataCompatybilityTypes.end(); ++cIT){

				utils::ConstObjectsList tData;
				for(auto dIT = cIT->second.begin(); dIT != cIT->second.end(); ++dIT){
					const auto & objs = objectsByTypes[*dIT];
					//tData.insert(objs.begin(), objs.end());
				}

				//dataForInput.insert(DataByTypes::value_type(*cIT, tData));
			}

			//mozemy przystapic do generowania mozliwych wejsc konwertera
			generateConverterInputs(dataForInput, conversionData);
			
		}
	}
}

void ConverterManager::generateConverterInputs(const DataByTypes & input, std::list<utils::ConstObjectsVector> & conversionData)
{
	std::map<utils::TypeInfo, utils::ConstObjectsList::iterator> generationState;

	// inicjalizujemy pierwszy stan generatora
	/*for(auto iIT = input.begin(); iIT != input.end(); ++iIT){
		generationState[iIT->first] = iIT->second.begin();
	}*/

	//weryfikujemy ze faktycznie mozna cos generowac - nigdzie nie brakuje danych

	// kontynujemy tak d³ugo a¿ wyczerpiemy wszystkie mo¿liwoœci
	do {


	} while(true);
}