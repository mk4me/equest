/********************************************************************
    created:  2012/11/20
    created:  20:11:2012   9:28
    filename: Serializable.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___SERIALIZABLE_H__
#define HEADER_GUARD___SERIALIZABLE_H__

#include <string>
#include <type_traits>

namespace utils{

class Serializable
{
public:
	virtual std::string serialize() const = 0;
	virtual void deserialize(const std::string &) = 0;

	template<class T>
	static inline bool isSerializable(T * obj = nullptr)
	{
		return std::is_base_of<Serializable, T>();
	}
};

}

#endif	//	HEADER_GUARD___SERIALIZABLE_H__
