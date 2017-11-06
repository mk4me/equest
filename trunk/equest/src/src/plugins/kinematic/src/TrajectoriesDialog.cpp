#include "PCH.h"
#include "TrajectoriesDialog.h"
#include <QtWidgets/QTreeWidget>
#include <boost/array.hpp>
#include "TrajectoriesDrawer.h"
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QColorDialog>
#include "KinematicVisualizer.h"

class SchemeElementWidgetItem : public QTreeWidgetItem
{
public:

	SchemeElementWidgetItem(SchemeParametersEditor::TreeSchemeElement * element)
		: QTreeWidgetItem(QTreeWidgetItem::UserType), element_(element)
	{

	}

	SchemeParametersEditor::TreeSchemeElement * element()
	{
		return element_;
	}

	const SchemeParametersEditor::TreeSchemeElement * element() const
	{
		return element_;
	}

private:
	SchemeParametersEditor::TreeSchemeElement * element_;
};

const Qt::CheckState merge(const Qt::CheckState a, const Qt::CheckState b)
{
	if(a == b){
		return a;
	}

	return Qt::PartiallyChecked;
}

const Qt::CheckState updateStatusDown(QTreeWidgetItem * current)
{
	//najpierw w dół
	auto schemeElementItem = dynamic_cast<SchemeElementWidgetItem*>(current);	

	//auto children = schemeElementItem->element()->children();
	Qt::CheckState cs = schemeElementItem->element()->visible() == true ? Qt::Checked : Qt::Unchecked;

	for(unsigned int i = 0; i < current->childCount(); ++i){
		auto ccs = updateStatusDown(current->child(i));
		cs = merge(cs, ccs);
	}

	current->setCheckState(0, cs);
	current->setData(2, Qt::UserRole, schemeElementItem->element()->color());
	current->setData(3, Qt::UserRole, schemeElementItem->element()->size());

	return cs;
}

void updateStatusUp(QTreeWidgetItem * current)
{
	
}

void updateStatus(QTreeWidgetItem * current)
{
	updateStatusDown(current);
	updateStatusUp(current);
}

const Qt::CheckState initializeSchemeElementTree(SchemeParametersEditor::TreeSchemeElement * element,
	QTreeWidgetItem * parentItem)
{
	Qt::CheckState ret = element->visible() == true ? Qt::Checked : Qt::Unchecked;

	SchemeElementWidgetItem * wi = new SchemeElementWidgetItem(element);	
	auto children = element->children();
	for(auto it = children.begin(); it != children.end(); ++it){
		ret = merge(ret, initializeSchemeElementTree(*it, wi));
	}

	wi->setCheckState(0, ret);
	wi->setText(1, element->name());
	wi->setData(2, Qt::UserRole, element->color());
	wi->setData(3, Qt::UserRole, element->size());

	parentItem->addChild(wi);

	return ret;
}


SchemeParametersEditor::SchemeParametersEditor(KinematicVisualizer * visualizer, QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), treeWidget(new QTreeWidget), visualizer(visualizer)
{
	auto l = new QHBoxLayout;
	l->addWidget(treeWidget);
	setLayout(l);
	
	connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		this, SLOT(onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

	connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
		this, SLOT(onItemChanged(QTreeWidgetItem*, int)));

}

SchemeParametersEditor::~SchemeParametersEditor()
{

}

void SchemeParametersEditor::initialize(TreeSchemeElement * root)
{
	treeWidget->clear();
	initializeSchemeElementTree(root, treeWidget->invisibleRootItem());
}

void SchemeParametersEditor::onCurrentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous )
{
	emit currentSchemeElementChanged(dynamic_cast<SchemeElementWidgetItem*>(current)->element(),
		dynamic_cast<SchemeElementWidgetItem*>(previous)->element());
}

void SchemeParametersEditor::onItemChanged(QTreeWidgetItem * current, int column)
{
	if(column == 0 || column == 2 || column == 3){

		auto element = dynamic_cast<SchemeElementWidgetItem*>(current)->element();
		auto data = current->data(column, Qt::UserRole);

		switch(column){

		case 0:
			element->setVisible(current->checkState(0) == Qt::Unchecked ? false : true);
			break;

		case 2:				
			
			element->setColor(data.toInt());
			break;

		case 3:
			element->setSize(data.toFloat());
			break;
		}

		treeWidget->blockSignals(true);
		updateStatus(current);
		treeWidget->blockSignals(false);

		visualizer->requestUpdate();
	}
}












//
//
//#include <QtWidgets/QTableView>
//#include "TrajectoriesDialog.h"







