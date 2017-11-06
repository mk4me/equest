#include "SummaryWindow.h"
#include "AnalisisWidget.h"
#include <plugins/hmdbCommunication/IPatient.h>

void SummaryWindow::initialize()
{
    placeholder->setLayout(new QVBoxLayout());
    placeholder->layout()->setContentsMargins(0, 0, 0, 0);
    placeholder->layout()->setMargin(0);
    tree = new QTreeWidget();
    tree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    tree->setHeaderHidden(true);
    placeholder->layout()->addWidget(tree);
    QTreeWidgetItem* twi = new QTreeWidgetItem();
    tree->addTopLevelItem(twi);
}

void SummaryWindow::display( const core::IHierarchyItemConstWeakPtr& helper )
{
    HelpersCollection hc;
    hc.push_back(helper);
    display(hc);
}

void SummaryWindow::display( const HelpersCollection& helpers )
{
    UTILS_ASSERT(tree != nullptr);

    tree->clear();
    auto size = helpers.size();
    if (size > 0) {
        QTreeWidgetItem* root = new QTreeWidgetItem();
        root->setText(0, QObject::tr("Data Info"));
        QFont font = root->font(0);
        font.setBold(true);
        font.setPointSize(12);
        root->setFont(0, font);
        tree->addTopLevelItem(root);

        for (auto itHelper = helpers.begin(); itHelper != helpers.end(); ++itHelper) {
            
            // TODO
            /*NewMultiserieHelperPtr multiserieHelper = utils::dynamic_pointer_cast<NewMultiserieHelper>(*itHelper);
            if (multiserieHelper) {
                const auto& desc = multiserieHelper->getChartWithDescriptions();
                for (auto it = desc.begin(); it != desc.end(); ++it) {
                    auto motion = it->motion;
                    if (motion) {
						std::string name;
						it->wrapper->tryGetMeta("core/name", name);
                        QString text = QString::fromStdString(name) + createDescription(it->motion);
                        addItem(text, root);
                    }
                }
            } else {*/
                // TODO obsluga motion
                //auto motion = (*itHelper)->getMotion();
                //QString text = motion ? createDescription(motion) : QString();
                
                core::IHierarchyItemConstPtr hdi = itHelper->lock();
                QString text = hdi->getName() + "\n";
                if (hdi) {
                    text += hdi->getDescription();
                }
                addItem(text, root);
            //}
        }

        root->setExpanded(true);
    }
}

void SummaryWindow::addItem( QString text, QTreeWidgetItem* root )
{
    QStringList splits = text.split("\n", QString::SkipEmptyParts);
    for (auto it = splits.begin(); it != splits.end(); ++it) {
        QTreeWidgetItem* itm = new QTreeWidgetItem();
        itm->setText(0, *it);
        root->addChild(itm);
    }
}

void SummaryWindow::clear()
{
    tree->clear();
}


SummaryWindowController::SummaryWindowController( SummaryWindowPtr sw, AnalisisModelPtr analysisModel) :
	summary(sw),
    analysisModel(analysisModel)
{

}
    
void SummaryWindowController::onVisualizer( coreUI::CoreVisualizerWidget* visualizatorWidget )
{
    // TODO, tymczasowo wylaczone (jest problem przy ponownym otwieraniu wizualizatora)
    return;
    core::VisualizerPtr visualizer = visualizatorWidget->getVisualizer();
    
    SummaryWindow::HelpersCollection collection;
    auto desc = analysisModel->getVisualizerDataDescription(visualizatorWidget->getVisualizer());\
    core::HierarchyHelperPtr helper  = analysisModel->getHelper(desc);
    if (helper && helper->getParent().lock()) {
        collection.push_back(helper->getParent());
        summary->display(collection);
    }
}
