#include "PCH.h"
#include "SchemeDialog.h"
#include "KinematicVisualizer.h"
#include <osgutils/OsgSchemeDrawer.h>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include "AbstractSkeletonSerie.h"

SchemeDialog::SchemeDialog(QWidget* parent, KinematicVisualizer * visualizer) :
QDialog(parent), visualizer(visualizer),
autoHideSegments(new QCheckBox(tr("Auto-hide connected segments"), this)),
pointsTree(nullptr), connectionsTree(nullptr),
pointsOrientationsDrawer(nullptr)
{
	auto ml = new QVBoxLayout;

	auto tabWidget = new QTabWidget(this);

	ml->addWidget(tabWidget);

	auto pointsWidget = new QWidget(tabWidget);

	auto pl = new QVBoxLayout;

	pointsTree = new QTreeWidget(pointsWidget);

	pl->addWidget(pointsTree);
	pl->addWidget(autoHideSegments);

	pointsWidget->setLayout(pl);

	tabWidget->addTab(pointsWidget, tr("Points"));

	auto connectionsWidget = new QWidget(tabWidget);

	auto cl = new QVBoxLayout;

	connectionsTree = new QTreeWidget(connectionsWidget);

	cl->addWidget(connectionsTree);

	connectionsWidget->setLayout(cl);

	tabWidget->addTab(connectionsWidget, tr("Connections"));

	setLayout(ml);


	autoHideSegments->setChecked(true);
    setWindowTitle(tr("Drawing schemes"));
	QIcon icon( QString::fromUtf8(":/kinematic/icons/trajectory.png") );
	QTreeWidgetItem* pitem = pointsTree->headerItem();
	pointsTree->setColumnCount(3);
	pitem->setIcon(0, icon);
	pitem->setText(0, "");
    pitem->setText(2, tr("Color"));	

	QTreeWidgetItem* citem = connectionsTree->headerItem();
	connectionsTree->setColumnCount(4);
	citem->setIcon(0, icon);
	citem->setText(0, "");
	citem->setText(1, tr("Parent"));
	citem->setText(2, tr("Child"));
	citem->setText(3, tr("Color"));

	connect(pointsTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(pointItemChanged(QTreeWidgetItem*, int)));
	connect(connectionsTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(connectionItemChanged(QTreeWidgetItem*, int)));
}

void SchemeDialog::setDrawer(osgutils::IBaseDrawerSchemePtr drawer, const QString& rootName, const QStringList& names, const osgutils::IConnectionDrawerWithDescriptors& connections,
	PointsOrientationsDrawer * pointsOrientationsDrawer)
{
	this->pointsOrientationsDrawer = pointsOrientationsDrawer;
    QTreeWidgetItem* root = new QTreeWidgetItem();
    root->setText(1, rootName);
    pointsTree->addTopLevelItem(root);

    //QPushButton* globalColorButton = new QPushButton();
    //globalColorButton->setFixedSize(16, 16);
    //connect(globalColorButton, SIGNAL(clicked(bool)), this, SLOT(globalColorClicked()));
    //tree->setItemWidget(root, 2, globalColorButton);
    //QCheckBox* rootCheck = new QCheckBox(tree);
    //rootCheck->setChecked(true);
    //tree->setItemWidget(root, 0, rootCheck);
    //connect(rootCheck, SIGNAL(clicked(bool)), this, SLOT(rootVisibilityChanged(bool)));

    drawer2Connections[drawer] = connections;

    int count = names.size();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(0, drawer->visible(i) ? Qt::Checked : Qt::Unchecked);
        item->setText(1, names[i]);
        root->addChild(item);        
        pointItem2Drawer[item] = std::make_pair(drawer, i);
    }

	root = new QTreeWidgetItem();
	root->setText(1, tr("Connections"));
	connectionsTree->addTopLevelItem(root);
	
	for (int i = 0; i < connections.second.size(); ++i) {
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(0, connections.first->visible(i) ? Qt::Checked : Qt::Unchecked);
		item->setText(1, names[connections.second[i].range.first]);
		item->setText(2, names[connections.second[i].range.second]);
		root->addChild(item);
		connectionItem2Drawer[item] = std::make_pair(connections.first, i);
	}
}
    
QColor SchemeDialog::transformColor( const osg::Vec4& color ) const
{
	return QColor(
		static_cast<int>(255 * color[0]),
		static_cast<int>(255 * color[1]),
		static_cast<int>(255 * color[2]),
		static_cast<int>(255 * color[3])
		);
}

osg::Vec4 SchemeDialog::transformColor( const QColor& color ) const
{
	return osg::Vec4 (
		static_cast<float>(color.red() / 255.0f),
		static_cast<float>(color.green() / 255.0f),
		static_cast<float>(color.blue() / 255.0f),
		static_cast<float>(color.alpha() / 255.0f)
		);

}

QTreeWidgetItem* _getItemWhichContainsRecurr(QTreeWidget* tree, QTreeWidgetItem* item, QObject* object )
{
    int columnCount = tree->columnCount();
    for (int c = 0; c < columnCount; ++c) {
        if (tree->itemWidget(item, c) == object) {
            return item;
        }
    }

    for (int c = 0; c < item->childCount(); ++c) {
        QTreeWidgetItem* child = _getItemWhichContainsRecurr(tree, item->child(c), object);
        if (child) {
            return child;
        }
    }

    return nullptr;
}

