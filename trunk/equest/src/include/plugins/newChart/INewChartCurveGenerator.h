/********************************************************************
	created:  2016/08/26 11:10
	filename: INewChartCurveGenerator.h
	author:	  Wojciech Knieć
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_PLUGIN_NEWCHART__INEWCHARTCURVEGENERATOR_H__
#define HEADER_GUARD_PLUGIN_NEWCHART__INEWCHARTCURVEGENERATOR_H__


#include <plugins/c3d/C3DChannels.h>

class INewChartCurveGenerator
{
public:
	struct Param
	{
		Param(const std::string& n, const QVariant& val) : name(n), value(val) {}
		std::string name;
		QVariant value;
	};

	typedef std::vector<Param> Params;
public:
	virtual ~INewChartCurveGenerator() {}
	virtual std::string getName() const = 0;
	virtual Params getDefaultParams() const = 0;
	virtual std::pair<c3dlib::ScalarChannelReaderInterfacePtr, std::string> generate(const c3dlib::ScalarChannelReaderInterfaceConstPtr& sc, const Params&) const = 0;
};
DEFINE_SMART_POINTERS(INewChartCurveGenerator)

namespace GenericGeneratorImpl {

	template <class T>
	struct Type2Variant {
		QVariant val = QVariant(T());
	};

	typedef INewChartCurveGenerator IG;

	template <int N, class ...T> struct paramsGenerator;

	template <int N>
	struct paramsGenerator < N > {
		IG::Params val;
	};

	template <int N, class T>
	struct paramsGenerator < N, T > {
		paramsGenerator() {
			val = { { std::string("Param_") + std::to_string(N), Type2Variant<T>().val } };
		}
		IG::Params val;

	};


	template <int N, class T, class... V>
	struct paramsGenerator < N, T, V... > {
		paramsGenerator()
		{
			auto a = paramsGenerator<N, T>().val;
			auto b = paramsGenerator<N + 1, V...>().val;
			a.insert(a.end(), b.begin(), b.end());
			val = a;
		}

		IG::Params val;
	};


	template <int N, class T, class... Types>
	struct getNthT{
		typedef typename getNthT<N - 1, Types...>::type type;
	};

	template < class T, class... Types>
	struct getNthT < 0, T, Types... > {
		typedef T type;
	};

	template <int Idx, int N, class... Types>
	struct caller
	{
		template <class Pr, class... Args>
		static c3dlib::ScalarChannelReaderInterfacePtr call(Pr f, const c3dlib::ScalarChannelReaderInterfaceConstPtr& ch, const IG::Params& params, Args... args)
		{
			IG::Param p = params[N - 1 - Idx];
			typedef getNthT<N - 1 - Idx, Types...>::type nthType;
			return caller<Idx + 1, N, Types...>::call<Pr>(f, ch, params, qvariant_cast<nthType>(p.value), args...);
		}
	};

	template <int N, class... Types>
	struct caller < N, N, Types... > {
		template <class Pr, class... Args>
		static c3dlib::ScalarChannelReaderInterfacePtr call(Pr f, const c3dlib::ScalarChannelReaderInterfaceConstPtr& ch, const IG::Params& params, Args... args)
		{
			return f(ch, args...);
		}
	};

	template <class... Args>
	struct packCaller {
		template<class Pr>
		static c3dlib::ScalarChannelReaderInterfacePtr run(Pr f, const c3dlib::ScalarChannelReaderInterfaceConstPtr& ch, const IG::Params& params)
		{
			return caller<0, sizeof...(Args), Args...>::call<Pr>(f, ch, params);
		}
	};
}

template<class Base, class... Args>
class GenericGenerator : public INewChartCurveGenerator
{
public:
	virtual std::string getName() const
	{
		return "generic generator";
	}

	virtual Params getDefaultParams() const
	{
		return GenericGeneratorImpl::paramsGenerator<0, Args...>().val;
	}
	virtual std::pair<c3dlib::ScalarChannelReaderInterfacePtr, std::string> generate(const c3dlib::ScalarChannelReaderInterfaceConstPtr& sc, const Params& p) const
	{
		return std::make_pair(GenericGeneratorImpl::packCaller<Args...>::run(&Base::generator, sc, p), getName());
	}
};


#endif