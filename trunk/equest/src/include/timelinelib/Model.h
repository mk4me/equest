#ifndef HEADER_GUARD__TIMELINEMODEL_H__
#define HEADER_GUARD__TIMELINEMODEL_H__

#include <utils/NamedTree.h>
#include <utils/SmartPtr.h>
#include <timelinelib/Types.h>
#include <timelinelib/Channel.h>
#include <utils/ObserverPattern.h>
#include <boost/tokenizer.hpp>
#include <timelinelib/State.h>
#include <map>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
	////////////////////////////////////////////////////////////////////////////////

	class Model : public utils::Observable<Model>
	{
	public:

		//! Forward declartation
		class TChannel;
		typedef utils::shared_ptr<TChannel> TChannelPtr;
		typedef utils::shared_ptr<const TChannel> TChannelConstPtr;

		typedef utils::weak_ptr<TChannel> TChannelWPtr;
		typedef utils::weak_ptr<const TChannel> TChannelConstWPtr;

		//! Klasa wprowadzająca hierarchię kanałów, kolejność, obsługę ścieżek
		class TChannel : public NamedTree<Channel, utils::PtrPolicyStd>
		{
		public:
			TChannel(const std::string & name = "UnnamedChannel") : NamedTree<Channel, utils::PtrPolicyStd>(name, Ptr(new Channel()))
			{
			}

			virtual void addChild(const std::string & path)
			{
				NamedTreeBase::addChild<TChannel>(path, -1);
			}

			void addChild(const std::string & path, NamedTreeBase::size_type idx)
			{
				NamedTreeBase::addChild<TChannel>(path, idx);
			}
		};

		typedef TChannel::const_iterator channel_const_iterator;
		typedef TChannel::size_type channel_size_type;

		typedef ConstTags::const_iterator tag_const_iterator;
		typedef ConstTags::size_type tag_size_type;

		typedef Channel::Mask Mask;

	private:

		typedef Tags::iterator tag_iterator;

		//! Mapowanie Kanałów do kanałów z obsługa ścieżek i hierarchią
		typedef std::map<ChannelConstPtr, TChannelConstPtr> ChannelToTChannel;

		typedef std::vector<ChannelConstPtr> ConstChannels;

		//! Typ ścieżki w formacie ścieżka/liść
		typedef std::pair<std::string, std::string> ExtPath;

		typedef std::set<IChannelPtr> IChannels;

	private:

		//! Wszystkie tagi wystepujące w timeline
		Tags allTags;

		//! Wszystkie tagi wystepujące w timeline
		ConstTags constAllTags;

		//! Root dla timeline
		TChannelPtr root;

		//! Root dla timeline
		TChannelConstPtr constRoot;

		//! Mapowanie kanałów logicznych do kanałów w hierarchii
		ChannelToTChannel channelToTChannel;

		//! Aktualny stan modelu
		State state;

		////! Ostatnie zmiany w modelu dla których wywołano notify - ważne tylko podczas notify, potem są resetowane!!
		//int latestChanges;

		IChannels iChannels;

	public:

		//! Konstruktor zerujący
		Model(const std::string & name = "DefaultModel");

		~Model();

		////! Powiadamianie z kasowaniem ostatnich zmian
		//virtual void notify();

		////! \return Ostatnie zmiany w modelu
		//int getLatestChanges() const;

		//! \return Stan modelu
		const State & getState() const;

		//! \param state Stan modelu
		void setState(const State & state);

		//! \return Wewnętrzna maska czasu kanału
		const Mask & getMask() const;

		//! \return początek maski
		double getMaskBegin() const;

		//! \return Koniec maski
		double getMaskEnd() const;

		//! \return Offset czasu - przesunięcie względem 0
		double getOffset() const;

		//! \return długość kanału w sekundach, uwzględniając skale
		double getLength() const;

		//! \return Czas początku w timeline
		double getBeginTime() const;

		//! \return Czas końca w timeline
		double getEndTime() const;

		//! \return Aktualny czas kanału
		double getTime() const;

		//! \return Znormalizowany czas timeline
		double getNormalizedTime() const;

		//! \return Skala czasu kanału
		double getTimeScale() const;

		//! maska dla podkanałów
		//! \return Czy kanał jest aktywny podczas odtwarzania timeline i operacji czasowych
		bool isActive() const;

		//! \param mask Maska czasu realizowana wewnętrznie przez timeline, 0 <= mask <= length
		void setMask(const Mask & mask);

		//! \param maskBegin początek maski - 0 <= maskBegin <= maskEnd
		void setMaskBegin(double maskBegin);

		//! \param maskEnd Koniec maski - maskBegin <= maskEnd <= length
		void setMaskEnd(double maskEnd);

		//! propaguje zmiane na wszystkie aktywne podkanały
		//! sprawdza maske i offset
		//! \param time Aktualny czas timeline, 0 <= time <= length
		//! \param lockHolder Obiekt chcacy aktualizować czas, trzymający blokade
		void setTime(double time);

		//! propaguje zmiane na wszystkie aktywne podkanały
		//! sprawdza maske i offset
		//! \param normTime Aktualny czas timeline, 0 <= time <= length
		//! \param lockHolder Obiekt chcacy aktualizować czas, trzymający blokade
		void setNormalizedTime(double normTime);

		//! modyfikuje skale
		//! \param Aktualna długość timeline
		//virtual void setLength(double length);

		//! \param offset Czas względem 0
		void setOffset(double offset);

		//! \param scale Skala kanału, bezwzględna wartość skali
		void setTimeScale(double scale);

		//! \param active Czy strumień jest aktywny podczas operacji oczasowych i odtwarzania timeline
		void setActive(bool active);

		//! Czysci wszystkie tagi w timeline
		void clearAllTags();

		//! Resetuje skale czasu dla wszystkich kanałów do ich globalnej wartości rownej 1
		void resetAllScales();

		//! Resetuje offsety dla wszystkich kanałów do ich globalnej wartości rownej 0
		void resetAllOffsets();

		//! Resetuje wszystkie offsety (0) i skale (1) w timeline
		void resetAllOffsetsAndScales();

		//! \param path Sciezka do taga w postaci sciezka do kanału sciezki / nazwa taga
		//! \param time Czas wystapienia taga
		void addTag(const std::string & path, double begin = 0, double length = 0);

		//! \param path Sciezka do taga w postaci sciezka do kanału sciezki / nazwa taga
		//! \return Tag
		const TagConstPtr & getTag(const std::string & path) const;

		//! \param path Sciezka do taga w postaci sciezka do kanału sciezki / nazwa taga
		void removeTag(const std::string & path);

		//! \return Pierwszy tag wsrod wszystkich obecnych w timeline
		tag_const_iterator beginAllTags() const;

		//! \return Koniec tagów wsrod wszystkich obecnych w timeline
		tag_const_iterator endAllTags() const;

		//! \param idx Indeks tagu wsrod wqszystkich tagów w timeline
		//! \return Tag o zadanym indeksie wsrod wszystkich obecnych w timeline
		const TagConstPtr & getAllTags(tag_size_type idx) const;

		//! \return Ilość wszystkich tagów w timeline
		tag_size_type sizeAllTags() const;

		//! \param path Sciezka nowego kanału
		//! \param channel fatyczny kanał dotarczony przez klienta
		void addChannel(const std::string & path, const IChannelPtr & channel);

		//! \param channels Kanały do dodawnia w formie ściezka -> kanał, kanał nie powiniem być nullptr
		void addChannels(const std::map<std::string, IChannelPtr> & channels);

		//! \param path Sciezka kanału do usuniecia
		void removeChannel(const std::string & path);

		//! \param paths Sciezki kanałów do usuniecia
		void removeChannels(const std::set<std::string> & paths);

		//! \return Pierwszy kanał timeline
		channel_const_iterator beginChannels() const;

		//! \return Koniec kanałów timeline
		channel_const_iterator endChannels() const;

		//! \param idx Indeks kanału w timeline
		//! \return kanał w timeline o zadanym indeksie
		TChannelConstPtr getChannel(channel_size_type idx) const;

		//! \param path Sciezka do kanału względem roota
		//! \return kanał o zadanej sciezce
		TChannelConstPtr getChannel(const std::string & path) const;

		//! \return Ilość kanałów dzieci w timeline
		channel_size_type sizeChannels() const;

		//------------------- Operacje na kanałach -----------------------
		// ich zmiany sa propagowane w calym timeline a obserwatorzy beda powiadomieni

		//! \param path Sciezka do kanału
		//! \param offset Offset kanału względem rodzica
		void setChannelLocalOffset(const std::string & path, double offset);

		//! \param path Sciezka do kanału
		//! \param offset Offset kanału względem 0
		void setChannelGlobalOffset(const std::string & path, double offset);

		//! \param path Sciezka do kanału
		//! \param timeScale Skala czasu kanału, lokalna względem rodzica
		void setChannelLocalTimeScale(const std::string & path, double timeScale);

		//! \param path Sciezka do kanału
		//! \param timeScale Skala czasu kanału, globalna
		void setChannelGlobalTimeScale(const std::string & path, double timeScale);

		//! \param path Sciezka do kanału
		//! \param name Nazwa kanału uniklalna względem jego rodzica
		void setChannelName(const std::string & path, const std::string & name);

		//! \param path Sciezka do kanału
		//! \param mask Maska czasu dla kanału, musi być w zakresie 0 <= mask <= length
		void setChannelMask(const std::string & path, const Channel::Mask & mask);

		//! \param path Sciezka do kanału
		//! \param maskBegin początek maski czasu dla kanału, musi być w zakresie 0 <= maskBegin <= maskEnd
		void setChannelMaskBegin(const std::string & path, double maskBegin);

		//! \param path Sciezka do kanału
		//! \param maskEnd Koniec maski czasu dla kanału, musi być w zakresie maskBegin <= maskEnd <= length
		void setChannelMaskEnd(const std::string & path, double maskEnd);

		//! \param path Sciezka do kanału
		//! \param active Czy kanał jest aktywny podczas operacji czasowych (odtwarzanie timeline)
		void setChannelActive(const std::string & path, bool active);

		//! \param path Sciezka do kanału
		void clearChannelTags(const std::string & path, bool deeper = false);

		////! \param path Sciezka do kanału
		//void resetChannelOffsets(const std::string & path, bool deeper = false);

		////! \param path Sciezka do kanału
		//void resetChannelTimeScales(const std::string & path, bool deeper = false);

		////! \param path Sciezka do kanału
		//void resetChannelOffsetsAndTimeScales(const std::string & path, bool deeper = false);

		//! \param path Sciezka do kanału
		//! \param time Czas kanału w którym dokona się podzial kanału - 0 < time < length
		//! \param nameA Nazwa młodszej części kanału po podziale, musi być unikalna względem rodzica kanału
		//! \param nameB Nazwa starszej części kanału po podziale, musi być unikalna względem rodzica kanału
		void splitChannel(const std::string & path, double time, const std::string & nameA = "A", const std::string & nameB = "B");

		//! oba kanału musza być w obrebie jednego rodzica
		//! \param pathA Pierwszy kanał do scalenia
		//! \param pathB Drugi kanał do scalenia
		//! \param name Nazwa kanału po połączeniu
		void mergeChannels(const std::string & pathA, const std::string & pathB, const std::string & name = "Merged");

		//--------------------------- Zarzadzanie tagami ------------------------------------------------

		//! \param path Sciezka do taga w postaci sciezka do kanału sciezki / nazwa taga
		//! \param name Nazwa tagu
		void setTagName(const std::string & path, const std::string & name);

		//! \param path Sciezka do taga w postaci sciezka do kanału sciezki / nazwa taga
		//! \param time Czas wystapienia tagu w kanale - 0 <= time <= length
		void setTagTime(const std::string & path, double time);

		//! \param path Sciezka do taga w postaci sciezka do kanału sciezki / nazwa taga
		//! \param length długość trwania tagu tak aby begin >= 0 && begin + length <= channel->length
		void setTagLength(const std::string & path, double length);

		//! \param path Sciezka do taga w postaci sciezka do kanału sciezki / nazwa taga
		//! \param dTime Czas przesuniecia taga w kanale tak aby po zastosowaniu begin >= 0 && begin + length <= channel->length
		void shiftTag(const std::string & path, double dTime);

		//-------------------- Pomocnicze funkcje ----------------------------

		//! \param channel Reprezentacja węzła w postaci NamedTreeBase
		//! \return wskaźnik na Channel
		static TChannelConstPtr toTChannel(const NamedTreeBaseConstPtr & channel);

		//! \param channel kanał wynikający z elementów jak tag czy selection
		//! \return kanał timeline opakowujący zadany kanał logiczny
		TChannelConstPtr getTChannelForChannel(const ChannelConstPtr & channel) const;

		//! \param path Sciezka do szukanego kanału
		//! \return wskaźnik do szukanego kanału lub pusty wskaźnik jeśli nie znaleziono
		TChannelConstPtr findChannel(const std::string & path) const;

	private:

		static void getAllChildrenData(const TChannelConstPtr & channel, ConstTags & tags, ConstChannels & channels);

		//! \param channel kanał danych
		//! \param offset Offset kanału względem rodzica
		void innerSetChannelLocalOffset(const TChannelConstPtr & channel, double offset);

		//! \param channel kanał danych
		//! \param offset Offset kanału względem 0
		void innerSetChannelGlobalOffset(const TChannelConstPtr & channel, double offset);

		//! \param channel kanał danych
		void innerRemoveChannel(const TChannelConstPtr & channel);

		//! \param channel kanał danych
		//! \param timeScale Skala czasu kanału, lokalna względem rodzica
		void innerSetChannelLocalTimeScale(const TChannelConstPtr & channel, double timeScale);

		//! \param channel kanał danych
		//! \param timeScale Skala czasu kanału, globalna
		void innerSetChannelGlobalTimeScale(const TChannelConstPtr & channel, double timeScale);

		//! \param channel kanał danych
		//! \param name Nazwa kanału uniklalna względem jego rodzica
		void innerSetChannelName(const TChannelConstPtr & channel, const std::string & name);

		//! \param channel kanał danych
		//! \param mask Maska czasu dla kanału, musi być w zakresie 0 <= mask <= length
		void innerSetChannelMask(const TChannelConstPtr & channel, const Channel::Mask & mask);

		//! \param channel kanał danych
		//! \param maskBegin początek maski czasu dla kanału, musi być w zakresie 0 <= maskBegin <= maskEnd
		void innerSetChannelMaskBegin(const TChannelConstPtr & channel, double maskBegin);

		//! \param channel kanał danych
		//! \param maskEnd Koniec maski czasu dla kanału, musi być w zakresie maskBegin <= maskEnd <= length
		void innerSetChannelMaskEnd(const TChannelConstPtr & channel, double maskEnd);

		//! \param channel kanał danych
		//! \param active Czy kanał jest aktywny podczas operacji czasowych (odtwarzanie timeline)
		void innerSetChannelActive(const TChannelConstPtr & channel, bool active);

		//! \param channel kanał danych
		void innerClearChannelTags(const TChannelConstPtr & channel, bool deeper = false);

		//! \param channel kanał danych
		//! \param time Czas kanału w którym dokona się podzial kanału - 0 < time < length
		//! \param nameA Nazwa mlodszej części kanału po podziale, musi być unikalna względem rodzica kanału
		//! \param nameB Nazwa starszej części kanału po podziale, musi być unikalna względem rodzica kanału
		void innerSplitChannel(const TChannelConstPtr & channel, double time, const std::string & nameA = "A", const std::string & nameB = "B");

		//! oba kanały musza być w obrebie jednego rodzica
		//! \param channelA Pierwszy kanał do scalenia
		//! \param channelB Drugi kanał do scalenia
		//! \param name Nazwa kanału po połączeniu
		void innerMergeChannels(const TChannelConstPtr & channelA, const TChannelConstPtr & channelB, const std::string & name = "Merged");

		//! \param path Sciezka rozszerzona, gdzie ostatni element sciezki to nazwa, prawdziwa sciezka do kanału znajduje się wczesniej
		//! \return Podzielona sciezka na nazwę i sciezke
		static ExtPath getExtPath(const std::string & path);

		//! Konwertuje stale kanały podawane przez klienta na kanały które mozna modyfikowac
		//! sciaga modyfikatro const!!
		//! \param channel kanał do konwersji na wersje do zapisu
		static TChannelPtr getWritableTChannel(const TChannelConstPtr & channel);

		//! Konwertuje stale kanały podawane przez klienta na kanały które mozna modyfikowac
		//! sciaga modyfikatro const!!
		//! \param channel kanał do konwersji na wersje do zapisu
		static TChannelPtr getWritableTChannel(const NamedTreeBaseConstPtr & channel);

		//! Konwertuje stale kanały podawane przez klienta na kanały które mozna modyfikowac
		//! sciaga modyfikatro const!!
		//! \param channel kanał do konwersji na wersje do zapisu
		static ChannelPtr getWritableChannel(const ChannelConstPtr & channel);

		//! Konwertuje stale kanały podawane przez klienta na kanały które mozna modyfikowac
		//! sciaga modyfikatro const!!
		//! \param channel kanał do konwersji na wersje do zapisu
		static ChannelPtr getWritableChannel(const TChannelConstPtr & channel);

		//! Konwertuje stale tagi podawane przez klienta na tagi które mozna modyfikowac
		//! sciaga modyfikatro const!!
		//! \param tag Tag do konwersji na wersje do zapisu
		static TagPtr getWritableTag(const TagConstPtr & tag);

		//! \param ratio względna zmiana skali do zastosowania na skalach dzieci
		void updateChildrenScale(const TChannelConstPtr & child, double ratio);

		//! \param child Dziecko którego długość ulegla zmianie
		static void updateParentLength(const TChannelConstPtr & parent, const TChannelConstPtr & child);

		//! \param channel kanał któremu uaktualniamy długość
		//! \return Czy długość kanału ulegla zmianie
		static bool refreshChannelLength(const TChannelConstPtr & channel);

		//! \param dOffset Przyrost offsetu globalnego dzieci
		static void updateChildrenOffset(const TChannelConstPtr & child, double dOffset);

		static void updateParentOffset(const TChannelConstPtr & parent, const Model::TChannelConstPtr & child);

		//! \param ratio Zamiana skali wplywająca na pozycje Tagow, zaznaczen i masek
		static void updateForScaleRatio(const ChannelPtr & channel, double ratio);
	};

	////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINEMODEL_H__
