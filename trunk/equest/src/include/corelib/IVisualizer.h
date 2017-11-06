/*******************************************************************
	created:  2011/03/14
	created:  14:3:2011   16:49
	filename: IVisualizer.h
	author:	  Piotr Gwiazdowski

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_CORE__IVISUALIZER_H__
#define HEADER_GUARD_CORE__IVISUALIZER_H__

#include <string>
#include <atomic>
#include <utils/SmartPtr.h>
#include <corelib/Variant.h>
#include <corelib/IIdentifiable.h>
#include <corelib/Version.h>
#include <threadingUtils/StreamData.h>
#include <QtGui/QPixmap>

class QObject;
class QWidget;
class QIcon;

namespace plugin
{
	//! Interfejs wizualizatorów dla typów danych zarejestrowanych w aplikacji
	class IVisualizer : public ICoreElement
	{
	public:

		//! !!UWAGA!! Wszystykie operacje na seriach danych muszą być robione z poziomu UI, wyjątkiem jest update
		class ISerie
		{
		private:
			//! Metoda wywoływana na seri danych kiedy dane się zmienią - wymusza odrysowanie
			virtual void update() = 0;

		protected:
			ISerie(const bool update = true) : update_(update) {}
		public:
			//! Destrutkor prywatny
			virtual ~ISerie() {}
			//! \param name Nazwa serii danych do ustawienia
			virtual void setName(const std::string & name) = 0;
			//! \return Nazwa serii danych
			virtual const std::string getName() const = 0;
			//! \return Dane serii
			virtual const core::VariantConstPtr & getData() const = 0;
			//! \return Typ, jak mają być traktowane dane - może chcę pokazać dane z niższego typu
			virtual const utils::TypeInfo & getRequestedDataType() const = 0;
			//! Próbujemy odświeżać dane jeśli jest to konieczne
			bool tryUpdate()
			{
				const bool needUpdate = update_.exchange(false);					

				if (needUpdate == true){
					update();					
				}

				return needUpdate;
			}

			//! Metoda zaznacza potrzebę odświeżenia serii danych
			void requestUpdate()
			{				
				update_ = true;
			}

		private:
			//! Czy wymagana jest aktualizacja serii
			std::atomic<bool> update_;
		};
		//! Seria o charakterze czasowym - pozwala manipulować czasem w serii danych
		//! setTime może być wołane poza watkiem UI - trzeba to uwzględniać przy update
		class ITimeAvareSerieFeatures
		{
		public:
			//! Destruktor wirtualny
			virtual ~ITimeAvareSerieFeatures() {}

			//! \param time Aktualny czas serii - najprawdopodobniej timeline będzie go ustawiał
			virtual void setTime(double time) = 0;
			//! \return Czas trwania serii
			virtual double getLength() const { return getEnd() - getBegin(); }
			//! \return Początek czasu
			virtual double getBegin() const { return 0.0; }
			//! \return Koniec czasu
			virtual double getEnd() const = 0;
		};

		//! Dla serii o charakterze czasowym - czy można edytować skalę czasu i offset
		class ITimeEditableSerieFeatures
		{
		public:
			//! Destruktor wirtualny
			virtual ~ITimeEditableSerieFeatures() {}

			//! \param offset Przesunięcie serii względem aktualnego stanu (pierwsze względem 0)
			virtual void setOffset(double offset) = 0;
			//! \param scale Zmienia skalę serii danych
			virtual void setScale(double scale) = 0;
		};

		//! Interfejs opisujący akcje vizualizatorów dla serii danych
		class IAction : public plugin::IIdentifiable
		{
		public:
			//! Destruktor wirtualny
			virtual ~IAction() {}
			//! \param visualizer Wizualizator dla którego tworzona jest akcja
			//! \return Instancja akcji
			virtual IAction * create(IVisualizer * visualizer) = 0;
			//! \return Nazwa akcji
			virtual QString name() const = 0;
			//! \param serie Dodawana seria danych			
			virtual void addSerie(ISerie * serie) = 0;
			//! \param serie Usuwana seria danych			
			virtual void removeSerie(ISerie * serie) = 0;
			//! \param serie Aktywna seria dancyh
			virtual void setActiveSerie(ISerie * serie) {}
			//! \return Widget konfiguracyjny
			virtual QWidget * configurationWidget() { return nullptr; }
			//! \return Opis akcji
			virtual QString description() const = 0;
			//! \return Wersja akcji
			virtual core::Version version() const = 0;
		};


	public:
		//! Destruktor wirtualny
		virtual ~IVisualizer() {}

		//! Przeładowana - tworzy nowy wizualizator
		virtual IVisualizer * create() const = 0;

		//! Tylko tutaj powinno następować tworzenie widgetu. Metoda wywoływana tylko jeden raz.
		//! To wizualizator musi niszczyć widget w destruktorze. Gdy widget jest równocześnie
		//! widokiem OSG to wystarczy przypisać go do osg::ref_ptr.
		//! \return Widget będący korzeniem wizualizacji
		virtual QWidget* createWidget() = 0;
		//! Tworzy ikonę dla zadanego wizualizatora. Może zwracać nullptr, chociaż to niewskazane.
		//! W odróżnieniu od createWidget ikona przejmowana jest na własność.
		virtual QIcon* createIcon() = 0;
		//! \return Screenshot z wizualizatora
		virtual QPixmap takeScreenshot() const = 0;
		//! Aktualizacja wyświetlania. NIE aktualizacja stanu wyświetlanych danych.
		//! \param deltaTime Zmiana czasu od ostatniego update,
		//! czas ujemny oznacza update serwisowy - po create serie, removeSerie, setActiveSerie oraz innerUpdateRequired
		virtual void update(double deltaTime) = 0;
		//! \param requestedType Typ danych który faktycznie ma być przedstawiony w serii
		//! \param data Dane które będą brane do wizualizaowania w serii
		//! \return Seria danych która można ustawiac - nazwa i dane, nie zarządza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
		virtual ISerie* createSerie(const utils::TypeInfo & requestedType, const core::VariantConstPtr & data) = 0;
		//! \param serie Seria bazowa której konfigurację powielamy
		//! \return Nowa seria danych na bazie zadanej serii - powiela odwołąnie do danych
		virtual ISerie* createSerie(const ISerie* serie, const utils::TypeInfo & requestedType, const core::VariantConstPtr & data) = 0;
		//! \param serie Seria bazowa której konfigurację powielamy
		//! \return Nowa seria danych na bazie zadanej serii - powiela odwołąnie do danych
		virtual ISerie* createSerie(const ISerie* serie) = 0;

		//! \param serie Seria danych do usunięcia, nie powinien usuwać tej serii! Zarządzamy nią my!!
		virtual void removeSerie(ISerie* serie) = 0;

		//! Ustawia daną serię aktywną
		virtual void setActiveSerie(ISerie * serie) = 0;
		//! \return Pobiera aktywną serię, nullptr gdy nie ma żadnej aktywnej
		virtual const ISerie * getActiveSerie() const = 0;
		//! \return Pobiera aktywną serię, nullptr gdy nie ma żadnej aktywnej
		virtual ISerie * getActiveSerie() = 0;

		//! \param supportedTypes [out] Lista typów wspieranych przez dany wizualizator, z których potrafi stworzyć serię danych
		virtual void getSupportedTypes(utils::TypeInfoList & supportedTypes) const = 0;
		//----------------- Obsługa serii danych ---------------------
		//! Wizualizator musi zapewnic ze da się stworzyć MAX serii za pomoca metody createSerie. Ilos serii jest dynamicznie zarządzana z zewnątrz poprzez create i remove serie.
		//! ISerie to klasa delegata, która implementuje specyficzne dla danego wizualizatora operacje ustawiania nazwy serii i jej danych. Kazdy wizualizator może inaczej ustawiac te informacje i prezentować je.
		//! \return Maksymalna ilość serii danych jaka może obsłużyc wizualizator, wartość 0 i mniej oznacza maksumalny zakres dla INT, inna wartość stanowi górna granice
		virtual int getMaxDataSeries() const = 0;
		//! \return Czy wymagany jest wewnętrzny update
		virtual bool innerUpdateRequired() { return false; }
	};

	DEFINE_SMART_POINTERS(IVisualizer);

	//! Klasa pomocnicza przy odświeżaniu danych strumieniowych obsługiwanych przez serię danych
	class StreamDataSerieUpdater : public threadingUtils::IStreamStatusObserver
	{
	public:
		//! \param serie Seria danych obsługująca dane strumieniowe
		StreamDataSerieUpdater(IVisualizer::ISerie * serie)
			: serie(serie)
		{
		}

		//! Destruktor wirtualny
		virtual ~StreamDataSerieUpdater() {}

		//! Metoda wołana kiedy faktycznie stan strumienia się zmieni
		virtual void update()
		{
			serie->requestUpdate();
		}

	private:
		//! Seria którą odświeżamy
		IVisualizer::ISerie * serie;
	};

} // namespace plugin

#endif  // HEADER_GUARD_CORE__IVISUALIZER_H__