QTreeWidgetItem* SchemeDialog::getItemWhichContains( QObject* object ) const
{
	int count = pointsTree->topLevelItemCount();
	for (int i = 0; i < count; ++i) {
		QTreeWidgetItem* item = _getItemWhichContainsRecurr(pointsTree, pointsTree->topLevelItem(i), object);
        if (item) {
            return item;
        }
		
	}
	throw loglib::runtime_error("Tree item not found");
}

void SchemeDialog::setButtonColor( QPushButton* button, const QColor& color )
{
	//QString style = QString("QPushButton { background-color: rgb( %1, %2, %3); }").arg(color.red()).arg(color.green()).arg(color.blue());
	//button->setStyleSheet(style);
}

void SchemeDialog::visibilityChanged( bool visible )
{
	QCheckBox* box = qobject_cast<QCheckBox*>(sender());
	if (box) {
		QTreeWidgetItem* current = getItemWhichContains(box);
        //std::string name = current->text(1).toStdString();
        auto it = pointItem2Drawer.find(current);
        if (it != pointItem2Drawer.end()) {
			const bool visible = box->isChecked();
            auto t = it->second;
			osgutils::IBaseDrawerSchemePtr drawer = t.first;
            auto idx = t.second;
			drawer->setVisible(idx, visible);
			if (pointsOrientationsDrawer != nullptr){
				pointsOrientationsDrawer->setVisible(visible, idx);
			}

			if (autoHideSegments->isChecked() == true){

				connectionsTree->blockSignals(true);
				auto root = connectionsTree->topLevelItem(0);
				osgutils::SegmentsDescriptors descriptions = drawer2Connections[drawer].second;
				osgutils::IConnectionsSchemeDrawerPtr connections = drawer2Connections[drawer].first;
				int count = descriptions.size();
				for (int i = 0; i < count; ++i) {
					auto a = descriptions[i].range.first;
					auto b = descriptions[i].range.second;
					if (a == idx || b == idx) {
						connections->setVisible(i, visible);
						root->child(i)->setCheckState(0, visible ? Qt::Checked : Qt::Unchecked);
					}
				}

				connectionsTree->blockSignals(false);
			}

			visualizer->requestUpdate();
        }
	}
}

void SchemeDialog::blockAllSignals( bool val )
{
	this->blockSignals(val);
	/*startSlider->blockSignals(val);
	startTimeSpin->blockSignals(val);
	endTimeSpin->blockSignals(val);
	endSlider->blockSignals(val);
	thicknessSpin->blockSignals(val);*/
	
}

//void SchemeDialog::rootVisibilityChanged( bool visible )
//{
//    QCheckBox* box = qobject_cast<QCheckBox*>(sender());
//    if (box) {
//        QTreeWidgetItem* item = getItemWhichContains(box);
//        item2Root[item]->setVisible(visible);
//    }
//}

//void SchemeDialog::globalColorClicked()
//{
//    QPushButton* button = qobject_cast<QPushButton*>(sender());
//    if (button) {
//        QTreeWidgetItem* item = getItemWhichContains(button);
//        //VisualizationSchemePtr scheme = utils::const_pointer_cast<VisualizationScheme>(item2Root[item]->getVisualiztionScheme());
//        //if (scheme) {
//        //    QColor color(255, 255, 255, 80);
//        //    color = QColorDialog::getColor ( color, this, tr("Choose color"), QColorDialog::ShowAlphaChannel);
//        //    scheme->setGlobalStatesColor(transformColor(color));
//        //    item2Root[item]->update();
//        //}
//    }
//}

void SchemeDialog::pointItemChanged(QTreeWidgetItem * item, int column)
{
	if (column == 0){

		osgutils::IBaseDrawerSchemePtr drawer = pointItem2Drawer[item].first;
		auto idx = pointItem2Drawer[item].second;
		const bool visible = item->checkState(0) == Qt::Checked ? true : false;
		drawer->setVisible(idx, visible);
		if (pointsOrientationsDrawer != nullptr){
			pointsOrientationsDrawer->setVisible(visible, idx);
		}

		if (autoHideSegments->isChecked() == true){
			connectionsTree->blockSignals(true);
			auto root = connectionsTree->topLevelItem(0);
			osgutils::SegmentsDescriptors descriptions = drawer2Connections[drawer].second;
			osgutils::IConnectionsSchemeDrawerPtr connections = drawer2Connections[drawer].first;
			int count = descriptions.size();
			for (int i = 0; i < count; ++i) {
				auto a = descriptions[i].range.first;
				auto b = descriptions[i].range.second;
				if (a == idx || b == idx) {
					connections->setVisible(i, visible);
					root->child(i)->setCheckState(0, visible ? Qt::Checked : Qt::Unchecked);
				}
			}
			connectionsTree->blockSignals(false);
		}

		visualizer->requestUpdate();
	}
}

void SchemeDialog::connectionItemChanged(QTreeWidgetItem * item, int column)
{
	if (column == 0){
		connectionItem2Drawer[item].first->setVisible(connectionItem2Drawer[item].second, item->checkState(0) == Qt::Checked ? true : false);
		visualizer->requestUpdate();
	}
}