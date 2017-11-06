#include "ChannelsTree.h"
#include "TimeChannelWidget.h"
#include <QtWidgets/QCheckBox>

ChannelsTree::ChannelsTree(QWidget* parent, const timeline::ModelPtr & model) : QTreeWidget(parent), model(model) {
    if(model != nullptr){
        model->attach(this);
        createTreeFromModel();
    }
};

ChannelsTree::~ChannelsTree() {};

void ChannelsTree::update(const timeline::Model * model)
{
    //takeTopLevelItem(0);
    //createTreeFromModel();
    QMetaObject::invokeMethod(this, "refreshTime", Qt::AutoConnection);
    //QWidget::update();
}

void ChannelsTree::setModel(const timeline::ModelPtr & model)
{
    if(this->model != nullptr){
        this->model->detach(this);
        takeTopLevelItem(0);
    }

    this->model = model;
    this->constModel = model;

    if(model != nullptr){
        model->attach(this);
        createTreeFromModel();
    }
}

const timeline::ModelConstPtr & ChannelsTree::getModel() const
{
    return constModel;
}

void ChannelsTree::refreshTime()
{
    timeline::Model::TChannelConstPtr mRoot = model->findChannel("./");
    double time = (mRoot->getData()->getTime()- mRoot->getData()->getGlobalOffset()) / mRoot->getData()->getLength();

    for(int i = 0; i < topLevelItemCount(); i++){
        refreshChannelTime(this, topLevelItem(i), time);
    }
}

void ChannelsTree::refreshChannelTime(QTreeWidget * tree, QTreeWidgetItem * item, double time)
{
    dynamic_cast<TimeChannelWidget*>(tree->itemWidget(item,7))->setCurrent(time);

    for(int i = 0; i < item->childCount(); i++){
        refreshChannelTime(tree, item->child(i), time);
    }
}

void ChannelsTree::createTreeFromModel()
{
    timeline::Model::TChannelConstPtr mRoot = model->findChannel("./");

    ChannelsTreeItem * root = createTreeItemFormChannel(mRoot);
    setHeaderLabels(root->getHeaders());
    addTopLevelItem(root);

    double normCurrentTime = (mRoot->getData()->getTime()- mRoot->getData()->getGlobalOffset()) / mRoot->getData()->getLength();

    initTimeChannelWidgets(root, model->getOffset(), model->getLength(), normCurrentTime);
}

ChannelsTreeItem * ChannelsTree::createTreeItemFormChannel(timeline::Model::TChannelConstPtr channel)
{
    ChannelsTreeItem * ret = new ChannelsTreeItem(channel);

    for(auto i = 0; i < channel->size(); i++){
        ret->addChild(createTreeItemFormChannel(timeline::Model::toTChannel(channel->getChild(i))));
    }

    return ret;
}

void ChannelsTree::initTimeChannelWidgets(ChannelsTreeItem * item, double refStart, double refLength, double normCurrentTime)
{
    double normStart = (item->getChannel()->getData()->getGlobalOffset() - refStart) / refLength;
    double normEnd = item->getChannel()->getData()->getLength() / refLength + normStart;
    setItemWidget(item,7, new TimeChannelWidget(0,0, normStart, normEnd, normCurrentTime));

    QCheckBox * checkBox = new QCheckBox();

    checkBox->setChecked(item->getChannel()->getData()->isActive());

    setItemWidget(item,0, checkBox);

    for(auto i = 0; i < item->childCount(); i++){
        initTimeChannelWidgets(dynamic_cast<ChannelsTreeItem*>(item->child(i)), refStart, refLength, normCurrentTime);
    }
}
