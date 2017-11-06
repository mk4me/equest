/********************************************************************
	created:  2014/12/27	16:26:17
	filename: CoreVectorInputDelegate.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_COREUI__COREVECTORINPUTDELEGATE_H__
#define __HEADER_GUARD_COREUI__COREVECTORINPUTDELEGATE_H__

#include <coreui/Export.h>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QPushButton>
#include <type_traits>
#include <array>
#include <utils/StringConversionTools.h>
#include <boost/bimap.hpp>
#include <coreui/CoreLabeledWidget.h>
#include <QtWidgets/QFileDialog>
#include <utils/StringConversionTools.h>
#include <numeric>

namespace coreUI
{
	//! Klasa pomocnicza przy obsłudze wprowadzania wartości liczbowych
	//! \tparam ValueType Typ liczb jakie chcemy wczytywać
	//! \tparam EditorType Typ widgeta obsługującego edycję liczb danego typu
	template<typename ValueType, class EditorType>
	class SingleNumbersEditorWidgetHelper
	{
		static_assert(std::is_arithmetic<ValueType>::value, "ValueType must be arithmetic type");		
		static_assert(std::is_base_of<QWidget, EditorType>::value, "EditorType must be derived from QWidget");

	public:

		//! \param parent Rodzic edytora
		//! \return Widget edytora
		static EditorType * createEditor(QWidget * parent = nullptr)
		{
			auto ret = new EditorType(parent);
			ret->setDecimals(std::min(5, std::numeric_limits<ValueType>::max_exponent10 + std::numeric_limits<ValueType>::digits10));
			ret->setRange(std::numeric_limits<ValueType>::lowest(), std::numeric_limits<ValueType>::max());			
			ret->setValue(std::max(std::numeric_limits<ValueType>::lowest(), ValueType()));
			ret->setSingleStep(std::numeric_limits<ValueType>::epsilon());
			return ret;
		}
		
		//! \return Nazwa sygnału generowanego przez widget kiedy zmienia się jego wartość
		static QString changeSignal()
		{
			return "valueChanged(const QString&)";
		}

		//! \param editor Widget edytora z którego wyciągamy wartość
		//! \return Wartość z edytora
		static const ValueType value(QWidget * editor)
		{
			auto e = static_cast<EditorType*>(editor);
			auto val = e->value();
			return (ValueType)val;
		}

		//! \param editor Widget edytora
		//! \param value Wartość ustawiana w edytorze
		static void setValue(QWidget * editor, const ValueType value)
		{
			auto e = static_cast<EditorType*>(editor);
			e->setValue(value);			
		}

		//! \param editor Widget edytora
		//! \param value Wartość ustawiana w edytorze
		static void setDecimals(QWidget * editor, const int value)
		{
			auto e = static_cast<EditorType*>(editor);
			e->setDecimals(std::min(value, std::numeric_limits<ValueType>::max_exponent10 + std::numeric_limits<ValueType>::digits10));
		}

		//! \param editor Widget edytora
		//! \param step Krok o jaki zmienia się wartość w edytorze
		static void setStep(QWidget * editor, const ValueType step)
		{
			auto e = static_cast<EditorType*>(editor);
			e->setSingleStep(step);
		}
		//! \param editor Widget edytora
		//! \param minVal Wartość minimalna w edytorze
		//! \param maxVal Wartość maksymalna w edytorze
		static void setLimits(QWidget * editor, const ValueType minVal,
			const ValueType maxVal)
		{
			auto e = static_cast<EditorType*>(editor);
			e->setRange(minVal, maxVal);			
		}
	};

	//! Klasa pomocnicza przy edycji warunków - prawda fałsz
	class COREUI_EXPORT ConditionEditorWidgetHelper
	{
	public:

		//! \param parent Rodzic edytora
		//! \return Widget obsługujący warunki
		static QCheckBox * createEditor(QWidget * parent = nullptr)
		{			
			return new QCheckBox(parent);
		}

		//! \return Nazwa sygnału na zmianę stanu warunku
		static QString changeSignal()
		{
			return "stateChanged(int)";
		}

		//! \param editor Widget edytora z którego wyciągamy wartość
		//! \return Wartość z edytora
		static const bool value(QWidget * editor)
		{
			auto e = static_cast<QCheckBox*>(editor);
			return (e->checkState() == Qt::Checked);
		}

		//! \param editor Widget edytora
		//! \param value Wartość ustawiana w edytorze
		static void setValue(QWidget * editor, const bool value)
		{
			auto e = static_cast<QCheckBox*>(editor);
			e->setChecked(value);
		}
	};

	//! Klasa pomocnicza przy edycji tekstów
	//! \tparam ValueType Typ tekstu (wg traitu utils::is_text)
	//! \tparam EditorType Typ widgeta obsługującego edycję tekstu
	template<typename ValueType, class EditorType = QLineEdit>
	class TextEditorWidgetHelper
	{
	public:

		//! \param parent Rodzic edytora
		//! \return Widget edytora
		static EditorType * createEditor(QWidget * parent = nullptr)
		{
			return new EditorType(parent);
		}

		//! \return Nazwa sygnały na zmianę stanu warunku
		static QString changeSignal()
		{
			return "textChanged(const QString&)";
		}

		//! \param editor Widget edytora z którego wyciągamy wartość
		//! \return Wartość z edytora
		static const ValueType value(QWidget * editor)
		{
			auto e = static_cast<EditorType*>(editor);
			return utils::Convert::toString(e->text());
		}

		//! \param editor Widget edytora
		//! \param value Wartość ustawiana w edytorze
		static void setValue(QWidget * editor, const ValueType & value)
		{
			auto e = static_cast<EditorType*>(editor);
			e->setText(utils::Convert::toString(value));
		}
	};

	//! Klasa pomocnicza przy edycji tekstów - specjalizacja dla edytora obsługującego style i HTML
	//! \tparam ValueType Typ tekstu (wg traitu utils::is_text)	
	template<typename ValueType>
	class TextEditorWidgetHelper<ValueType, QTextEdit>
	{
	public:

		//! \param parent Rodzic edytora
		//! \return Widget edytora
		static QTextEdit * createEditor(QWidget * parent = nullptr)
		{
			return new QTextEdit(parent);
		}

		//! \return Nazwa sygnały na zmianę stanu warunku
		static QString changeSignal()
		{
			return "textChanged()";
		}

		//! \param editor Widget edytora z którego wyciągamy wartość
		//! \return Wartość z edytora
		static const ValueType value(QWidget * editor)
		{
			QTextEdit* e = static_cast<QTextEdit*>(editor);
			return utils::Convert::toString(e->toPlainText());
		}

		//! \param editor Widget edytora
		//! \param value Wartość ustawiana w edytorze
		static void setValue(QWidget * editor, const ValueType & value)
		{
			auto e = static_cast<QTextEdit*>(editor);
			e->setPlainText(utils::Convert::toString(value));
		}
	};

	//! Klasa pomocnicza dostarczająca domyslnych polityk edycji dla zadanych typów prostych
	//! \tparam ValueType Typ wartości prostej tworzącej wektor
	template<typename ValueType>
	struct DefaultEditorHelper
	{
		static_assert(std::is_integral<ValueType>::value || std::is_floating_point<ValueType>::value || utils::is_text<ValueType>::value, "ValueType must be either floating point, integral or text");
		
		static_assert(std::is_same<typename std::conditional<std::is_integral<ValueType>::value,
			typename std::conditional<(sizeof(ValueType) < sizeof(int)) || ((sizeof(ValueType) == sizeof(int)) && std::is_unsigned<ValueType>::value), std::true_type, std::false_type>::type,
			std::true_type>::type, std::true_type>::value, "Integral type must be at most int range");

		typedef SingleNumbersEditorWidgetHelper<ValueType, typename std::conditional<std::is_integral<ValueType>::value, QSpinBox, QDoubleSpinBox>::type> type;
	};

	//! Klasa pomocnicza dostarczająca domyslnych polityk edycji dla zadanych typów prostych
	//! Specjalizacja dla bool
	template<>
	struct DefaultEditorHelper<bool>
	{
		typedef ConditionEditorWidgetHelper type;
	};

	//! Klasa pomocnicza dostarczająca domyslnych polityk edycji dla zadanych typów prostych
	//! Specjalizacja dla QString
	template<>
	struct DefaultEditorHelper < QString >
	{
		typedef TextEditorWidgetHelper<QString, QLineEdit> type;
	};

	//! Klasa pomocnicza dostarczająca domyslnych polityk edycji dla zadanych typów prostych
	//! Specjalizacja dla std::string
	template<>
	struct DefaultEditorHelper < std::string >
	{
		typedef TextEditorWidgetHelper<std::string, QLineEdit> type;
	};

	//! Klasa bazowa delegata obsługującego edycje wektorów
	class COREUI_EXPORT CoreVectorInputDelegate : public QStyledItemDelegate
	{
		Q_OBJECT

	protected:
		//! Konstruktor chroniony
		//! \param parent Obiekt rodzic naszego delegata
		CoreVectorInputDelegate(QObject * parent = nullptr);

	public:
		//! Destruktor wirtualny
		virtual ~CoreVectorInputDelegate();

	protected slots:
		//! Akcja na zakończenie edycji w edytorze wektora
		void commitAndCloseEditor();
	};

	//! Klasa bazowa widgeta edytującego wektor
	class COREUI_EXPORT CoreVectorEditor : public QWidget
	{
		Q_OBJECT

	protected:
		//! Konstruktor chorniony
		//! \param parent Widget rodzic naszego edytora
		CoreVectorEditor(QWidget * parent = nullptr);

	public:

		//! Destruktor wirtualny
		virtual ~CoreVectorEditor();

		static QString vectorPattern(const unsigned int size, const QChar valueSeparator = ',');
		static QString quaternionPattern(const QChar valueSeparator = ',');

		template<typename T>
		static QString quaternionToString(const T x, const T y, const T z, const T w,
			const QString & quatPattern = quaternionPattern())
		{
			return QString(quatPattern).arg(utils::Convert::toQString<T>(x))
				.arg(utils::Convert::toQString<T>(y))
				.arg(utils::Convert::toQString<T>(z))
				.arg(utils::Convert::toQString<T>(w));
		}

		template<typename VectorType>
		static QString quaternionToString(const VectorType & vector,
			const QString & quatPattern = quaternionPattern())
		{
			QString ret(quatPattern);

			for (const auto & val : vector)
			{
				ret = ret.arg(utils::Convert::toQString(val));
			}

			return ret;
		}

		/*template<typename T>
		static QString quaternionToString(const T(&array)[4],
		const QString & quatPattern = quaternionPattern())
		{
		QString ret(quatPattern);

		for (std::size_t i = 0; i < 4; ++i)
		{
		ret = ret.arg(utils::Convert::toQString<T>(array[i]));
		}

		return ret;
		}*/

		template<typename T>
		static QString quaternionToString(const T * array,
			const QString & quatPattern = quaternionPattern())
		{
			QString ret(quatPattern);

			for (std::size_t i = 0; i < 4; ++i)
			{
				ret = ret.arg(utils::Convert::toQString<T>(array[i]));
			}

			return ret;
		}

		template<typename VectorType>
		static QString vectorToString(const VectorType & vector, const QString & vecPattern)
		{
			QString ret(vecPattern);

			for (const auto & val : vector)
			{
				ret = ret.arg(utils::Convert::toQString(val));
			}

			return ret;
		}

		template<typename VectorType>
				static QString vectorToString(const VectorType & vector)
		{
			return vectorToString(vector, vectorPattern(vector.size()));
		}

		template<typename T, std::size_t N>
		static QString vectorToString(const T(&array)[N],
			const QString & vecPattern = vectorPattern(N))
		{
			QString ret(vecPattern);			

			for (std::size_t i = 0; i < N; ++i)
			{
				ret = ret.arg(utils::Convert::toQString(array[i]));
			}

			return ret;
		}

		template<typename VectorType>
		static QString vectorToString(const VectorType & vector,
			const std::size_t N, const QString & vecPattern)
		{
			QString ret(vecPattern);

			for (std::size_t i = 0; i < N; ++i)
			{
				ret = ret.arg(utils::Convert::toQString(vector[i]));
			}

			return ret;
		}

		template<typename T>
		static QString vectorToString(const T * array,
			const std::size_t N, const QString & vecPattern )
		{
			QString ret(vecPattern);

			for (std::size_t i = 0; i < N; ++i)
			{
				ret = ret.arg(utils::Convert::toQString(*array[i]));
			}

			return ret;
		}

		template<typename T>
				static QString vectorToString(const T * array, const std::size_t N)
		{
			return vectorToString(array, N, vectorPattern(N));
		}

	signals:

		//! \param idx Indeks wartości wektora która uległa zmianie
		void valueChanged(const unsigned int idx);
		//! Zakończenie edycji w wektorze
		void editingFinished();

	protected slots:

		//! Akcja na zmianę wartości w wektorze
		void onValueChanged();

	private:

		//! Metoda wywoływana przed emisją sygnału valueChanged
		//! \param idx Indeks wartosci wektora która uległa zmianie
		virtual void customActionsOnValueChange(const unsigned int idx) {}

		//! \param w Widget edytora
		//! \return Indeks wartości obsługiwanej przez ten edytor
		virtual unsigned int indexOfValueEditor(QWidget * w) const = 0;
	};

	//! Klasa realizująca widget edytora dla konkretnego wektora
	//! \tparam ValueType Typ wartości wektora
	//! \tparam SizeType Rozmiar wektora
	//! \tparam	ValueEditorHelper Polityka pomocna przy zarządzaniu edytorem poszczególnych wartości
	template<typename ValueType, unsigned int SizeType,
	typename ValueEditorHelper = typename DefaultEditorHelper<ValueType>::type>
	class CoreVectorEditorImpl : public CoreVectorEditor
	{
		static_assert(SizeType > 1, "Vector size must be greater or equal 2");

	public:

		//! Typ wartości wektora
		typedef std::array<ValueType, SizeType> VectorType;

	public:

		//! \param parent Widget rodzic naszego edytora wektorów
		CoreVectorEditorImpl(QWidget * parent = nullptr) : CoreVectorEditor(parent),
			nameLabel(nullptr), vectorArea(nullptr), finalizeButton(nullptr)
		{
			auto layout = new QHBoxLayout;
			nameLabel = new QLabel(QObject::tr("Vector[%1]").arg(SizeType), this);
			layout->addWidget(nameLabel);
			auto w = new QScrollArea(this);
			layout->addWidget(w);

			vectorArea = new QWidget;
			auto l = new QHBoxLayout;
			vectorArea->setLayout(l);

			for (unsigned int i = 0; i < SizeType; ++i)
			{
				auto lw = new CoreLabeledWidget(this);
				auto editor = ValueEditorHelper::createEditor(lw);
				bool ret = connect(editor, qFlagLocation(QString("2" + ValueEditorHelper::changeSignal() + QLOCATION).toStdString().c_str()), this, SLOT(onValueChanged()));				
				lw->label()->setText(QString("x%1:").arg(i));
				lw->setLabeledWidget(editor);
				l->addWidget(lw);
				widgetsMapping.insert(boost::bimap<int, CoreLabeledWidget*>::value_type(i, lw));

				if (i != (SizeType - 1))
				{
					l->addSpacing(5);
				}
			}

			finalizeButton = new QPushButton(QObject::tr("Finish"), vectorArea);
			finalizeButton->setEnabled(false);
			l->addWidget(finalizeButton);
			connect(finalizeButton, SIGNAL(clicked(bool)), this, SIGNAL(editingFinished()));

			w->setWidget(vectorArea);
			setLayout(layout);
		}

		//! Destruktor wirtualny
		virtual ~CoreVectorEditorImpl()
		{

		}

		//! \return Nazwa wartosci reprezentowanej przez wektor
		QString name() const
		{
			return nameLabel->text();
		}

		//! \param name Nazwa wartosci reprezentowanej przez wektor
		void setName(const QString & name)
		{
			nameLabel->setText(name);
		}

		//! \param visible Czy nazwa wektora ma być widoczna
		void setNameVisible(const bool visible)
		{
			nameLabel->setVisible(visible);
		}

		//! \param idx Indeks wartości w wektorze
		//! \return Nazwa wartosci w wektorze pod zadanym indeksem
		QString name(const unsigned int idx) const
		{
			if (idx >= SizeType){
				throw std::runtime_error("Index out of the bounds");
			}

			auto it = widgetsMapping.left.find(idx);

			return it->get_right()->label()->text();
		}

		//! \param idx Indeks wartości w wektorze
		//! \param name Nazwa wartosci w wektorze pod zadanym indeksem
		void setName(const unsigned int idx, const QString & name)
		{
			if (idx >= SizeType){
				throw std::runtime_error("Index out of the bounds");
			}

			widgetsMapping.left[idx]->label()->setText(name);
		}

		//! \param idx Indeks wartości w wektorze
		//! \param visible Czy nazwa elementu wektora ma być widoczna
		void setNameVisible(const unsigned int idx, const bool visible)
		{
			if (idx >= SizeType){
				throw std::runtime_error("Index out of the bounds");
			}

			widgetsMapping.left[idx]->label()->setVisible(visible);			
		}

		//! \return Wartość całego wektora
		VectorType value() const
		{
			VectorType ret;

			for (const auto & val : widgetsMapping)
			{
				ret[val.get_left()] = ValueEditorHelper::value(val.get_right()->labeledWidget());
			}

			return ret;
		}

		//! \param value Wartość całego wektora
		void setValue(const VectorType & value)
		{
			for (unsigned int i = 0; i < SizeType; ++i)
			{
				ValueEditorHelper::setValue(widgetsMapping.left.find(i)->get_right()->labeledWidget(), value[i]);
			}
		}

		//! \param idx Indeks wartości w wektorze
		//! \return Wartość elementu wektora o zadanym indeksie
		ValueType value(const unsigned int idx) const
		{
			if (idx >= SizeType){
				throw std::runtime_error("Index out of the bounds");
			}

			auto it = widgetsMapping.left.find(idx);
			return ValueEditorHelper::value(it->get_right()->labeledWidget());
		}

		//! \param idx Indeks wartości w wektorze
		//! \param value Wartość elementu wektora o zadanym indeksie
		void setValue(const unsigned int idx, const ValueType & value)
		{
			if (idx >= SizeType){
				throw std::runtime_error("Index out of the bounds");
			}

			auto it = widgetsMapping.left.find(idx);
			ValueEditorHelper::setValue(it->get_right()->labeledWidget(), value);
		}

		//! \param idx Indeks wartości w wektorze
		//! \param minVal Wartość minimalna elementu wektora o zadanym indeksie
		//! \param maxVal Wartość maksymalna elementu wektora o zadanym indeksie
		void setDecimals(const unsigned int idx, const int value)
		{
			if (idx >= SizeType){
				throw std::runtime_error("Index out of the bounds");
			}

			auto it = widgetsMapping.left.find(idx);
			ValueEditorHelper::setDecimals(it->get_right()->labeledWidget(), value);
		}

		//! \param editor Widget edytora
		//! \param value Wartość ustawiana w edytorze
		void setDecimals(const int value)
		{
			for (const auto & val : widgetsMapping)
			{
				ValueEditorHelper::setDecimals(val.get_right()->labeledWidget(), value);
			}
		}

		//! \param idx Indeks wartości w wektorze
		//! \param minVal Wartość minimalna elementu wektora o zadanym indeksie
		//! \param maxVal Wartość maksymalna elementu wektora o zadanym indeksie
		void setLimits(const unsigned int idx, const ValueType & minVal,
			const ValueType & maxVal)
		{
			if (idx >= SizeType){
				throw std::runtime_error("Index out of the bounds");
			}

			auto it = widgetsMapping.left.find(idx);
			ValueEditorHelper::setLimits(it->get_right()->labeledWidget(), minVal, maxVal);
		}

		//! \param idx Indeks wartości w wektorze
		//! \param step Krok o jaki zmienia się wartość wektora o zadanym indeksie		
		void setStep(const unsigned int idx, const ValueType & step)
		{
			if (idx >= SizeType){
				throw std::runtime_error("Index out of the bounds");
			}

			auto it = widgetsMapping.left.find(idx);
			ValueEditorHelper::setStep(it->get_right()->labeledWidget(), step);
		}

		//! \param minVal Wartość minimalna elementów wektora
		//! \param maxVal Wartość maksymalna elementów wektora
		void setLimits(const ValueType & minVal,
			const ValueType & maxVal)
		{
			for (const auto & val : widgetsMapping)
			{
				ValueEditorHelper::setLimits(val.get_right()->labeledWidget(), minVal, maxVal);
			}			
		}

		//! \param step Krok o jaki zmieniają się wartości wektora
		void setStep(const ValueType & step)
		{
			for (const auto & val : widgetsMapping)
			{
				ValueEditorHelper::setStep(val.get_right()->labeledWidget(), step);
			}
		}

	private:

		virtual void customActionsOnValueChange(const unsigned int idx) override
		{
			finalizeButton->setEnabled(true);
		}

		virtual unsigned int indexOfValueEditor(QWidget * w) const override
		{
			CoreLabeledWidget * lw = nullptr;

			auto it = std::find_if(widgetsMapping.begin(), widgetsMapping.end(),
				[w](const boost::bimap<int, CoreLabeledWidget*>::value_type & val)
			{
				return val.get_right()->labeledWidget() == w;
			});

			return it->get_left();
		}

	private:
		//! Widget z nazwa wektora
		QLabel * nameLabel;
		//! Widget z elementami wektora
		QWidget * vectorArea;
		//! Przycisk kończący edycję
		QPushButton * finalizeButton;
		//! Mapowanie elementów wektora do widgetów obsługujących ich edytory lokalne
		boost::bimap<int, CoreLabeledWidget*> widgetsMapping;
	};

	//! Implementacja delegata obługującego konkretny typ wektora
	//! \tparam ValueType Typ wartości elementów wektora
	//! \tparam SizeType Rozmiar wektora
	//! \tparam ValueEditorWidgetHelper Klasa pomocnicza przy tworzeniu edytorów elementów wektora
	//! \tparam ValueToString Obiekt konwertujący wartości wektora do stringa (wg utils::is_text)
	template<typename ValueType, unsigned int SizeType,
		typename ValueEditorWidgetHelper = typename DefaultEditorHelper<ValueType>::type>
	class CoreVectorInputDelegateImpl : public CoreVectorInputDelegate
	{
	public:

		//! Typ edytora wektora
		typedef CoreVectorEditorImpl<ValueType, SizeType, ValueEditorWidgetHelper> VectorEditorWidget;

	public:

		//! \param displayPattern Wzór wektora wyświetlany po zakończeniu edycji, zasilany wartościami wektora
		CoreVectorInputDelegateImpl(const QString & displayPattern,			
			QObject * parent = nullptr) : CoreVectorInputDelegate(parent),
			displayPattern_(displayPattern)
		{

		}

		//! Destruktor wirtualny
		virtual ~CoreVectorInputDelegateImpl()
		{

		}

		//! \return Wzór wektora wyświetlany po zakończeniu edycji
		const QString & displayPattern() const
		{
			return displayPattern_;
		}

		//! \param displayPattern Wzór wektora wyświetlany po zakończeniu edycji
		//! \param value Wektor wartosci
		//! \return Sformatowany wektor gotowy do wyświetlenia po edycji
		static QString valueToDisplay(const QString & displayPattern, const typename VectorEditorWidget::VectorType & value)
		{
			return CoreVectorEditor::vectorToString(value, displayPattern);
		}

		virtual QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option,
			const QModelIndex &index) const override
		{
			/*
			VectorEditorWidget * editor = new VectorEditorWidget(parent);
			connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));			
			return editor;
			*/
			return new QFileDialog(parent);
		}

		virtual QSize sizeHint(const QStyleOptionViewItem &option,
			const QModelIndex &index) const override
		{
			QSize ret = QStyledItemDelegate::sizeHint(option, index);
			return VectorEditorWidget().sizeHint();
		}

		virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override
		{
			if (index.data(Qt::UserRole).canConvert<VectorEditorWidget::VectorType>()) {
				//VectorEditorWidget::VectorType value = qvariant_cast<VectorEditorWidget::VectorType>(index.data(Qt::UserRole));
				//VectorEditorWidget * e = static_cast<VectorEditorWidget*>(editor);
				//e->setValue(value);				
			}
			else {
				QStyledItemDelegate::setEditorData(editor, index);
			}
		}

		virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
			const QModelIndex &index) const override
		{
			if (index.data(Qt::UserRole).canConvert<VectorEditorWidget::VectorType>()) {
				/*
				VectorEditorWidget * e = static_cast<VectorEditorWidget*>(editor);
				auto val = e->value();
				model->setData(index, QVariant::fromValue(val), Qt::UserRole);
				model->setData(index, valueToDisplay(displayPattern_, val));
				*/
			}
			else {
				QStyledItemDelegate::setModelData(editor, model, index);
			}
		}

	private:
		//! Wzór wektora wyświetlany po zakończeniu edycji
		const QString displayPattern_;
	};
}

#endif	// __HEADER_GUARD_COREUI__COREVECTORINPUTDELEGATE_H__
