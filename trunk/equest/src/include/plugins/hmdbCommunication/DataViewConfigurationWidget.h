/********************************************************************
	created:  2014/09/17	17:45:49
	filename: DataViewConfigurationWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWCONFIGURATIONWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWCONFIGURATIONWIDGET_H__

#include <plugins/hmdbCommunication/Export.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <utils/SmartPtr.h>
#include <plugins/hmdbCommunication/ShallowCopyFilter.h>
#include <plugins/hmdbCommunication/IDataSourcePerspective.h>
#include <plugins/hmdbCommunication/IDataSourceContent.h>

namespace Ui
{
	class DataViewConfigurationWidget;
}

class HMDBCOMMUNICATION_EXPORT DataViewConfigurationWidget : public QWidget
{
	Q_OBJECT

public:

	//! \param parent Widget rodzica
	//! \param f Flagi okna
	DataViewConfigurationWidget(QWidget * parent = nullptr, Qt::WindowFlags f = 0);
	//! Destruktor wirtualny
	virtual ~DataViewConfigurationWidget();

	//! \return Aktualny filtr - mo¿e byæ nullptr
	hmdbCommunication::ShallowCopyFilter * currentFilter();
	//! \return Aktualna perspektywa danych - nie mo¿e byæ nullptr
	hmdbCommunication::IDataSourcePerspective * currentPerspective();
	//! \return Aktualny content danych - nie mo¿e byæ nullptr
	hmdbCommunication::IDataSourceContent * currentContent();

	//! \param filter Rejestrowany filtr
	void registerFilter(hmdbCommunication::ShallowCopyFilter * filter);
	//! Dodaje przerwê miêdzy filtrami
	void addFilterSeparator();
	//! \param perspective Rejestrowana perspektywa
	void registerPerspective(hmdbCommunication::IDataSourcePerspective * perspective);
	//! Dodaje przerwê miêdzy perspektywami
	void addPerspectiveSeparator();
	//! \param filter Rejestrowany content
	void registerContent(hmdbCommunication::IDataSourceContent * content);
	//! Dodaje przerwê miêdzy contentami
	void addContentSeparator();
	//! Czyœci filtry
	void clearFilters();
	//! Czyœci perspektywy
	void clearPerspectives();
	//! Czyœci contenty
	void clearContents();
	//! \return Iloœæ filtrów
	const unsigned int filtersCount() const;
	//! \return Iloœæ perspektyw
	const unsigned int perspectivesCount() const;
	//! \return Iloœæ contentów
	const unsigned int contentsCount() const;

private slots:

	//! \param idx Aktualny indeks combo filtra
	void onFilterChange(int idx);
	//! \param idx Aktualny indeks combo perspektywy
	void onPerspectiveChange(int idx);
	//! \param idx Aktualny indeks combo contentów
	void onContentChange(int idx);

public slots:

	//! \param visible Czy widaæ panel filtrów
	void setFilterVisible(const bool visible);
	//! \param enable Czy panel filtrów aktywny
	void setFilterEnabled(const bool enable);
	//! \param idx Indeks aktualnego filtra
	void setCurrentFilter(const int idx);
	//! \param visible Czy widaæ panel perspektyw
	void setPerspectiveVisible(const bool visible);
	//! \param enable Czy panel perspektyw aktywny
	void setPerspectiveEnabled(const bool enable);
	//! \param idx Indeks aktualnej perspektywy
	void setCurrentPerspective(const int idx);
	//! \param visible Czy widaæ panel contentów
	void setContentVisible(const bool visible);
	//! \param enable Czy panel contentów aktywny
	void setContentEnabled(const bool enable);
	//! \param idx Indeks aktualnego contentu
	void setCurrentContent(const int idx);

signals:
	//! Nast¹pi³a zmiana filtra
	void filterChanged();
	//! Nast¹pi³a zmiana perspektywy
	void perspectiveChanged();
	//! Nast¹pi³a zmiana contentu
	void contentChanged();

private:

	template<class T>
	class ItemHelper
	{
	public:
		ItemHelper(QLabel * label, QComboBox * combo) : label(label),
			combo(combo), currentIDX(0), currentItem_(nullptr)
		{

		}

		~ItemHelper()
		{

		}

		void add(const QString & name)
		{
			++currentIDX;
			combo->addItem(name);
		}

		void add(T * item)
		{
			ItemDescription id = { combo->count(), item };
			mapping[currentIDX++] = id;
			combo->addItem(item->name());
		}

		void clear()
		{
			currentItem_ = nullptr;
			currentIDX = 0;
			std::map<unsigned int, ItemDescription>().swap(mapping);
			combo->clear();
		}

		void addSeparator()
		{
			++currentIDX;
			combo->insertSeparator(combo->count());
		}

		const bool setCurrentItem(const unsigned int idx, const bool updateUI = false)
		{			
			bool ret = false;
			auto it = mapping.find(idx);
			if (it != mapping.end()){
				currentItem_ = it->second.item;
				if (updateUI == true){
					combo->blockSignals(true);
					combo->setCurrentIndex(it->second.idx);
					combo->blockSignals(false);
				}

				ret = true;
			}else{
				currentItem_ = nullptr;
			}

			return ret;
		}

		T * currentItem()
		{
			return currentItem_;
		}

		const T * currentItem() const
		{
			return currentItem_;
		}

		const unsigned int count() const
		{
			return mapping.size();
		}

		void setVisible(const bool visible)
		{
			label->setVisible(visible);
			combo->setVisible(visible);
		}

		void setEnable(const bool enable)
		{
			combo->setEnabled(enable);
		}

	private:

		struct ItemDescription
		{
			unsigned int idx;
			T * item;
		};

	private:
		//! Aktualny item
		T * currentItem_;
		//! Mapowanie filtrów do indeksów
		std::map<unsigned int, ItemDescription> mapping;
		//! Aktualny indeks filtra w dropbox
		unsigned int currentIDX;
		//! Obs³ugiwane combo
		QComboBox * combo;
		//! Opis
		QLabel * label;
	};

private:
	//! Ui
	Ui::DataViewConfigurationWidget * ui;
	//! Obs³uga filtrów
	utils::shared_ptr < ItemHelper<hmdbCommunication::ShallowCopyFilter>> filters;
	//! Obs³uga perspektyw
	utils::shared_ptr < ItemHelper<hmdbCommunication::IDataSourcePerspective>> perspectives;
	//! Obs³uga contentów
	utils::shared_ptr < ItemHelper<hmdbCommunication::IDataSourceContent>> contents;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__DATAVIEWCONFIGURATIONWIDGET_H__