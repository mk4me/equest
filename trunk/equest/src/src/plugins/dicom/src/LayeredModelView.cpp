#include "DicomPCH.h"
#include <QtGui/QPixmap>
#include "LayeredModelView.h"
#include <plugins/dicom/Annotations.h>
#include <QtGui/QFont>
#include "PointsLayer.h"
#include "LayeredImageVisualizer.h"
#include <coreui/CoreVisualizerWidget.h>
#include <coreui/CoreDockWidget.h>
#include <utils/modeltest.h>

using namespace dicom; 

LayeredModelView::LayeredModelView(LayeredImageVisualizer *parent)
    :QAbstractItemModel(parent), layeredVisualizer(parent)
{
#ifdef _DEBUG
    //new ModelTest(this, this);
#endif
}

//-----------------------------------------------------------------
int LayeredModelView::rowCount(const QModelIndex & parent) const
{
    if (!parent.isValid()) {
        if (!image) {
            return 0;
        } else if (layeredVisualizer->userIsReviewer()) {
            return image->getNumTags();
        } else {
            auto tags = image->getTags();
            auto it = std::find(tags.begin(), tags.end(), layeredVisualizer->getUserName());
            return it != tags.end() ? 1 : 0;
        }
    } else {
        treedata* td = static_cast<treedata*>(parent.internalPointer());
        if (td->idx == -1 && parent.column() == 0) {
            std::string tag = image->getTag(td->tag);
            return image->getNumLayerItems(tag);
        } else {
            return 0;
        }
    }
}

//-----------------------------------------------------------------
int LayeredModelView::columnCount(const QModelIndex & parent) const
{
    if (!parent.isValid()) {
        return 2;
    } else {
        treedata* td = static_cast<treedata*>(parent.internalPointer());
        if (td->idx == -1 && parent.column() == 0) {
            return 2;
        } else {
            return 0;
        }
    }

}

//-----------------------------------------------------------------
QVariant LayeredModelView::data(const QModelIndex &index, int role) const
{
    if (image && index.isValid()) {
        if ( role == Qt::CheckStateRole){
			if( index.column() == 0 ) {
				treedata* td = static_cast<treedata*>(index.internalPointer());
				if (td->idx == -1) {
					std::string tag = image->getTag(td->tag);
					return static_cast< int >(image->getTagVisible(tag) ? Qt::Checked : Qt::Unchecked);
				}
			}
        }else if (role == Qt::DecorationRole) {
            treedata* td = static_cast<treedata*>(index.internalPointer());
            if (td->idx != -1 && index.column() == 0) {
                std::string tag = image->getTag(td->tag);
                ILayerItemConstPtr itm = image->getLayerItem(tag, index.row());
                return getItemColorPixmap(itm);
            }
        } else if (role == Qt::DisplayRole) {
            treedata* td = static_cast<treedata*>(index.internalPointer());
            std::string tag = image->getTag(td->tag);
            if (td->idx == -1) {
                if (index.column() == 0) {
                    return QString::fromStdString(tag);
                }
            } else {
                ILayerItemConstPtr itm = image->getLayerItem(tag, index.row());
                if (index.column() == 0) {
                    return itm->getName();
                } else if (index.column() == 1) {

					auto aidx = itm->getAdnotationIdx();

					switch(aidx) {

					case annotations::bloodLevel:
						{
							auto bil = boost::dynamic_pointer_cast<const BloodLevelLayer>(itm);
							if(bil != nullptr){

								auto blt = annotations::instanceBloodLevels();
								return blt->left.at(bil->value());								
							}
						}
						break;

					case annotations::inflammatoryLevel:
						{
							auto ail = boost::dynamic_pointer_cast<const InflammatoryLevelLayer>(itm);
							if(ail != nullptr){

								auto alt = annotations::instanceInflammatoryLevels();
								return alt->left.at(ail->value());
							}
						}
						break;

					case annotations::fingerType:
						{
							auto fil = boost::dynamic_pointer_cast<const FingerTypeLayer>(itm);
							if(fil != nullptr){

								auto flt = annotations::instanceFingerTypes();
								return flt->left.at(fil->value());
							}
						}
						break;

					case annotations::jointType:
						{
							auto jil = boost::dynamic_pointer_cast<const JointTypeLayer>(itm);
							if(jil != nullptr){

								auto jlt = annotations::instanceJointTypes();
								return jlt->left.at(jil->value());
							}
						}
						break;

					case annotations::imageType:
						{
							auto jil = boost::dynamic_pointer_cast<const ImageQualityLayer>(itm);
							if(jil != nullptr){
								auto jlt = annotations::instanceImageTypes();
								return jlt->left.at(jil->value());
							}
						}
						break;

					default:
						{
							annotations::AdnotationsTypePtr adn = annotations::instance();
							auto it = adn->left.find(static_cast<annotations::annotationsIdx>(aidx));
							if (it != adn->left.end()) {
								return it->second;
							}
						}
						break;
					}
                }
            }
            
        } else if (role == Qt::FontRole) {
			treedata* td = static_cast<treedata*>(index.internalPointer());
            if (td->idx != -1) {
                std::string tag = image->getTag(td->tag);
                ILayerItemConstPtr itm = image->getLayerItem(tag, index.row());
				auto gitm = boost::dynamic_pointer_cast<const ILayerGraphicItem>(itm);
                QFont f;
				if(gitm != nullptr){
					f.setBold(gitm->getSelected());
				}
                return f;
            }
        }
    }
    
    return QVariant();
}

