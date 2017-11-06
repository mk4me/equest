/********************************************************************
    created:  2012/03/14
    created:  14:3:2012   8:04
    filename: AntropometricDataWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__ANTROPOMETRICDATAWIDGET_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__ANTROPOMETRICDATAWIDGET_H__

#include <qthtmlgenlib/TableGenerator.h>
#include <QtWidgets/QTextEdit>

//! Widget realizujący prezentację danych antropometrycznych
//! Robi to w oparciu o generowanie/formatowanie/stylowanie tabel HTML za pomocą CSS
class AntropometricDataWidget : public QTextEdit
{
	Q_OBJECT

public:
	//! Konstruktor
	AntropometricDataWidget(QWidget * parent = nullptr);
	//! Destruktor
	virtual ~AntropometricDataWidget();

public slots:
	//! \param autUpdate Czy przy zmianie dowolnego parametru danych antropometrycznych widget ma się odświeżyć - wygenerować tabele HTML, ....
	void setAutoUpdate(bool autoUpdate = true);

	//! Funkcje seterów wartości
	//! \param val Masa
	void setBodyMass(const QString & val);
	//! \param val Wzrost
	void setBodyHeight(const QString & val);
	//! \param val TODO
	void setASIS(const QString & val);
	//! \param val Długość lewej nogi
	void setLeftLegLength(const QString & val);
	//! \param val Długość prawej nogi
	void setRightLegLenght(const QString & val);
	//! \param val Szerokośc lewego kolana
	void setLeftKneeWidth(const QString & val);
	//! \param val Szerokośc prawego kolana
	void setRightKneeWidth(const QString & val);
	//! \param val Szerokośc lewej kostki
	void setLeftAnkleWidth(const QString & val);
	//! \param val Szerokośc prawej kostki
	void setRightAnkleWidth(const QString & val);
	//! \param val Szerokośc TODO
	void setLeftCircuitThigh(const QString & val);
	//! \param val Szerokośc TODO
	void setRightCircuitThight(const QString & val);
	//! \param val TODO
	void setLeftCircuitShank(const QString & val);
	//! \param val TODO
	void setRightCircuitShank(const QString & val);
	//! \param val Przesunięcie lewego ramienia
	void setLeftShoulderOffset(const QString & val);
	//! \param val Przesunięcie prawego ramienia
	void setRightShoulderOffset(const QString & val);
	//! \param val Szerokość lewego łokcia
	void setLeftElbowWidth(const QString & val);
	//! \param val Szerokość prawego łokcia
	void setRightElbowWidth(const QString & val);
	//! \param val Szerokość lewego nadgarstka
	void setLeftWristWidth(const QString & val);
	//! \param val Szerokość prawego nadgarstka
	void setRightWristWidth(const QString & val);
	//! \param val Grubość lewego nadgarstka
	void setLeftWristThickness(const QString & val);
	//! \param val Grubość prawego nadgarstka
	void setRightWristThickness(const QString & val);
	//! \param val Szerokość lewej dłoni
	void setLeftHandWidth(const QString & val);
	//! \param val Szerokość prawej dłoni
	void setRightHandWidth(const QString & val);
	//! \param val Grubość lewej dłoni
	void setLeftHandThickness(const QString & val);
	//! \param val Grubość prawej dłoni
	void setRightHandThickness(const QString & val);

	//! \param val Wartość która zastąpi wszystkie wpisy w tabelach
	void resetAntropometricData(const QString & val = QString("-"));

private:
	//! Inicjuje tabele HTML
	void initAntropometric();
	//! Inicjuje nagłówki tabel
	void initAntropometricHeadersStructure();
	//! Inicjuje zawartośc tabel
	void initAntropometricContent();
	//! Inicjuje style tabel
	void initAntropometricStyles();
	//! Inicjucje atrybuty tabel
	void initAntropometricAtributes();

	//! Odświeża zawartośc tabel HTML - generuje/formatuje i styluje od zera
	void updateContent();
	//! Próbuje odświeżać zawartość tabel jeśli jest to konieczne i możliwe
	void tryUpdateContent();
	// Próbuje zmienić zawartośc tabeli i odświeżyć ją
	void tryChangeContent();

private:
	//! Struktura do opisu tabeli z danymi
	struct AntropometricTabel {
		//! Zawartość tabeli
		htmlGen::HtmlDataTableContent antropometricContent;
		//! Ukłąd nagłóków tabeli
		htmlGen::TableHeadersStructureDescriptor antropometricHeadersStructure;
		//! Atrybuty poszczególnych komórek tabeli
		htmlGen::HtmlDataTableCellAttributes antropometricCellsAttributes;
		//! Style tabeli
		htmlGen::HtmlDataTableStyles antropometricStyles;
	};

private:

	//! Czy należy przy każdej zmianie aktualizować
	bool autoUpdate_;
	//! Czy nastąpiła zmiana zawartości
	bool changed_;
	//! Opis danych antropometrycznych ogólnych
	AntropometricTabel antropometricGeneralTable;
	//! Opis danych antropometrycznych z podziałem na lewą i prawą stronę
	AntropometricTabel antropometricLeftRightTable;
};

#endif // HEADER_GUARD_HMDBCOMMUNICATION__ANTROPOMETRICDATAWIDGET_H__
