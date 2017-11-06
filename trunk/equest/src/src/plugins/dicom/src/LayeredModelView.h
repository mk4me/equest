/********************************************************************
	created:	2013/11/04
	created:	4:11:2013   20:46
	filename: 	LayeredModelView.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYEREDMODELVIEW_H__
#define HEADER_GUARD_DICOM__LAYEREDMODELVIEW_H__

#include <QtCore/QAbstractItemModel>
#include <QtCore/QString>
#include <plugins/dicom/ILayeredImage.h>

namespace dicom {

	class LayeredImageVisualizer;

class LayeredModelView : public QAbstractItemModel
{
    Q_OBJECT
public:
    LayeredModelView(LayeredImageVisualizer* parent);

public:
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    
    virtual Qt::ItemFlags flags(const QModelIndex & index) const ;
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    virtual QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex( ) ) const;
    virtual QModelIndex parent( const QModelIndex &child ) const;

public:
    ILayeredImageConstPtr getImage() const;
    void setImage(ILayeredImagePtr val);
	void refresh() {
		beginResetModel();
		endResetModel();
	}
    void refreshSelected();
    typedef std::vector<std::pair<QModelIndex, bool>> Expands;
    Expands getExpands() const;

    static std::pair<int, int> getTagAndIndex(const QModelIndex& idx);

protected:
    virtual bool removeRows( int row, int count, const QModelIndex &parent = QModelIndex( ) );

public:
    struct treedata
    {
        int tag, idx;
        bool operator <(const treedata& d) const
        {
            if (tag == d.tag) {
                return idx < d.idx;
            }
            return tag < d.tag ;
        }
    };

    mutable std::set<treedata> datas;
    const treedata* getData(int tag, int idx) const;
    QPixmap getItemColorPixmap( ILayerItemConstPtr itm ) const;
private:
    ILayeredImagePtr image;
	LayeredImageVisualizer* layeredVisualizer;
    
Q_SIGNALS:
    void editCompleted(const QString &);
	void changeSelection(const QModelIndex&);
};

}

#endif
