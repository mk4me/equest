#include "TimelineWidget.h"
#include <QtWidgets/QCheckBox>
#include <QtGui/QResizeEvent>
#include <QtGui/QPainter>

#define VISUAL_CHANNEL_COLUMN_HEADER 7

TimelineWidget::TimelineWidget(const timeline::ModelPtr & model) : model(model), isPlaying(false), overlay(new QWidget(this)),
    visualChannelsColumnHeader(new ChannelsTreeHeaderWidget())
{
    UTILS_ASSERT((model != nullptr), "Zly model dla timeline");
    setupUi(this);

    overlay->setPalette(Qt::transparent);
    overlay->setAttribute(Qt::WA_TransparentForMouseEvents);

    model->attach(this);

    timeCounter.setSlider(manualTime);

    createTreeFromModel();
}

TimelineWidget::~TimelineWidget()
{
    if(timeCounter.isRunning() == true){
        timeCounter.quit();
        while(timeCounter.isRunning() == true) { OpenThreads::Thread::microSleep(5000); }
    }
}

void TimelineWidget::setModel(const timeline::ModelPtr & model)
{
    UTILS_ASSERT((model != nullptr), "Zly model dla timeline");
    
    this->model->detach(this);
    channelsTree->takeTopLevelItem(0);

    this->model = model;
    this->constModel = model;

    model->attach(this);
    createTreeFromModel();
}

const timeline::ModelPtr & TimelineWidget::getModel()
{
    return model;
}

const timeline::ModelConstPtr & TimelineWidget::getModel() const
{
    return constModel;
}

void TimelineWidget::update(const timeline::Model * model)
{
    QMetaObject::invokeMethod(this, "refreshTime", Qt::AutoConnection);
}

void TimelineWidget::toBegin()
{
    manualTime->setValue(manualTime->minimum());
}

void TimelineWidget::toEnd()
{
    manualTime->setValue(manualTime->maximum());
}

void TimelineWidget::resizeEvent(QResizeEvent * qResizeEvent)
{
    overlay->resize(qResizeEvent->size());
    updateTimeCursor();
    qResizeEvent->accept();

    //TODO
    //odrysowac wskaznik czasu na osi
}

void TimelineWidget::updateTimeCursor()
{    
    int height = getChannelsHeight(channelsTree->topLevelItem(0));
    int x = getCursorXMin() + (getCursorXMax() - getCursorXMin()) * model->getNormalizedTime();

    QRect cursorRect(x, getCursorTop(), 1, height);

    QPainter painter(overlay.data());
    painter.setRenderHints(QPainter::TextAntialiasing | QPainter::HighQualityAntialiasing);

    QPen pen(Qt::black,0);
    painter.setPen(pen);
    painter.eraseRect(overlay->rect());
    painter.fillRect(cursorRect,QColor(255,255,255,255));
}

int TimelineWidget::getChannelsHeight(QTreeWidgetItem * item) const
{
    int ret = channelsTree->visualItemRect(item).height();

    if(item->isExpanded() == true){
        for(int i = 0; i < item->childCount(); i++){
            ret += getChannelsHeight(item->child(i));
        }
    }

    return ret;
}

int TimelineWidget::getCursorTop() const
{
    return visualChannelsColumnHeader->rect().bottom();
}

int TimelineWidget::getCursorXMin() const
{
    return visualChannelsColumnHeader->rect().left();
}

int TimelineWidget::getCursorXMax() const
{
    return visualChannelsColumnHeader->rect().right();
}

void TimelineWidget::play()
{
    if(isPlaying == false){
        isPlaying = true;
        playPlayer->setText("Pause");
        if(timeCounter.isSuspended() == true){
            timeCounter.resume();
        }else{
            timeCounter.start();
        }
    }else{
        isPlaying = false;
        playPlayer->setText("Play");
        timeCounter.pause();
    }
}

