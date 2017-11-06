/********************************************************************
created:  2013/11/18
created:  18:11:2013   12:01
filename: StreamData.h
author:   Mateusz Janiak

purpose:  Baza dla danych strumieniowych
*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__STREAMDATABASE_H__
#define HEADER_GUARD_THREADINGUTILS__STREAMDATABASE_H__

#include <threadingUtils/Export.h>
#include <utils/SmartPtr.h>
#include <limits>
#include <type_traits>
#include <mutex>
#include <functional>
#include <utils/Debug.h>
#include <list>
#include <utility>

namespace threadingUtils {
	//! Interfejs obiektu obserwującego strumień
	//! Implementacja musi gwarantować bezpieczne operacje wielowątkowe
	class IStreamStatusObserver
	{
	public:
		virtual ~IStreamStatusObserver() {}
		//! Metoda wołana kiedy faktycznie stan strumienia się zmieni
		virtual void update() = 0;
	};

	//! Klasa obsługująca odpytywanie statusu strumienia
	//! (czy dane uległy aktualizacji) i resetująca status po sprawdzeniu
	class THREADINGUTILS_EXPORT ResetableStreamStatusObserver : public IStreamStatusObserver
	{
	private:

		//! Wewętrzna implementacja
		class ResetableStreamStatusImpl;
	public:
		//! Domyslny konstruktor
		ResetableStreamStatusObserver();
		//! Destruktor wirtualny
		~ResetableStreamStatusObserver();
		//! \return Czy strumień uległ zmianie
		bool modified() const;
		//! \param sb Strumień aktualizujący mój stan
		virtual void update() override;

	private:
		//! Implementacja
		utils::shared_ptr<ResetableStreamStatusImpl> impl;
	};

	using StreamStatusObserverPtr = utils::shared_ptr<IStreamStatusObserver>;

	//! Klasa bazowa strumienia
	class THREADINGUTILS_EXPORT StreamBase
	{
	protected:
		//! Chroniony konstruktor domyślny
		StreamBase();

		//! Pomiadamia obserwatorów o zmianie
		void notify();

	public:
		//! Destruktor wirtualny
		virtual ~StreamBase();

		//! \param observer Dołączany obiekt obserwujący strumień
		void attach(StreamStatusObserverPtr observer);

		//! \param observer Odłączany obiekt obserwujący strumień
		void detach(StreamStatusObserverPtr observer);

	protected:
		//! Obiekt synchronizujący
		mutable std::recursive_mutex synch_;

	private:
		//! Lista obserwatorów
		std::list<StreamStatusObserverPtr> observers;
	};

	typedef utils::shared_ptr<StreamBase> StreamBasePtr;

	//! Wzorzec klasy obsługującej bufor danych dla strumieni
	//! \tparam T Typ danych strumienia i bufora
	template<typename T>
	class StreamBufferT
	{
	public:

		//! Typ kolekcji danych bufora
		using ListT= std::list<T>;

		//! Typ rozmiaru bufora
		using size_type = typename ListT::size_type;

		//! Typ referencji do danych
		using ref_type = typename std::add_lvalue_reference<T>::type;

		//! Typ stałej referencji do danych
		using const_ref_type = typename std::add_const<ref_type>::type;

	public:
		//! Konstruktor domyslny
		//! \param size Rozmiar bufora cyklicznego - domyślnie 50, 0 oznacza brak ograniczeń na bufor, 1 - wartość niedozwolona
		StreamBufferT(const size_type size = 50) : maxSize(size) {}
		//! Destruktor wirtualny
		~StreamBufferT() {}
		//! \param size Maksymalny rozmiar bufora - 0 lub mniej oznacza brak ograniczeń		
		void setMaxBufferSize(const size_type size)
		{
			std::lock_guard<std::mutex> lock(synch_);
			maxSize = size;
			adjustBufferSize();
		}

		//! \return Maksymalny rozmiar bufora
		size_type maxBufferSize() const
		{
			std::lock_guard<std::mutex> lock(synch_);
			return maxSize;
		}

		//! \return Aktualny rozmiar bufora
		size_type size() const
		{
			std::lock_guard<std::mutex> lock(synch_);
			return bufferData.size();
		}

		//! \param data Dane do włożenia do bufora
		void pushData(const_ref_type data)
		{
			std::lock_guard<std::mutex> lock(synch_);
			bufferData.push_back(data);
			adjustBufferSize();
		}

		//! \param data Dane do włożenia do bufora
		void pushData(T && data)
		{
			std::lock_guard<std::mutex> lock(synch_);
			bufferData.push_back(std::move(data));
			adjustBufferSize();
		}

		//! \param data [out] Obiekt docelowy dla danych bufora
		void data(ListT & data)
		{
			std::lock_guard<std::mutex> lock(synch_);
			data.splice(data.end(), bufferData);
		}

	private:

		//! Metoda dostosowuje rozmiar bufora danych do zadanej wielkości
		void adjustBufferSize()
		{
			const auto s = bufferData.size();
			if ((maxSize > 0) && (s > maxSize)) {
				auto it = bufferData.begin();
				std::advance(it, s - maxSize);
				bufferData.erase(bufferData.begin(), it);
			}
		}

	private:
		//! Obiekt synchronizujący
		mutable std::mutex synch_;
		//! Bufor danych
		ListT bufferData;
		//! Rozmiar bufora
		volatile size_type maxSize;
	};

	template<typename T>
	class IStreamT : public StreamBase
	{
	public:

		//! Typ interfejsu strumienia do czytania
		using stream_reader_type = IStreamT<T>;

		//! Typ bufora
		using StreamBufferType = StreamBufferT<T>;

		//! Typ danych mojego strumienia
		using value_type = T ;

		//! Typ referencji do danych
		using ref_type = typename StreamBufferType::ref_type;

		//! Typ stałej referencji do danych
		using const_ref_type = typename StreamBufferType::const_ref_type;

		//! Typ wskaźnika na bufor
		using StreamBufferPtr = utils::shared_ptr<StreamBufferType>;

	private:

		//! Typ listy buforów
		using StreamBuffersList = std::list<StreamBufferPtr>;

	public:
		//! Destruktor wirtualny
		virtual ~IStreamT() {}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia
		virtual void data(ref_type d) const = 0;

		using StreamBase::attach;
		using StreamBase::detach;

		//! \param bufferPtr Nowy bufor danych aktualizowany danymi strumienia
		void attach(StreamBufferPtr bufferPtr)
		{
			if (bufferPtr == nullptr) {
				throw std::runtime_error("Uninitialized buffer pointer");
			}

			std::lock_guard<std::recursive_mutex> lock(synch_);
			streamBufferList.remove(bufferPtr);
			streamBufferList.push_back(bufferPtr);
		}

		//! \param bufferPtr Odłanczany bufor danych
		void detach(StreamBufferPtr bufferPtr)
		{
			std::lock_guard<std::recursive_mutex> lock(synch_);
			streamBufferList.remove(bufferPtr);
		}

	protected:

		//! Return Czy podłączono jakieś bufory
		bool buffersAttached() const
		{
			return streamBufferList.empty() == false;
		}

		//! \param data Data received from the stream
		void pushBuffersData(const_ref_type data)
		{
			UTILS_ASSERT(streamBufferList.empty() == true, "Stream data with no buffers");

			for (auto & buff : streamBufferList) {
				buff->pushData(data);
			}
		}

	private:
		//! Lista buforów
		StreamBuffersList streamBufferList;
	};

	template<typename T>
	using IStreamPtr = utils::shared_ptr<IStreamT<T>>;

	//! Generyczny obiekt strumienia, synchronizuje dane
	//! \tparam T Typ danych w strumieniu
	template<typename T>
	class StreamT : public IStreamT < T >
	{
	public:
		//! Domyślny konstruktor
		StreamT() {}

		//! \param data Data received from the stream
		void pushData(typename IStreamT<T>::const_ref_type data)
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);

			pushBuffersData(data);

			data_ = data;

			this->notify();
		}

		//! \param data Data received from the stream
		void pushData(T && data)
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);

			pushBuffersData(std::forward<T>(data));

			data_ = std::move(data);

			this->notify();
		}

		//! Destruktor wirtualny
		virtual ~StreamT() {}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia
		virtual void data(typename IStreamT<T>::ref_type d) const override
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);
			d = data_;
		}

	private:
		//! Aktualne dane
		T data_;
	};

	class BaseUpdatebleStream
	{
	public:
		virtual ~BaseUpdatebleStream() {}
		virtual void localUpdate() = 0;
	};

	//! Klasa obserwujaca strumien bazowy
	//! \tparam DestStream Strumień docelowy
	class BaseStreamUpdater : public IStreamStatusObserver
	{
	public:
		//! \param destStream Strumien docelowy
		BaseStreamUpdater(BaseUpdatebleStream * destStream)
			: destStream(destStream)
		{
		}

		//! Lapie zmiane w strumieniu zrodlowym i propaguje na nasz strumien
		virtual void update()
		{
			destStream->localUpdate();
		}

	private:
		//! Strumien docelowy
		BaseUpdatebleStream * destStream;
	};

	class StreamModifierHelper
	{
	public:

		StreamModifierHelper(StreamBasePtr baseStream, BaseUpdatebleStream * destStream)
			: baseStream_(baseStream), sourceStreamObserver(new BaseStreamUpdater(destStream))
		{
			if (baseStream_ == nullptr) {
				throw std::invalid_argument("Uninitialized base stream");
			}

			baseStream_->attach(sourceStreamObserver);
		}

		virtual ~StreamModifierHelper()
		{
			baseStream_->detach(sourceStreamObserver);
		}

	private:
		//! Strumień który przykrywam
		StreamBasePtr baseStream_;
		//! Obserwator strumienia bazowego
		StreamStatusObserverPtr sourceStreamObserver;
	};

	//! Klasa zmieniająca reprezentację strumienia
	template<typename T, typename Filter>
	class StreamFilterT : public IStreamT<T>, private BaseUpdatebleStream
	{
	public:

		template<typename F = Filter, typename TT = T>
		//! \param baseStream Strumień który filtruję
		//! \param filter Obiekt filtrujący zmiany strumienia
		StreamFilterT(IStreamPtr<T> baseStream,
			F && filter = F(), TT && startValue = TT())
			: baseStream(baseStream), modifierHelper(baseStream, this),
			filter_(std::forward<F>(filter)), currentData_(std::forward<TT>(startValue))
		{

		}		

		//! Destruktor wirtualny
		virtual ~StreamFilterT()
		{

		}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia po filtracji
		virtual void data(typename IStreamT<T>::ref_type d) const override
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);
			d = currentData_;
		}

	private:

		//! Realizuje aktualizacje buforów jeśli jest to konieczne + notyfikuje o zmianach
		virtual void localUpdate() override
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);

			T bd;
			baseStream->data(bd);

			if (filter(bd) == true) {
				currentData_ = bd;
				if (this->buffersAttached() == true) {
					pushBuffersData(currentData_);
				}

				this->notify();
			}
		}

	private:
		//! Aktualne dane
		mutable T currentData_;
		//! Obiekt do filtrowania danych danych
		Filter filter_;
		//!
		IStreamPtr<T> baseStream;
		//!
		StreamModifierHelper modifierHelper;
	};

	//! Klasa zmieniająca reprezentację strumienia
	template<typename T, typename Processor>
	class StreamProcessorT : public IStreamT<T>, private BaseUpdatebleStream
	{
	public:

		template<typename P = Processor, typename TT = T>
		//! \param baseStream Strumień który filtruję
		//! \param filter Obiekt filtrujący zmiany strumienia
		StreamProcessorT(IStreamPtr<T> baseStream,
			P && processor = P(), TT && startValue = TT())
			: baseStream(baseStream), modifierHelper(baseStream, this),
			processor_(std::forward<P>(processor)), currentData_(std::forward<TT>(startValue)),
			processReuired(false)
		{

		}

		//! Destruktor wirtualny
		virtual ~StreamProcessorT()
		{

		}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia po filtracji
		virtual void data(typename IStreamT<T>::ref_type d) const override
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);
			if (processReuired == true) {
				processor_(currentData_);
				processReuired = false;
			}

			d = currentData_;
		}

	private:

		//! Realizuje aktualizacje buforów jeśli jest to konieczne + notyfikuje o zmianach
		virtual void localUpdate() override
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);

			baseStream->data(currentData_);

			processReuired = !this->buffersAttached();

			if (processReuired == false) {
				processor_(currentData_);
				pushBuffersData(currentData_);
			}

			this->notify();
		}

	private:
		//! Aktualne dane
		mutable T currentData_;
		//! Obiekt do przetwarzania danych danych
		mutable Processor processor_;
		//! Czy należy przetworzyć dane czy zostały już przetworzone
		volatile mutable bool processReuired;
		//!
		IStreamPtr<T> baseStream;
		//!
		StreamModifierHelper modifierHelper;
	};

	//! Klasa zmieniająca reprezentację strumienia
	template<typename Base, typename Dest, typename Extractor>
	class StreamAdapterT : public IStreamT<Dest>, private BaseUpdatebleStream
	{

	public:

		template<typename E = Extractor, typename D = Dest>
		//! \param baseStream Strumień którego reprezentację zmieniamy
		//! \param extractorFunction Funktor realizujące przepakowanie typów danych
		StreamAdapterT(IStreamPtr<Base> baseStream,
			E && extractor = E(), D && startValue = D())
			: baseStream(baseStream), modifierHelper(baseStream, this),
			extractor_(std::forward<E>(extractor)), currentData_(std::forward<D>(startValue)),
			unpackRequired_(false)
		{

		}		

		//! Destruktor wirtualny
		virtual ~StreamAdapterT()
		{

		}

		//! \param d [out]  Obiekt docelowy dla aktualnych danych ze strumienia po przepakowaniu
		virtual void data(typename IStreamT<Dest>::ref_type d) const override
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);
			if (unpackRequired_ == true) {
				extractor_.extract(baseData_, currentData_);
				unpackRequired_ = false;
			}

			d = currentData_;
		}

	private:

		//! Realizuje aktualizacje buforów jeśli jest to konieczne + notyfikuje o zmianach
		virtual void localUpdate() override
		{
			std::lock_guard<std::recursive_mutex> lock(this->synch_);

			Base bd;
			baseStream->data(bd);

			if (extractor_.verify(bd) == true) {
				unpackRequired_ = !this->buffersAttached();
				if (unpackRequired_ == false) {
					extractor_.extract(bd, currentData_);
					pushBuffersData(currentData_);
				}
				else {
					baseData_ = bd;
				}

				this->notify();
			}
		}

	private:
		//! Aktualne dane do rozpakowania
		mutable Base baseData_;
		//! current data
		mutable Dest currentData_;
		//! Funktor do weryfikacji i wypakowywania danych
		Extractor extractor_;
		//! Czy dane wymagają pobrania ze źródła
		mutable volatile bool unpackRequired_;
		//!
		IStreamPtr<Base> baseStream;
		//!
		StreamModifierHelper modifierHelper;
	};
}

//! Makro definiujące nazwę extractora pola
//! \param memberName Nazwa wypakowywanego pola
#define STREAM_MEMBER_EXTRACTOR_NAME(memberName) \
	StreamMemberExtractor##memberName

//! Makro definiujące klasę wypakowującą zadane pole
//! \param memberName Nazwa wypakowywanego pola
#define STREAM_MEMBER_EXTRACTOR(memberName) \
class STREAM_MEMBER_EXTRACTOR_NAME(memberName){\
public:\
	template<typename SrcType>\
	inline static bool verify(const SrcType &) { return true; }\
	template<typename SrcType, typename DestType>\
	inline static void extract(const SrcType & src, DestType & dest){\
		dest = src.memberName;\
	}\
};

//! Makro definiujące nazwę adaptera dla zadanej klasy bazowej i pola
//! \param baseType Typ bazowy z którego wyciągamy pole
//! \param memberName Nazwa wypakowywanego pola
#define STREAM_MEMBER_ADAPTER_NAME(baseType, memberName) \
	StreamMemberAdapter##baseType##memberName

//! Makro definiujące adapter dla wyciąganego pola
//! \param baseType Typ bazowy z którego wyciągamy pole
//! \param destType Typ docelowy jeśli chcemy wymusić dostępną konwersję
//! \param memberName Nazwa wypakowywanego pola
#define STREAM_MEMBER_ADAPTER_EXT(baseType, destType, memberName) \
using STREAM_MEMBER_ADAPTER_NAME(baseType,memberName) = StreamAdapterT<baseType, destType, STREAM_MEMBER_EXTRACTOR_NAME(memberName)>;

//! Makro definiujące adapter dla wyciąganego pola
//! \param baseType Typ bazowy z którego wyciągamy pole
//! \param memberName Nazwa wypakowywanego pola
#define STREAM_MEMBER_ADAPTER(baseType, memberName) \
	STREAM_MEMBER_ADAPTER_EXT(baseType, decltype(std::declval<baseType>().memberName), memberName);

#endif	//	HEADER_GUARD_THREADINGUTILS__STREAMDATABASE_H__