//-----------------------------------------------------------------
//! [quoting mymodel_e]
bool LayeredModelView::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (!index.isValid()) {
        return false;
    } else if ( role == Qt::CheckStateRole) {
        treedata* td = static_cast<treedata*>(index.internalPointer());
        if (td->idx == -1) {
            Qt::CheckState checked = static_cast< Qt::CheckState >( value.toInt() );
            std::string tag = image->getTag(td->tag);
            image->setTagVisible(tag, checked == Qt::Checked);
			emit changeSelection(index);
        }

    } else if (role == Qt::EditRole) {

		auto w = layeredVisualizer->createWidget();
		if(w != nullptr){
			w->setWindowModified(true);
		}

        treedata* td = static_cast<treedata*>(index.internalPointer());
        int idx = td->idx;
        if (idx != -1) {
            std::string tag = image->getTag(td->tag);
            int col = index.column();
            if (image->getNumLayerItems(tag) > idx ) {
                if (col == 0) {
                    auto result = value.toString();
                    if (!result.isEmpty()) {
                        image->getLayerItem(tag, idx)->setName(result);
                        Q_EMIT editCompleted( result );
                    }
                } else if (col == 1) {

					auto il = image->getLayerItem(tag, idx);

					auto aidx = il->getAdnotationIdx();

					switch(aidx) {

					case annotations::bloodLevel:
						{
							auto bil = boost::dynamic_pointer_cast<BloodLevelLayer>(il);
							if(bil != nullptr){

								int val = value.toInt();
								bil->setValue((dicom::annotations::bloodLevelDescriptor)val);

								auto blt = annotations::instanceBloodLevels();
								QString result = blt->left.at(bil->value());
								Q_EMIT editCompleted( result );
							}
						}
						break;

					case annotations::inflammatoryLevel:
						{
							auto ail = boost::dynamic_pointer_cast<InflammatoryLevelLayer>(il);
							if(ail != nullptr){

								int val = value.toInt();
								ail->setValue((dicom::annotations::inflammatoryLevelDescriptor)val);

								auto alt = annotations::instanceInflammatoryLevels();
								QString result = alt->left.at(ail->value());
								Q_EMIT editCompleted( result );
							}
						}
						break;

					case annotations::fingerType:
						{
							auto fil = boost::dynamic_pointer_cast<FingerTypeLayer>(il);
							if(fil != nullptr){

								int val = value.toInt();
								fil->setValue((dicom::annotations::fingerTypeDescriptor)val);

								auto flt = annotations::instanceFingerTypes();
								QString result = flt->left.at(fil->value());
								Q_EMIT editCompleted( result );
							}
						}
						break;

					case annotations::jointType:
						{
							auto jil = boost::dynamic_pointer_cast<JointTypeLayer>(il);
							if(jil != nullptr){

								int val = value.toInt();
								jil->setValue((dicom::annotations::jointTypeDescriptor)val);

								auto jlt = annotations::instanceJointTypes();
								QString result = jlt->left.at(jil->value());
								Q_EMIT editCompleted( result );
							}
						}
						break;

					case annotations::imageType:
						{
							auto jil = boost::dynamic_pointer_cast<ImageQualityLayer>(il);
							if(jil != nullptr){

								int val = value.toInt();
								jil->setValue((dicom::annotations::imageTypeDescriptor)val);

								auto jlt = annotations::instanceImageTypes();
								QString result = jlt->left.at(jil->value());
								Q_EMIT editCompleted( result );
							}
						}
						break;

					/*
					default:
						{
							QString result = value.toString();
							auto adn = adnotations::instance();
							int adnotationIdx = adn->right.at(result);

							//! TODO
							//il->setAdnotationIdx(adnotationIdx);
							Q_EMIT editCompleted( result );
						}
						break;
					*/
					}
                }
            }
        }
    }
    return true;
}
//! [quoting mymodel_e]

