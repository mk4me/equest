#include "PCH.h"
#include <osgui/Grid.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

// class WidgetIndexHack : public osgWidget::Widget
// {
// public:
//     void setIndex(unsigned index)
//     {
//         _index = index;
//     }
// };

Grid::Grid( const std::string& name /*= ""*/, unsigned rows /*= 0*/, unsigned columns /*= 0*/ ) :
osgWidget::Table(name, rows, columns), dirtyMode(false)
{
    rowsWeights.resize(rows, 1);
    columnsWeights.resize(columns, 1);
}

Grid::Grid( const Grid& grid, const osg::CopyOp& copyop ) :
osgWidget::Table(grid, copyop), rowsWeights(grid.rowsWeights), columnsWeights(grid.columnsWeights), dirtyMode(grid.dirtyMode)
{
    // TODO: zaproponować fixa dla OSG
    UTILS_ASSERT(false, "osgWidget::Table ma błędnie działający konstruktor kopiujący");
}

void Grid::setDimensions( unsigned rows, unsigned columns )
{
    // kopia starego stanu
    UIObjectParent::Vector oldObjects;
    unsigned oldRows = getNumRows();
    unsigned oldColumns = getNumColumns();
    oldObjects.swap(_objects);
    // wypełnienie tablicy na nowo
    _objects.resize( rows * columns );
    for ( unsigned y = 0; y < rows && y < oldRows; ++y ) {
        // skopiowanie tych samych
        for ( unsigned x = 0; x < columns && x < oldColumns; ++x)  {
            _objects[y * columns + x] = oldObjects[y * oldColumns + x];
        }
        // usnięcie starych kolumn
        for ( unsigned x = columns; x < oldColumns; ++x ) {
            osg::ref_ptr<osgWidget::Widget> ptr;
            if ( oldObjects[y * oldColumns + x].lock(ptr) ) {
                _removeFromGeode(ptr.get());
            }
        }
    }
    // usunięcie starych wierszy
    for ( unsigned y = rows; y < oldRows; ++y ) {
        for ( unsigned x = 0; x < oldColumns; ++x ) {
            osg::ref_ptr<osgWidget::Widget> ptr;
            if ( oldObjects[y * oldColumns + x].lock(ptr) ) {
                _removeFromGeode(ptr.get());
            }
        }
    }

    _rows = rows;
    _cols = columns;

    rowsWeights.resize(rows, 1);
    columnsWeights.resize(columns, 1);

    // odswiezenie rozmiaru
    if ( !isDirtyMode() ) {
        resizeAdd();
    }
}

void Grid::adjustDimensions( osgWidget::point_type width, osgWidget::point_type height, const std::vector<osgWidget::point_type>& aspectRatios )
{
    using namespace osgWidget;
    UTILS_ASSERT(aspectRatios.size() == getNumObjects());



    // obliczenie optymalnego rozmiaru
    osg::Vec2s optimalDimensions(1, 1);
    XYCoord optimalCellSize(width, height);
    double optimalField = -DBL_MAX;
    for (unsigned rows = 1; rows <= getNumObjects(); ++rows) {
        unsigned columns = static_cast<unsigned>( ceil(static_cast<double>(getNumObjects())/rows) );

        // rozmiar komórki
        XYCoord cellSize = XYCoord( width/columns, height/rows );
        double cellRatio = cellSize.x() / cellSize.y();
        double usedField = 0.0;

        // zsumowanie pól widgetów
        for(auto aspectRatio : aspectRatios) {
            XYCoord widgetSize = cellSize;
            if ( aspectRatio > cellRatio ) {
                widgetSize.y() = widgetSize.x() / aspectRatio;
            } else {
                widgetSize.x() = widgetSize.y() * aspectRatio;
            }
            usedField += widgetSize.x() * widgetSize.y();
        }

        // sprawdzenie, czy nowe rozwiązanie jest lepsze
        if ( usedField > optimalField ) {
            optimalField = usedField;
            optimalDimensions = osg::Vec2s(columns, rows);
            optimalCellSize = cellSize;
        }
    }

    //LOG_ERROR("size: "<<width<<" "<<height<<" dim"<<optimalDimensions.x()<<" "<<optimalDimensions.y());


    // nadanie optymalnego rozmiaru
    // tutaj lekko zhackowana zmiana rozmiaru w stosunku do setDimensions

    // można tak zrobić, gdyż nowy rozmiar >= bieżący
    // TODO: czy to poprawne? może powodować rozrost do nieskończoności
    _objects.resize( optimalDimensions.x() * optimalDimensions.y() );
    _rows = optimalDimensions.y();
    _cols = optimalDimensions.x();
    rowsWeights.assign(_objects.size(), 1);
    columnsWeights.assign(_objects.size(), 1);

    point_type y = 0.0f;
    for(unsigned int row = 0; row < _rows; ++row) {
        point_type x = 0.0f;
        for(unsigned int col = 0; col < _cols; ++col) {
            Widget* widget = _objects[_calculateIndex(row, col)].get();
            if(widget) {
                widget->setOrigin(x, y);
                _positionWidget(widget, optimalCellSize.x(), optimalCellSize.y());
            }
            x += optimalCellSize.x();
        }
        y += optimalCellSize.y();
    }

    if ( !isDirtyMode() ) {
        resizeAdd();
    }
}

