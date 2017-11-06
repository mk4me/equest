/********************************************************************
    created:  2012/03/01
    created:  1:3:2012   9:25
    filename: TableGenerator.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___TABLEGENERATOR_H__
#define HEADER_GUARD___TABLEGENERATOR_H__

#include <vector>
#include <map>
#include <QtCore/QString>
#include <QtGui/QTextTable>
#include <boost/any.hpp>
#include <utils/Debug.h>
#include <array>

namespace htmlGen {

//! Wzorzec klasy z danymi do wyświetlania - najczęściej są to ciągi znaków: stD::String lub QString
template<class T>
class DataTable
{

public:
    //! Typ wpisu wiersza
    typedef std::vector<T> Row;
    //! Typ wpisu kolumny
    typedef Row Column;
    //! Zawartość tabeli
    typedef std::vector<Row> TableContent;

public:
	//! Konstruktor domyślny
    DataTable() : rows_(0), columns_(0)
    {

    }

	//! Konstruktor
	//! \param rows Ilośc wierszy tabeli danych
	//! \param column Ilość kolumn tabeli danych
    DataTable(int rows, int columns) : rows_(rows), columns_(columns)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        UTILS_ASSERT(columns > 0, "Wrong columns count");
        rebuildContent();
    }

	//! Konstruktor kopiujący
	//! \param htmlTableContent Tabela do skopiowania
    DataTable(const DataTable & htmlTableContent) : rows_(htmlTableContent.rows_), columns_(htmlTableContent.columns_), content_(htmlTableContent.content_)
    {

    }

	//! Destruktor
    ~DataTable() {}

	//! \param rows Ilość wierszy tabeli
    void setRows(int rows)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        rows_ = rows;
        rebuildContent();
    }

	//! \param columns Ilość kolumn tabeli
    void setColumns(int columns)
    {
        UTILS_ASSERT(columns > 0, "Wrong columns count");
        columns_ = columns;
        for(int i = 0; i < rows_; ++i){
            content_[i].resize(columns_);
        }
    }

	//! \param rows Ilość wierszy tabeli
	//! \param columns Ilość kolumn tabeli
    void setDimensions(int rows, int columns)
    {
        UTILS_ASSERT(rows > 0, "Wrong rows count");
        UTILS_ASSERT(columns > 0, "Wrong columns count");

        rows_ = rows;
        columns_ = columns;

        rebuildContent();
    }

	//! \return Ilość wierszy tabeli
    int rows() const
    {
        return rows_;
    }
	//! \return Ilość kolumn tabeli
    int columns() const
    {
        return columns_;
    }

	//! \param row Wiersz który ustawiamy
	//! \param rowContent Zawartość wiersza którą ustawiamy
    void setRow(int row, const Row & rowContent)
    {
        UTILS_DEBUG(row >= 0 && row < rows_, "Row out of range");
        int columnsToCopy = std::max(columns_, rowContent.size());
        std::copy(rowContent.begin(), rowContent.begin() + columnsToCopy, content_[row].begin());
    }
	//! \param column Kolumna który ustawiamy
	//! \param columnContent Zawartość kolumny którą ustawiamy
    void setColumn(int column, const Column & columnContent)
    {
        UTILS_DEBUG(column >= 0 && column < columns_, "Column out of range");
        int rowsToCopy = std::max(rows_, columnContent.size());
        for(int i = 0; i < rowsToCopy; ++i){
            content_[i][column] = columnContent[i];
        }
    }

	//! \param row Wiersz który czyścimy
	//! \param val Wartość którą ustawiamy w całym wierszu
    void clearRow(int row, const T & val = T())
    {
        UTILS_DEBUG(row >= 0 && row < rows_, "Row out of range");
        std::fill(content_[row].begin(), content_[row].end(), val);
    }
	//! \param column Kolumna którą czyścimy
	//! \param val Wartość którą ustawiamy w całej kolumnie
    void clearColumn(int column, const T & val = T())
    {
        UTILS_DEBUG(column >= 0 && column < columns_, "Column out of range");
        for(int i = 0; i < rows_; ++i){
            content_[i][column] = val;
        }
    }

	//! \param row Wiersz
	//! \param column Kolumna
	//! \return Wartość komórki o zadanych współrzędnych
    const T & cell(int row, int column) const
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        return content_[row][column];
    }
	//! \param row Wiersz
	//! \param column Kolumna
	//! \return Wartość komórki o zadanych współrzędnych
    T & cell(int row, int column)
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        return content_[row][column];
    }
	//! \param row Wiersz
	//! \param column Kolumna
	//! \param cellContent Zawartość komórki do ustawienia
    void setCell(int row, int column, const T & cellContent)
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        content_[row][column] = cellContent;
    }

	//! \param row Wiersz
	//! \param column Kolumna
	//! \param val Wartość którą czyścimy komórke
    void clearCell(int row, int column, const T & val = T())
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        content_[row][column] = val;
    }

	//! \param row Wiersz
	//! \return Zawartość wiersza
    const Row & row(int row) const
    {
        UTILS_ASSERT(row >= 0 && row < rows_, "Row out of range");
        return content_[row];
    }
	//! \param column Kolumna
	//! \return Zawartość kolumny
    const Column column(int column) const
    {
        UTILS_ASSERT(column >= 0 && column < columns_, "Column out of range");
        Column col;

        for(int i = 0; i < rows_; ++i){
            col.push_back(content_[i][column]);
        }

        return col;
    }
	//! \return Zawartość tabeli danych
    const TableContent & content() const
    {
        return content_;
    }

private:
	//! Fizycznie zmiania rozmiar tablicy
    void rebuildContent()
    {
        content_.resize(rows_);
        for(int i = 0; i < rows_; ++i){
            content_[i].resize(columns_);
        }
    }

private:
    //! Ilość wierszy
    int rows_;
    //! Ilość kolumn
    int columns_;
    //! Zawartość tabeli
    TableContent content_;
};

typedef DataTable<QString> HtmlDataTableContent;

//! Typ do rozszerzania komórek
typedef int CellSpan;

//! Typ wyrównania zawartości komórek w poziomie
enum HAlign {
    HALeft,
    HARight,
    HACenter,
    HAJustify
};

//! Typ wyrównania zawartości komórek w pionie
enum VAlign {
    VATop,
    VAMiddle,
    VABottom
};

//! Typ opisujący rodzaj zadawania wymiarów
enum HtmlWidthType {
    WRelative,
    WAbsolute
};

//! Typ opisujący wymiary
struct HtmlWidth
{
	//! Wymiar
    float width;
	//! Sposób zadawania wymiaru
    HtmlWidthType type;
};

//! Lista atrybutów komórki
enum HtmlCellAttribute {
    CellWidth,      //! Szerokość
    CellBgColor,    //! Kolor tła
    CellColSpan,    //! Na ile komórek w pionie rozszerzamy
    CellRowSpan,    //! Na ile komórek w poziomie rozszerzamy
    CellHAlign,     //! Wyrównanie tekstu komórki w poziomie
    CellVAlign,     //! Wyrównanie tekstu w pionie
};

//! Klasa do zarządzania atrybutami komórek tabeli HTML
class HtmlCellAttributes
{
public:
	//! Konstruktor domyślny
    HtmlCellAttributes();
	//! Destruktor
    ~HtmlCellAttributes();

public:
	//! \param html [out]Dokument do którego generujemy atrybut w postaci tagów HTML
    void generateHtmlAttributes(QString & html) const;
	//! \param width Rozmiar elementu
	//! \param typ Sposób zadawania rozmiaru
    void setWidth(float width, HtmlWidthType type);
	//! Czyści atrybut wymiaru
    void clearWidth();
	//! \param bgColor Kolor tła komórki
    void setBgColor(const QString & bgColor);
	//! Czyści atrybut koloru tła
    void clearBgColor();
	//! \param colSpan Ilość kolumn na które komórka będzie rozszerzana
    void setColSpan(CellSpan colSpan);
	//! Czyści atrybut ilości kolumn na które komórka będzie rozszerzana
    void clearColSpan();
	//! \param rowSpan Ilość wierszy na które komórka będzie rozszerzana
    void setRowSpan(CellSpan rowSpan);
	//! Czyści atrybut ilości wierszy na które komórka będzie rozszerzana
    void clearRowSpan();
	//! \param hAlign Wyrównanie zawartości komórki w poziomie
    void setHAlign(HAlign hAlign);
	//! Czyści atrybut wyrównania zawartości komórki w poziomie
    void clearHAlign();
	//! \param vAlign Wyrównanie zawartości komórki w pionie
    void setVAlign(VAlign vAlign);
	//! Czyści atrybut wyrównania zawartości komórki w pionie
    void clearVAlign();

private:
	//! Mapa atrubutów
    std::map<HtmlCellAttribute, boost::any> attributes;
};

//! Lista atrybutów tabeli
enum HtmlTableAttribute {
    TableWidth,			//! Szerokość
	TableHeight,		//! Wysokość
	TableBorder,		//! Grubość ramki
    TableBgColor,		//! Kolor tła
    TableCellSpacing,	//! Odstęp pomiędzy komórkami
    TableCellPadding    //! Padding komórek
};

//! Klasa obsługująca atrybuty tabeli
class HtmlTableAttributes
{
public:

    HtmlTableAttributes();
    ~HtmlTableAttributes();

public:
	//! \param html [out]Dokument do którego generujemy atrybut w postaci tagów HTML
    void generateHtmlAttributes(QString & html) const;
	//! \param width Szerokość tabeli
	//! \param typ Sposób zadawania rozmiaru
    void setWidth(float width, HtmlWidthType type);
	//! Czyści atrybut szerokości
    void clearWidth();
	//! \param height Wysokośc tabeli
	//! \param typ Sposób zadawania rozmiaru
	void setHeight(float height, HtmlWidthType type);
	//! Czyści atrybut wysokości
    void clearHeight();
	//! \param bgColor Kolor tła tabeli
    void setBgColor(const QString & bgColor);
	//! Czyści atrybut koloru tła
    void clearBgColor();
	//! \param border Szerokość ramki tabeli i komóek
	void setBorder(int border);
	//! Czyści atrybut szerokości ramki tabeli i komóek
    void clearBorder();
	//! \param spacing Dodatkowa ilość pixeli wokół właściwej zawartości komórki
    void setCellSpacing(int spacing);
	//! Czyści atrybut dodatkowej ilości pixeli wokół właściwej zawartości komórki
    void clearCellSpacing();
	//! \param spacing Odstęp pomiędzy komórkami w tabeli
    void setCellPadding(int padding);
	//! Czyści atrybut odstępu pomiędzy komórkami w tabeli
    void clearCellPadding();

private:
	//! Mapa atrybutów tabeli
    std::map<HtmlTableAttribute, boost::any> attributes;
};

//! Typ tabeli opisującej atrybuty komórek tabeli HTML
typedef DataTable<HtmlCellAttributes> HtmlDataTableCellAttributes;

//! Typ elementu tabeli - wiersza, kolumny, komórki
enum StyleStatus {
    Single  = 0, //! Pojedynczy element
    First   = 1, //! Pierwszy element
    Middle  = 2, //! Środkowy element
    Last    = 3  //! Ostatni element
};

//! \param value wartość dla której wyznaczmy typ
//! \param min Minimalna wartość zakresu
//! \param max Maksymalna wartośc zakresu
//! \return Typ elementu: Single -> min = max = value, First -> value = min, min != max, Middle -> value != min, value != max, min != max, Last -> value = max, min != max
StyleStatus styleStatus(int value, int min, int max);

//! Typ opisujący style wierszy dla ich statusów
typedef QString HtmlRowStyles[4];
//! Typ opisujący style kolumn dla ich statusów
typedef QString HtmlColumnStyles[4];

//! Typ opisujący style komórek zależnie od ich statusów
typedef QString HtmlStyles[4][4];

//! Agregat styli komórek
struct HtmlDataTableStyles
{
    //! Styl tabeli
    QString tableStyle_;

	//! Atrybuty tabeli
	HtmlTableAttributes tableAttributes;

    //! Styl wierszy gdy poziome nałówki
    HtmlRowStyles headerRowStyle_;

    //! Style wierszy danych
    HtmlRowStyles contentRowStyle_;

    //! Macierz styli komórek nagłówków
    HtmlStyles headersStyles;

    //! Macierz styli komórek danych
    HtmlStyles contentStyles;
};

//Opisuje strukturę nagłówków tabeli
struct TableHeadersStructureDescriptor {
    //! Ilość wierszy nagłówkowych
    int headerRows;
    //! Ilosć kolumn nagłówkowych
    int headerColumns;
};

//! Klasa generująca tabele HTML z danymi
class HtmlDataTableGenerator
{
public:
    //! \param content Zawartość tabeli wiersz-kolumna
    //! \param structure Struktura tabeli
    //! \param styles Style poszczególnycvh komórek
    //! \param rows Całkowita ilość wierszy tabeli (utniemy lub dodamy jeśli będzie trzeba
    //! \param columns Całkowita ilość kolumn tabeli (utniemy lub dodamy jeśli będzie trzeba
    static void generateHtmlTable(QString & table, const HtmlDataTableContent & content, TableHeadersStructureDescriptor structure,
        const HtmlDataTableCellAttributes & cellAttributes = HtmlDataTableCellAttributes(), const HtmlDataTableStyles & styles = HtmlDataTableStyles(), int rows = 0, int columns = 0);

private:
	//! Metoda generuje/wypełnia/styluje nagłówki tabeli
	//! \param content Zawartośc tabeli którą uzupełniamy
	//! \param cellAttributes Atrybuty komórek
	//! \param rowsData Dane wiersza
	//! \param structure Układ tabeli - ilośc nagłówków...
	//! \param styles Style tabeli
	//! \param rows Ilość wierszy które będą brane pod uwagę
	//! \param columns Ilość kolumn które będą brane pod uwagę
    static void buildHeaders(HtmlDataTableContent & content, const HtmlDataTableCellAttributes & cellAttributes,
        HtmlDataTableContent::Column & rowsData, const TableHeadersStructureDescriptor & structure,
        const HtmlDataTableStyles & styles, int rows, int columns);

	//! Metoda generuje zawartość komórki tabeli
	//! \param content [out] Zawartość komórki
	//! \param style Styl komórki
	//! \param cellAttributes Atrybuty komórki
    static void buildCell(QString & content, const QString & style = QString(), const QString & cellAtributes = QString());

	//! Metoda generuje zawartość tabeli (dane, nie nagłówki)
    static void buildContent(HtmlDataTableContent & content, const HtmlDataTableCellAttributes & cellAttributes,
        const TableHeadersStructureDescriptor & structure, const HtmlDataTableStyles & styles, int rows, int columns);

	//! Metoda otwiera znaczniki HTML tabeli
	//! \param table [out] Dokument HTML do którego generujemy tabelę
	//! \param style Styl tabeli
	//! \param attributes Atrybuty tabeli
    static void openTable(QString & table, const QString & style, const HtmlTableAttributes & attributes);
	//! Metodad zamyka znaczniki HTML tabeli
	//! \param table [out] Dokument HTML do którego generujemy tabelę
    static void closeTable(QString & table);
	//! Metoda otwiera znaczniki HTML wiersza tabeli
	//! \param table [out] Dokument HTML do którego generujemy tabelę
	//! \param style Styl wiersza
    static void openRow(QString & table, const QString & style);
	//! Metodad zamyka znaczniki HTML wiersza tabeli
	//! \param table [out] Dokument HTML do którego generujemy tabelę
    static void closeRow(QString & table);
};

typedef std::array<std::array<std::pair<QTextTableCellFormat, QTextBlockFormat>,4>,4> TableCellsFormat;

//! Kalsa pomocnicza przy wypełnianiu tabeli wg standardów Qt
class QTextTableViewHelper
{
public:
	QTextTableViewHelper();
	~QTextTableViewHelper();

	void setHeadersCellsFormat(const TableCellsFormat & format);
	void setDataCellsFormat(const TableCellsFormat & format);
	void setTableFormat(const QTextTableFormat & format);

	void setHeaderRows(int headerRows);
	void setHeaderColumns(int headerColumns);

	const TableCellsFormat & headersCellsFormat() const;
	const TableCellsFormat & dataCellsFormat() const;
	const QTextTableFormat & tableFormat() const;

	int headerRows() const;
	int headerColumns() const;

	void update(QTextTable * table);
	void updateContent(QTextTable * table);
	void updateHeaders(QTextTable * table);
	void updateTable(QTextTable * table);

private:

	int headerRows_;
	int headerColumns_;

	TableCellsFormat headersCellsFormat_;
	TableCellsFormat dataCellsFormat_;
	QTextTableFormat tableFormat_;
};

}
#endif	//	HEADER_GUARD___TABLEGENERATOR_H__