//-----------------------------------------------------------------
//! [quoting mymodel_f]
Qt::ItemFlags LayeredModelView::flags(const QModelIndex & index) const
{
    if (!index.isValid()) {
        return 0;
    }

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	auto col = index.column();
    treedata* td = static_cast<treedata*>(index.internalPointer());
    if (td->idx == -1) {
        if ( col == 0 ) {
            flags |= Qt::ItemIsUserCheckable;
        }
    } else {

		std::string tag = image->getTag(td->tag);
		if (image->getNumLayerItems(tag) > td->idx ) {
			if (col == 0) {
				if(image->getLayerItem(tag, td->idx)->getAdnotationIdx() < 8){
					flags |=  Qt::ItemIsEditable;
				}
			}else{
				flags |=  Qt::ItemIsEditable;
			}
		}        
    }

    return flags;
}

QVariant LayeredModelView::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */ ) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return tr("Tag");
            case 1:
                return tr("Label");
            }
        }
    }
    return QVariant();
}

dicom::ILayeredImageConstPtr dicom::LayeredModelView::getImage() const
{
    return image;
}

void dicom::LayeredModelView::setImage( ILayeredImagePtr val )
{
    image = val;
    //this->reset();
    this->refreshSelected();
}

bool dicom::LayeredModelView::removeRows( int row, int count, const QModelIndex &parent /*= QModelIndex( ) */ )
{
    std::list<ILayerItemConstPtr> toRemove;
    // TODO
    /*int endRow = (std::min)(row + count, image->getNumLayerItems(TODO));
    for (int i = row; i < endRow; ++i) {
        toRemove.push_back(image->getLayerItem(i));
    }
    
    for (auto it = toRemove.begin(); it != toRemove.end(); ++it) {
        image->removeLayer(*it);
    }*/

    return !toRemove.empty();
}

QModelIndex dicom::LayeredModelView::index( int row, int column, const QModelIndex &parent /*= QModelIndex( ) */ ) const
{
    if (!image || column < 0 || parent.column() > 0) {
        return QModelIndex();
    }
    if (!parent.isValid()) {
        if (layeredVisualizer->userIsReviewer()) {
            if (row >= 0 && row < image->getNumTags()) {
                return createIndex(row, column, (void*)getData(row, -1));
            }
        } else if (row == 0) { 
            for (int i = image->getNumTags() - 1; i >= 0; --i) {
                if (layeredVisualizer->getUserName() == image->getTag(i)) {
                    return createIndex(row, column, (void*)getData(i, -1));
                }
            }
        }
        return QModelIndex();
    } else {
        treedata* td = static_cast<treedata*>(parent.internalPointer());
        if (td->idx == -1) {
            return createIndex(row, column, (void*)getData(td->tag, row));
        } else {
            return QModelIndex();
        }
    }


}

QModelIndex dicom::LayeredModelView::parent( const QModelIndex &child ) const
{
    if (child.isValid()) {
        treedata* td = static_cast<treedata*>(child.internalPointer());
        if (td->idx != -1) {
            return createIndex(layeredVisualizer->userIsReviewer() ? td->tag : 0, 0, (void*)getData(td->tag, -1));
        }
    }
    return QModelIndex();
}

const dicom::LayeredModelView::treedata* dicom::LayeredModelView::getData( int tag, int idx ) const
{
    // insert zwraca pare <iterator do elementu, flaga okreslajaca czy element isnial czy wlasnie zostal dodany> 
    treedata t = {tag, idx};
    return &(*(datas.insert(t).first));
}

std::pair<int, int> dicom::LayeredModelView::getTagAndIndex( const QModelIndex& idx )
{
    if (idx.isValid()) {
        treedata* td = static_cast<treedata*>(idx.internalPointer());
        if (td) {
            return std::make_pair(td->tag, td->idx);
        }
    }

    return std::make_pair(-1, -1);
}

void dicom::LayeredModelView::refreshSelected()
{
    Q_EMIT layoutChanged();
}

dicom::LayeredModelView::Expands dicom::LayeredModelView::getExpands() const
{
    Expands exp;
    if (layeredVisualizer->userIsReviewer()) {
        for (int row = image->getNumTags() - 1; row >= 0; --row) {
            exp.push_back(std::make_pair(createIndex(row, 0, (void*)getData(row, -1)), image->getTagVisible(image->getTag(row))));
        }
    } else {
        for (int i = image->getNumTags() - 1; i >= 0; --i) {
            if (image->getTag(i) == layeredVisualizer->getUserName()) {
                exp.push_back(std::make_pair(createIndex(0, 0, (void*)getData(i, -1)), image->getTagVisible(image->getTag(i))));
                break;
            }
        }
        
    }
    return exp;
}

QPixmap dicom::LayeredModelView::getItemColorPixmap( ILayerItemConstPtr itm ) const
{
    return DrawersBuilder::getColorPixmap(static_cast<annotations::annotationsIdx>(itm->getAdnotationIdx()));
}

//! [quoting mymodel_f]