// void Grid::adjustDimensions( osgWidget::point_type width, osgWidget::point_type height, osgWidget::point_type aspectRatio /*= -1*/ )
// {
//     // obliczenie optymalnego rozmiaru
//     // TODO: da się to zapewne zrobić analitycznie, ale na razie
//     // mamy tutaj prosty algorytm empiryczny
//     osg::Vec2s optimalDimensions(0, 0);
//     osg::Vec2 optimalCellSize(0, 0);
//     double optimalField = 0.0;
//
//     // dla każdej konfiguracji badamy zajęte pole
//     for ( unsigned rows = 1; rows < getNumObjects(); ++rows ) {
//         unsigned columns = static_cast<unsigned>(ceil(double(getNumObjects()) / rows));
//
//         // obliczenie maksymalnego rozmiaru przypadającego na komórkę
//         osg::Vec2 size = osg::Vec2( width/columns, height/rows );
//
//         double cellRatio = size.x() / size.y();
//         if ( aspectRatio > cellRatio ) {
//             size.y() = size.x() / aspectRatio;
//         } else {
//             size.x() = size.y() * aspectRatio;
//         }
//
//         // czy zajęto łącznie więcej miejsca niż poprzednio?
//         double usedField = size.x() * size.y();
//         if ( usedField > optimalField )
//         {
//             optimalField = usedField;
//             optimalDimensions = osg::Vec2s(rows, columns);
//             optimalCellSize = size;
//         }
//     }
//
//     UIObjectParent::Vector oldObjects;
//     oldObjects.swap(_objects);
//     oldObjects.resize( optimalDimensions.x() * optimalDimensions.y() );
//
//     _objects.resize(getNumRows() * getNumColumns());
//     setDimensions(optimalDimensions.x(), optimalDimensions.y());
//
//     oldObjects.swap(_objects);
//     for ( Iterator it = begin(); it != end(); ++it ) {
//         if ( it->valid() ) {
//             (*it)->setSize(  optimalCellSize.x(), optimalCellSize.y() );
//         }
//     }
//     resizeAdd();
// }

// void Grid::adjustDimensions( osgWidget::point_type width, osgWidget::point_type height, const std::vector<osgWidget::point_type>& aspectRatios )
// {
//
// }

void Grid::resetFillables()
{
    using namespace osgWidget;
    for(unsigned int row = 0; row < _rows; ++row) {
        for(unsigned int col = 0; col < _cols; ++col) {
            Widget* widget = _objects[_calculateIndex(row, col)].get();
            if(widget && widget->canFill() ) {
                widget->setSize(0, 0);
            }
        }
    }
    if ( !isDirtyMode() ) {
        resizeAdd();
    }
}

void Grid::_resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight )
{
    using namespace osgWidget;
    // kod wzięty z osgWidget::Table, nieznacznie zmodyfikowany
    CellSizes rowsWeights(this->rowsWeights);
    CellSizes columnsWeights(this->columnsWeights);

    // We use these vectors so that we don't have to repeatedly call isFillable
    // all the time. Usage such as this can really generate a lot of moronic,
    // misinformed opposition, but until std::bit_vector is available, this is
    // what we get. Deal with it.
    std::vector<bool> rowFills;
    std::vector<bool> colFills;

    point_type numRowFills = 0.0f;
    point_type numColFills = 0.0f;

    // Enumerate each row and determine whether it can fill. If so, increment
    // our numRowFills variable and set the position in rowFills to "true."
    for(unsigned int row = 0; row < _rows; ++row) {
        bool fill = isRowVerticallyFillable(row);

        if(fill) {
            ++numRowFills;
        } else {
            rowsWeights[row] = 0.0f;
        }

        rowFills.push_back(fill);
    }

    // Enumerate each column and determine whether it can fill. If so, increment
    // our numColFills variable and set the position in colFills to "true."
    for(unsigned int col = 0; col < _cols; ++col) {
        bool fill = isColumnHorizontallyFillable(col);

        if(fill) {
            ++numColFills;
        } else {
            columnsWeights[col] = 0.0f;
        }

        colFills.push_back(fill);
    }


    if(numRowFills > 0) {
        point_type totalWeight = std::accumulate( rowsWeights.begin(), rowsWeights.end(), 0.0f );
        for(unsigned row = 0; row < _rows; ++row) {
            if ( rowFills[row] ) {
                float dh;
                if ( totalWeight != 0.0f ) {
                    dh = rowsWeights[row] / totalWeight * diffHeight;
                } else {
                    dh = 0;
                }
                addHeightToRow(row, osg::round(dh));
            }
        }
    }
    if(numColFills > 0.0f) {
        point_type totalWeight = std::accumulate( columnsWeights.begin(), columnsWeights.end(), 0.0f );
        for(unsigned col = 0; col < _cols; ++col) {
            if ( colFills[col] ) {
                float dw;
                if ( totalWeight != 0.0f ) {
                    dw = columnsWeights[col] / totalWeight * diffWidth;
                } else {
                    dw = 0;
                }
                addWidthToColumn(col, osg::round(dw));
            }
        }
    }

    CellSizes rowHeights;
    CellSizes colWidths;

    getRowHeights(rowHeights);
    getColumnWidths(colWidths);

    point_type y = 0.0f;

    for(unsigned int row = 0; row < _rows; ++row) {
        point_type x = 0.0f;

        for(unsigned int col = 0; col < _cols; ++col) {
            Widget* widget = _objects[_calculateIndex(row, col)].get();

            if(widget) {
                widget->setOrigin(x, y);

                _positionWidget(widget, colWidths[col], rowHeights[row]);
            }

            x += colWidths[col];
        }

        y += rowHeights[row];
    }



//     width = getWidth() + diffWidth;
//     height = getHeight() + diffHeight;
//
//     // implementacja bazowa
//     Table::_resizeImplementation(diffWidth, diffHeight);
//     if ( keepCellAspectRatio ) {
//
//     }
//
//
//     // sprawdzamy, czy jest ok
//     point_type calcWidth = Table::_getWidthImplementation().current;
//     point_type calcHeight = Table::_getHeightImplementation().current;
//
//     diffWidth = width - calcWidth;
//     diffHeight = height - calcHeight;
//
//     if ( diffWidth != 0.0 || diffHeight != 0.0 ) {
//         point_type dwidth = diffWidth / getColumns();
//         point_type dheight = diffHeight / getRows();
//         for ( unsigned i = 0; i < getNumObjects(); ++i ) {
//             Widget* widget = _objects[i].get();
//             if ( widget ) {
//                 _positionWidget(widget, widget->getWidth() + dwidth, widget->getHeight() + dheight);
//             }
//         }
//     }

}

