#include "DicomPCH.h"
#include "AnnotationsDelegate.h"
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <plugins/dicom/Annotations.h>
#include <QtWidgets/QListView>
#include "LayeredModelView.h"

using namespace dicom;

AdnotationsDelegate::AdnotationsDelegate(const int column, QObject *parent /*= 0*/ ) : 
    QStyledItemDelegate(parent), column(column), editionActive(false)
{

}

void AdnotationsDelegate::setEditionActive(const bool active)
{
	editionActive = active;
}

void AdnotationsDelegate::setImage(dicom::ILayeredImageConstPtr val)
{
	this->val = val;
}

QWidget * AdnotationsDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex & index ) const
{
	QWidget * ret = nullptr;
	if(val != nullptr && editionActive == true){

		LayeredModelView::treedata* td = static_cast<LayeredModelView::treedata*>(index.internalPointer());
		int idx = td->idx;
		if (idx != -1) {
			std::string tag = val->getTag(td->tag);
			if (val->getNumLayerItems(tag) > idx ) {
				auto il = val->getLayerItem(tag, idx);
				auto aidx = il->getAdnotationIdx();

				if(aidx > 7){

					if(column == 1){
						QComboBox *editor = new QComboBox(parent);
						// z doc : "Sets the view to be used in the combobox popup to the given itemView. The combobox takes ownership of the view."
						editor->setView(new QListView());
						editor->setStyleSheet("QComboBox QAbstractItemView::item { min-height: 35px}");
						switch(aidx) {

						case annotations::bloodLevel:
							{
								auto adn = dicom::annotations::instanceBloodLevels();
								for(auto it = adn->left.begin(); it != adn->left.end(); ++it){
									editor->addItem(it->second, it->first);
								}
							}
							break;

						case annotations::inflammatoryLevel:
							{
								auto adn = dicom::annotations::instanceInflammatoryLevels();
								for(auto it = adn->left.begin(); it != adn->left.end(); ++it){
									editor->addItem(it->second, it->first);
								}
							}
							break;

						case annotations::fingerType:
							{
								auto adn = dicom::annotations::instanceFingerTypes();
								for(auto it = adn->left.begin(); it != adn->left.end(); ++it){
									editor->addItem(it->second, it->first);
								}
							}
							break;

						case annotations::jointType:
							{
								auto adn = dicom::annotations::instanceJointTypes();
								for(auto it = adn->left.begin(); it != adn->left.end(); ++it){
									editor->addItem(it->second, it->first);
								}
							}
							break;

						case annotations::imageType:
							{
								auto adn = dicom::annotations::instanceImageTypes();
								for(auto it = adn->left.begin(); it != adn->left.end(); ++it){
									editor->addItem(it->second, it->first);
								}
							}
							break;
						}

						ret = editor;
					}
				}else if(column == 0){
					ret = new QLineEdit(parent);					
				}			
			}
		}
	}

    return ret;
}

void AdnotationsDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();

	bool set = false;

	if(val != nullptr){

		LayeredModelView::treedata* td = static_cast<LayeredModelView::treedata*>(index.internalPointer());
		int idx = td->idx;
		if (idx != -1) {
			std::string tag = val->getTag(td->tag);
			if (val->getNumLayerItems(tag) > idx ) {
				auto il = val->getLayerItem(tag, idx);
				auto aidx = il->getAdnotationIdx();

				if(aidx > 7){

					QComboBox *box = static_cast<QComboBox*>(editor);
					int idx = box->findText(value);
					box->setCurrentIndex(idx);
					set = true;
					connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(onChange()));

				}
			}
		}	
	}
	
	if(set == false) {
		QLineEdit * edit = static_cast<QLineEdit*>(editor);
		edit->setText(value);
		connect(edit, SIGNAL(textChanged(const QString &)), this, SLOT(onChange()));
	}
}

void AdnotationsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const
{
	bool set = false;

	if(val != nullptr){
		LayeredModelView::treedata* td = static_cast<LayeredModelView::treedata*>(index.internalPointer());
		int idx = td->idx;
		if (idx != -1) {
			std::string tag = val->getTag(td->tag);
			if (val->getNumLayerItems(tag) > idx ) {
				auto il = val->getLayerItem(tag, idx);
				auto aidx = il->getAdnotationIdx();

				if(aidx > 7){
					QComboBox *box = static_cast<QComboBox*>(editor);		

					model->setData(index, box->itemData(box->currentIndex()), Qt::EditRole);

					set = true;
				}
			}
		}
	}
				
	if(set == false){
		QLineEdit * edit = static_cast<QLineEdit*>(editor);
		QString value = edit->text();
		model->setData(index, value, Qt::EditRole);
	}
}

void AdnotationsDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

void AdnotationsDelegate::onChange()
{
	QWidget * w = qobject_cast<QWidget*>(sender());
	emit commitData(w);
	emit closeEditor(w);
}