void TimelineWidget::setTime(int time)
{
    double normalizedTime = time;
    normalizedTime /= manualTime->maximum() - manualTime->minimum();

    model->setTime(normalizedTime * model->getLength());
}

void TimelineWidget::refreshTime()
{
    timeline::Model::TChannelConstPtr mRoot = model->findChannel("./");
    double time = (mRoot->getData()->getTime()- mRoot->getData()->getGlobalOffset()) / mRoot->getData()->getLength();

    for(int i = 0; i < channelsTree->topLevelItemCount(); i++){
        refreshChannelTime(channelsTree, channelsTree->topLevelItem(i), time);
    }
}

void TimelineWidget::refreshChannelTime(QTreeWidget * tree, QTreeWidgetItem * item, double time)
{
    dynamic_cast<TimeChannelWidget*>(tree->itemWidget(item,7))->setCurrent(time);

    for(int i = 0; i < item->childCount(); i++){
        refreshChannelTime(tree, item->child(i), time);
    }
}

void TimelineWidget::createTreeFromModel()
{
    timeline::Model::TChannelConstPtr mRoot = model->findChannel("./");

    ChannelsTreeItem * root = createTreeItemFormChannel(mRoot);
    channelsTree->setHeaderLabels(root->getHeaders());

    //modyfikujemy header naszego drzewa
    QTreeWidgetItem * header = channelsTree->headerItem();

    
    //channelsTree->header()->setIndexWidget(channelsTree->header()->logicalIndex(7), new ChannelsTreeHeaderWidget());
    /*QTreeWidgetItem * header = new QTreeWidgetItem();

    header->setText(0, "Active");
    header->setText(0, "Channel");
    header->setText(0, "Length");
    header->setText(0, "Global Time Scale");
    header->setText(0, "Local Time Scale");
    header->setText(0, "Global Offset");
    header->setText(0, "Local Offset");*/

    //channelsTree->setItemWidget(header, root->getChannelsColumnIndex(), visualChannelsColumnHeader.data());
    channelsTree->setItemWidget(header, 7, new ChannelsTreeHeaderWidget());

    //TODO
    //rysuj os czasu na visualChannelsColumnHeader


    channelsTree->addTopLevelItem(root);

    double normCurrentTime = (mRoot->getData()->getTime() - mRoot->getData()->getGlobalOffset()) / mRoot->getData()->getLength();

    initTimeChannelWidgets(root, model->getOffset(), model->getLength(), normCurrentTime);
}

ChannelsTreeItem * TimelineWidget::createTreeItemFormChannel(timeline::Model::TChannelConstPtr channel)
{
    ChannelsTreeItem * ret = new ChannelsTreeItem(channel);
    
    QFont font;
    font = ret->font(1);
    font.setBold(true);
    ret->setFont(1, font);

    for(auto i = 0; i < channel->size(); i++){
        ChannelsTreeItem * item = createTreeItemFormChannel(timeline::Model::toTChannel(channel->getChild(i)));
        
        ret->addChild(item);

        font = item->font(1);
        font.setBold(false);
        item->setFont(1, font);

    }

    return ret;
}

void TimelineWidget::initTimeChannelWidgets(ChannelsTreeItem * item, double refStart, double refLength, double normCurrentTime)
{
    double normStart = (item->getChannel()->getData()->getGlobalOffset() - refStart) / refLength;
    double normEnd = item->getChannel()->getData()->getLength() / refLength + normStart;
    channelsTree->setItemWidget(item,7, new TimeChannelWidget(0,0, normStart, normEnd, normCurrentTime));

    QCheckBox * checkBox = new QCheckBox();

    checkBox->setChecked(item->getChannel()->getData()->isActive());

    channelsTree->setItemWidget(item,0, checkBox);

    for(auto i = 0; i < item->childCount(); i++){
        initTimeChannelWidgets(dynamic_cast<ChannelsTreeItem*>(item->child(i)), refStart, refLength, normCurrentTime);
    }
}