osgWidget::point_type Grid::getRowWeight( unsigned row ) const
{
    UTILS_ASSERT(row < getNumRows());
    return rowsWeights[row];
}

void Grid::setRowWeight( unsigned row, osgWidget::point_type weight )
{
    UTILS_ASSERT(row < getNumRows());
    UTILS_ASSERT(weight >= 0);
    rowsWeights[row] = weight;
}

osgWidget::point_type Grid::getColumnWeight( unsigned column ) const
{
    UTILS_ASSERT(column < getNumColumns());
    return columnsWeights[column];
}

void Grid::setColumnWeight( unsigned column, osgWidget::point_type weight )
{
    UTILS_ASSERT(column < getNumColumns());
    UTILS_ASSERT(weight >= 0);
    columnsWeights[column] = weight;
}

void Grid::getColumnsWeights( CellSizes& weights ) const
{
    weights.assign( columnsWeights.begin(), columnsWeights.end() );
}

void Grid::getRowsWeights( CellSizes& weights ) const
{
    weights.assign( rowsWeights.begin(), rowsWeights.end() );
}

void Grid::setNumRows( unsigned rows )
{
    setDimensions(rows, getNumColumns());
}

void Grid::setNumColumns( unsigned columns )
{
    setDimensions(getNumRows(), columns);
}

void Grid::fillEmpty( osg::ref_ptr<osgWidget::Widget> prototype /*= osg::ref_ptr<osgWidget::Widget>() */ )
{
    if ( !prototype ) {
        prototype = new osgWidget::Widget("space");
        prototype->setCanFill(true);
        prototype->setColor(0,0,0,0);
    }
    for(unsigned int row = 0; row < _rows; ++row) {
        for(unsigned int col = 0; col < _cols; ++col) {
            osgWidget::Widget* widget = _objects[_calculateIndex(row, col)].get();
            if(!widget) {
                addWidget(  osg::clone(prototype.get(), osg::CopyOp::DEEP_COPY_ALL), row, col );
            }
        }
    }
    if ( !isDirtyMode() ) {
        resizeAdd();
    }
}

void Grid::flattenHorizontally()
{
    _cols = getNumObjects();
    _rows = 1;
    rowsWeights.assign(_rows, 1);
    columnsWeights.assign(_cols, 1);
    if ( !isDirtyMode() ) {
        resizeAdd();
    }
}

void Grid::flattenVertically()
{
    _rows = getNumObjects();
    _cols = 1;
    rowsWeights.assign(_rows, 1);
    columnsWeights.assign(_cols, 1);
    if ( !isDirtyMode() ) {
        resizeAdd();
    }
}

void Grid::setDirtyMode( bool dirtyMode )
{
    if ( this->dirtyMode ) {
        if ( !dirtyMode ) {
            // jeżeli wychodzimy z trybu dirty - automatyczny resize
            resizeAdd();
        }
    }
    this->dirtyMode = dirtyMode;
}

osg::Vec2s Grid::getDimensionsAsSquare( unsigned numItems )
{
    unsigned rows;
    unsigned columns = static_cast<unsigned>(ceil(sqrt(static_cast<double>(numItems))));
    if ( columns != 0 ) {
        if ( columns * (columns-1) >= numItems ) {
            rows = columns-1;
        } else {
            rows = columns;
        }
    } else {
        rows = 0;
    }
    return osg::Vec2s(columns, rows);
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////