TrajectoriesDialog::TrajectoriesDialog(QWidget* parent, KinematicVisualizer * visualizer) :
QDialog(parent), visualizer(visualizer)
{
	Ui::TrajectoriesDialog::setupUi(this);
	QIcon icon( QString::fromUtf8(":/kinematic/icons/trajectory.png") );
	QTreeWidgetItem* item = tree->headerItem();
	item->setIcon(0, icon);
	item->setText(0, "");
    
	connect(Ui::TrajectoriesDialog::colorButton, SIGNAL(clicked()), this, SLOT(colorClicked()));
	connect(thicknessSpin, SIGNAL(valueChanged(double)), this, SLOT(widthChanged(double)));
	connect(startTimeSpin, SIGNAL(valueChanged(double)), this, SLOT(startTimeChanged(double)));
	connect(endTimeSpin,   SIGNAL(valueChanged(double)), this, SLOT(endTimeChanged(double)));
	connect(startSlider,   SIGNAL(valueChanged(int)), this, SLOT(startSliderChanged(int)));
	connect(endSlider,     SIGNAL(valueChanged(int)), this, SLOT(endSliderChanged(int)));
	connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		    this, SLOT(treeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

void TrajectoriesDialog::setDrawer(TrajectoryDrawerManagerPtr drawer, const QString& rootName, const QStringList& names )
{
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(1, rootName);
    tree->addTopLevelItem(root);
    QCheckBox* rootCheck = new QCheckBox(tree);
    rootCheck->setChecked(true);
    tree->setItemWidget(root, 0, rootCheck);
    connect(rootCheck, SIGNAL(clicked(bool)), this, SLOT(rootVisibilityChanged(bool)));
    item2Root[root] = drawer;
    
    int count = drawer->count();
    if (count == names.size()) {
        for (int i = 0; i < count; ++i) {
            QTreeWidgetItem* item = new QTreeWidgetItem();

            item->setText(1, names[i]);

            item->setFlags(item->flags() | Qt::ItemIsSelectable);
            item->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
            root->addChild(item);

            QCheckBox* check1 = new QCheckBox(tree);
            tree->setItemWidget(item, 0, check1);
            connect(check1, SIGNAL(clicked(bool)), this, SLOT(visibilityChanged(bool)));
            item2Trajectories[item] = i;
        }
    } else {
        UTILS_ASSERT(false);
    }    
}




QColor TrajectoriesDialog::transformColor( const osg::Vec4& color ) const
{
	return QColor(
		static_cast<int>(255 * color[0]),
		static_cast<int>(255 * color[1]),
		static_cast<int>(255 * color[2]),
		static_cast<int>(255 * color[3])
		);
}

osg::Vec4 TrajectoriesDialog::transformColor( const QColor& color ) const
{
	return osg::Vec4 (
		static_cast<float>(color.red() / 255.0f),
		static_cast<float>(color.green() / 255.0f),
		static_cast<float>(color.blue() / 255.0f),
		static_cast<float>(color.alpha() / 255.0f)
		);

}

void TrajectoriesDialog::setButtonColor( QPushButton* button, const QColor& color )
{
	QString style = QString("QPushButton { background-color: rgb( %1, %2, %3); }").arg(color.red()).arg(color.green()).arg(color.blue());
	button->setStyleSheet(style);
}

void TrajectoriesDialog::colorClicked()
{
	QColor color(255, 255, 255, 80);// = transformColor(trajectories->getColor(name));
	color = QColorDialog::getColor ( color, this, tr("Choose color"), QColorDialog::ShowAlphaChannel);

    for (int j = 0; j < tree->topLevelItemCount(); ++j) {
        QTreeWidgetItem* topItem = tree->topLevelItem(j);
        int count = topItem->childCount();
        for (int i = 0; i < count; ++i) {
            QTreeWidgetItem* item = topItem->child(i);
            if (item && item->isSelected()) {
                //std::string name = item->text(1).toStdString();
                //currentTrajectories->setColor(name, transformColor(color));
                auto manager = item2Root[topItem];
                auto idx = item2Trajectories[item];
                manager->setColor(idx, transformColor(color));
                setButtonColor(colorButton, color);
            }
        }
    }

	visualizer->requestUpdate();
}

void TrajectoriesDialog::startTimeChanged( double time )
{
    for (int j = 0; j < tree->topLevelItemCount(); ++j) {
        QTreeWidgetItem* topItem = tree->topLevelItem(j);
        int count = topItem->childCount();
        for (int i = 0; i < count; ++i) {
            QTreeWidgetItem* item = topItem->child(i);
            if (item && item->isSelected()) {
                //std::string name = item->text(1).toStdString();
				TrajectoryDrawerManagerPtr manager = item2Root[topItem];
				int idx = item2Trajectories[item];
				osgutils::IRangeDrawer::Range times = manager->range(idx);
                blockAllSignals(true);
                startTimeSpin->setValue(time);
                startSlider->setValue(time);
                blockAllSignals(false);
                manager->setRange(idx, std::make_pair(static_cast<int>(time), times.second));
                //currentTrajectories->setTimes(name, std::make_pair(static_cast<float>(time), times.second));
            }
        }
    }

	visualizer->requestUpdate();
}

void TrajectoriesDialog::endTimeChanged( double time )
{
    for (int j = 0; j < tree->topLevelItemCount(); ++j) {
        QTreeWidgetItem* topItem = tree->topLevelItem(j);
        int count = topItem->childCount();
        for (int i = 0; i < count; ++i) {
            QTreeWidgetItem* item = topItem->child(i);
            if (item && item->isSelected()) {
                auto manager = item2Root[topItem];
                auto idx = item2Trajectories[item];
				osgutils::IRangeDrawer::Range times = manager->range(idx);
                blockAllSignals(true);
                endTimeSpin->setValue(time);
                endSlider->setValue(time);
                blockAllSignals(false);
                //const std::pair<float, float>& times = currentTrajectories->getTimes(name);

                manager->setRange(idx, std::make_pair(times.first, static_cast<int>(time)));
                //currentTrajectories->setTimes(name, std::make_pair(times.first, static_cast<float>(time)));
            }
        }
    }

	visualizer->requestUpdate();
}

void TrajectoriesDialog::treeItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous )
{
    auto parent = current->parent();
    if (parent) {
        TrajectoryDrawerManagerPtr manager = item2Root[parent];
        int idx = item2Trajectories[current];
                
	    QColor color = transformColor(manager->color(idx));
	    setButtonColor(colorButton, color);
	    blockAllSignals(true);
	    //const std::pair<float, float>& times = currentTrajectories->getTimes(name);
		osgutils::IRangeDrawer::Range times = manager->range(idx);
        startTimeSpin->setMinimum(0);
        startTimeSpin->setMaximum(manager->maxRange(idx));
        endTimeSpin->setMinimum(0);
        endTimeSpin->setMaximum(manager->maxRange(idx));
        startSlider->setMinimum(0);
        startSlider->setMaximum(manager->maxRange(idx));
        endSlider->setMinimum(0);
        endSlider->setMaximum(manager->maxRange(idx));

	    startTimeSpin->setValue(times.first);
	    endTimeSpin->setValue(times.second);
	    startSlider->setValue(times.first);
	    endSlider->setValue(times.second);
	    float thickness = manager->size(idx);
	    thicknessSpin->setValue(thickness);
	    blockAllSignals(false);
    }
}

void TrajectoriesDialog::startSliderChanged(int value)
{
	if (endSlider->value() > value) {
		startTimeChanged(value);
	} else {
		startSlider->setValue(endSlider->value() - 1);
	}
}

void TrajectoriesDialog::endSliderChanged(int value)
{
	if (value > startSlider->value()) {
		endTimeChanged(value);
	} else {
		endSlider->setValue(startSlider->value() + 1);
	}
}

void TrajectoriesDialog::blockAllSignals( bool val )
{
	this->blockSignals(val);
	startSlider->blockSignals(val);
	startTimeSpin->blockSignals(val);
	endTimeSpin->blockSignals(val);
	endSlider->blockSignals(val);
	thicknessSpin->blockSignals(val);
	
}

void TrajectoriesDialog::rootVisibilityChanged( bool visible )
{
    QCheckBox* box = qobject_cast<QCheckBox*>(sender());
    if (box) {
        QTreeWidgetItem* item = getItemWhichContains(box);
        item2Root[item]->setVisible(visible);
		visualizer->requestUpdate();
    }
}

void TrajectoriesDialog::widthChanged( double width )
{
    for (int j = 0; j < tree->topLevelItemCount(); ++j) {
        QTreeWidgetItem* topItem = tree->topLevelItem(j);
        int count = topItem->childCount();
        for (int i = 0; i < count; ++i) {
            QTreeWidgetItem* item = topItem->child(i);
            if (item && item->isSelected()) {
                auto manager = item2Root[topItem];
                auto idx = item2Trajectories[item];
                manager->setSize(idx, width);
            }
        }
    }

	visualizer->requestUpdate();
}

void TrajectoriesDialog::visibilityChanged( bool visible )
{
    QCheckBox* box = qobject_cast<QCheckBox*>(sender());
    if (box) {
        QTreeWidgetItem* item = getItemWhichContains(box);
        item->setSelected(true);

        auto manager = item2Root[item->parent()];
        auto idx = item2Trajectories[item];

        //currentTrajectories->setVisible(item->text(1).toStdString(), visible);
        manager->setVisible(idx, visible);

        int count = tree->topLevelItemCount();
        for (int i = 0; i < count; ++i) {
            QTreeWidgetItem* itemToDeselect = tree->topLevelItem(i);
            if (itemToDeselect != item) {
                itemToDeselect->setSelected(false);
            }
        }

        treeItemChanged(item, nullptr);

		visualizer->requestUpdate();
    }
}


QTreeWidgetItem* getItemWhichContainsRecurr(QTreeWidget* tree, QTreeWidgetItem* item, QObject* object )
{
    int columnCount = tree->columnCount();
    for (int c = 0; c < columnCount; ++c) {
        if (tree->itemWidget(item, c) == object) {
            return item;
        }
    }

    for (int c = 0; c < item->childCount(); ++c) {
        QTreeWidgetItem* child = getItemWhichContainsRecurr(tree, item->child(c), object);
        if (child) {
            return child;
        }
    }

    return nullptr;
}

QTreeWidgetItem* TrajectoriesDialog::getItemWhichContains( QObject* object ) const
{
    int count = tree->topLevelItemCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem* item = getItemWhichContainsRecurr(tree, tree->topLevelItem(i), object);
        if (item) {
            return item;
        }
    }

    throw loglib::runtime_error("Tree item not found");
}
