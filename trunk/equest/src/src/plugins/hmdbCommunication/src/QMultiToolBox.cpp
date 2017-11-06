#include "CommunicationPCH.h"
#include "QMultiToolBox.h"

QMultiToolBox::Page *QMultiToolBox::page(QWidget *widget) const
{
    if (!widget)
        return 0;

    for (PageList::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i)
        if ((*i).widget == widget)
            return (Page*) &(*i);
    return 0;
}

QMultiToolBox::Page *QMultiToolBox::page(int index)
{
    if (index >= 0 && index < pageList.size())
        return &pageList[index];
    return 0;
}

const QMultiToolBox::Page *QMultiToolBox::page(int index) const
{
    if (index >= 0 && index < pageList.size())
        return &pageList.at(index);
    return 0;
}

void QMultiToolBox::updateTabs()
{
    for (int index = 0; index < pageList.count(); ++index) {
        const Page &page = pageList.at(index);
        QMultiToolBoxButton *tB = page.button;
        // update indexes, since the updates are delayed, the indexes will be correct
        // when we actually paint.
        tB->setIndex(index);
        if (tB->backgroundRole() != QPalette::Window) {
            tB->setBackgroundRole(QPalette::Window);
            tB->update();
        }
    }
}

QSize QMultiToolBoxButton::sizeHint() const
{
    QSize iconSize(8, 8);
    if (!icon().isNull()) {
        int icone = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, parentWidget() /* QMultiToolBox */);
        iconSize += QSize(icone + 2, icone);
    }
    QSize textSize = fontMetrics().size(Qt::TextShowMnemonic, text()) + QSize(0, 8);

    QSize total(iconSize.width() + textSize.width(), qMax(iconSize.height(), textSize.height()));
    return total.expandedTo(QApplication::globalStrut());
}

QSize QMultiToolBoxButton::minimumSizeHint() const
{
    if (icon().isNull())
        return QSize();
    int icone = style()->pixelMetric(QStyle::PM_SmallIconSize, 0, parentWidget() /* QMultiToolBox */);
    return QSize(icone + 8, icone + 8);
}

void QMultiToolBoxButton::initStyleOption(QStyleOptionToolBox *option) const
{
    if (!option)
        return;
    option->initFrom(this);
    if (selected)
        option->state |= QStyle::State_Selected;
    if (isDown())
        option->state |= QStyle::State_Sunken;
    option->text = text();
    option->icon = icon();

    if (QStyleOptionToolBoxV2 *optionV2 = qstyleoption_cast<QStyleOptionToolBoxV2 *>(option)) {
        QMultiToolBox *toolBox = static_cast<QMultiToolBox *>(parentWidget()); // I know I'm in a multi tool box.
        int widgetCount = toolBox->count();
        if (widgetCount == 1) {
            optionV2->position = QStyleOptionToolBoxV2::OnlyOneTab;
        } else if (indexInPage == 0) {
            optionV2->position = QStyleOptionToolBoxV2::Beginning;
        } else if (indexInPage == widgetCount - 1) {
            optionV2->position = QStyleOptionToolBoxV2::End;
        } else {
            optionV2->position = QStyleOptionToolBoxV2::Middle;
        }
    }
}

void QMultiToolBoxButton::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    QString text = QAbstractButton::text();
    QPainter *p = &paint;
    QStyleOptionToolBoxV2 opt;
    initStyleOption(&opt);
    style()->drawControl(QStyle::CE_ToolBoxTab, &opt, p, parentWidget());
}

/*!
    \class QMultiToolBox

    \brief The QMultiToolBox class provides a column of tabbed widget items.


    \ingroup basicwidgets

    A toolbox is a widget that displays a column of tabs one above the
    other, with the current item displayed below the current tab.
    Every tab has an index position within the column of tabs. A tab's
    item is a QWidget.

    Each item has an itemText(), an optional itemIcon(), an optional
    itemToolTip(), and a widget(). The item's attributes can be
    changed with setItemText(), setItemIcon(), and
    setItemToolTip(). Each item can be enabled or disabled
    individually with setItemEnabled().

    Items are added using addItem(), or inserted at particular
    positions using insertItem(). The total number of items is given
    by count(). Items can be deleted with delete, or removed from the
    toolbox with removeItem(). Combining removeItem() and insertItem()
    allows you to move items to different positions.

    The index of the current item widget is returned by currentIndex(),
    and set with setCurrentIndex(). The index of a particular item can
    be found using indexOf(), and the item at a given index is returned
    by item().

    The currentChanged() signal is emitted when the current item is
    changed.

    \sa QTabWidget
*/

/*!
    \fn void QMultiToolBox::currentChanged(int index)

    This signal is emitted when the current item is changed. The new
    current item's index is passed in \a index, or -1 if there is no
    current item.
*/

/*!
    Constructs a new toolbox with the given \a parent and the flags, \a f.
*/
QMultiToolBox::QMultiToolBox(QWidget *parent, Qt::WindowFlags f)
    :  QFrame(parent, f), _currentlyExpandedItemsCount(0), lastPage(0), _lastIndex(-1),
    _layout(new QVBoxLayout())
{   
    setLayout(_layout);
    setBackgroundRole(QPalette::Button);
}

/*!
    Destroys the toolbox.
*/

QMultiToolBox::~QMultiToolBox()
{

}

/*!
    \fn int QMultiToolBox::addItem(QWidget *w, const QString &text)
    \overload

    Adds the widget \a w in a new tab at bottom of the toolbox. The
    new tab's text is set to \a text. Returns the new tab's index.
*/

/*!
    \fn int QMultiToolBox::addItem(QWidget *widget, const QIcon &iconSet,const QString &text)
    Adds the \a widget in a new tab at bottom of the toolbox. The
    new tab's text is set to \a text, and the \a iconSet is
    displayed to the left of the \a text.  Returns the new tab's index.
*/

/*!
    \fn int QMultiToolBox::insertItem(int index, QWidget *widget, const QString &text)
    \overload

    Inserts the \a widget at position \a index, or at the bottom
    of the toolbox if \a index is out of range. The new item's text is
    set to \a text. Returns the new item's index.
*/

int QMultiToolBox::currentlyExpandedItemsCount() const
{
    return _currentlyExpandedItemsCount;
}

/*!
    Inserts the \a widget at position \a index, or at the bottom
    of the toolbox if \a index is out of range. The new item's text
    is set to \a text, and the \a icon is displayed to the left of
    the \a text. Returns the new item's index.
*/

int QMultiToolBox::insertItem(int index, QWidget *widget, const QIcon &icon, const QString &text, bool expanded)
{
    if (!widget)
        return -1;

    connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(_q_widgetDestroyed(QObject*)));

    Page c;
    c.widget = widget;
    c.button = new QMultiToolBoxButton(this);
    c.button->setObjectName(QLatin1String("qt_multitoolbox_multitoolboxbutton"));
    connect(c.button, SIGNAL(clicked()), this, SLOT(_q_buttonClicked()));

    c.sv = new QScrollArea();
    c.sv->setWidget(widget);
    c.sv->setWidgetResizable(true);
    c.sv->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    c.sv->setFrameStyle(QFrame::NoFrame);

    c.setText(text);
    c.setIcon(icon);

    if (index < 0 || index >= (int)pageList.count()) {
        index = pageList.count();
        pageList.append(c);
        _layout->addWidget(c.button);
        _layout->addWidget(c.sv);
        if (index == 0)
            _lastIndex = index;
    } else {
        pageList.insert(index, c);
        relayout();
        if (lastPage) {
            QWidget *current = lastPage->widget;
            int oldindex = indexOf(current);
            if (index <= oldindex) {
                lastPage = 0; // trigger change
                _lastIndex = oldindex;
            }
        }
    }

    //--------------------------------------------------------------------------------------------
    if(expanded == false){
        c.sv->hide();
    }else{
        c.button->setSelected(true);
        setCurrentlyExpandedItemsCount(_currentlyExpandedItemsCount + 1);
    }
    //--------------------------------------------------------------------------------------------

    c.button->show();

    updateTabs();
    itemInserted(index);
    return index;
}

void QMultiToolBox::setCurrentlyExpandedItemsCount(int currentlyExpandedCount)
{
    int oldVal = _currentlyExpandedItemsCount;
    _currentlyExpandedItemsCount = currentlyExpandedCount;
    emit currentlyExpandedItemsCountChanged(oldVal, _currentlyExpandedItemsCount);
}

void QMultiToolBox::_q_buttonClicked()
{
    QMultiToolBoxButton *tb = qobject_cast<QMultiToolBoxButton*>(sender());
    QWidget* item = 0;
    for (PageList::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i)
        if ((*i).button == tb) {
            item = (*i).widget;
            break;
        }

    setItemExpanded(indexOf(item), !(tb->isSelected()));
}

/*!
    \property QMultiToolBox::count
    \brief The number of items contained in the toolbox.

    By default, this property has a value of 0.
*/

int QMultiToolBox::count() const
{
    return pageList.count();
}

void QMultiToolBox::expandAllItems(bool expanded)
{
    int oldVal = _currentlyExpandedItemsCount;
    if(expanded == true){
        if(_currentlyExpandedItemsCount == pageList.count()){
            return;
        }
    }else{
        if(_currentlyExpandedItemsCount == 0){
            return;
        }
    }

    for (PageList::Iterator i = pageList.begin(); i != pageList.end(); ++i) {
        Page *c = &*i;
        if (!c || c->button->isSelected() == expanded)
            continue;

        c->button->setSelected(expanded);

        lastPage = c;
        if(expanded == true){
            lastPage->sv->show();
            _currentlyExpandedItemsCount++;
        }else{
            lastPage->sv->hide();
            --_currentlyExpandedItemsCount;
        }

        emit expansionChanged(indexOf(c->widget), expanded);
    }

    //updateTabs();
    emit currentlyExpandedItemsCountChanged(oldVal, _currentlyExpandedItemsCount);
}

void QMultiToolBox::setItemExpanded(int index, bool expanded)
{
    int oldVal = _currentlyExpandedItemsCount;
    Page *c = page(index);
    if (!c || c->button->isSelected() == expanded)
        return;

    c->button->setSelected(expanded);

    lastPage = c;
    if(expanded == true){
        lastPage->sv->show();
        _currentlyExpandedItemsCount++;
    }else{
        lastPage->sv->hide();
        --_currentlyExpandedItemsCount;
    }

    //updateTabs();
    emit expansionChanged(index, expanded);
    emit currentlyExpandedItemsCountChanged(oldVal, _currentlyExpandedItemsCount);
}

void QMultiToolBox::relayout()
{
    delete _layout;
    _layout = new QVBoxLayout(this);
    setLayout(_layout);
    _layout->setMargin(0);
    for (PageList::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i) {
        _layout->addWidget((*i).button);
        _layout->addWidget((*i).sv);
    }
}

void QMultiToolBox::_q_widgetDestroyed(QObject *object)
{
    // no verification - vtbl corrupted already
    QWidget *p = (QWidget*)object;

    Page *c = page(p);
    if (!p || !c)
        return;

    bool wasExpanded = c->button->isSelected();

    _layout->removeWidget(c->sv);
    _layout->removeWidget(c->button);
    c->sv->deleteLater(); // page might still be a child of sv
    delete c->button;

    //bool removeCurrent = c == lastPage;
    pageList.removeAll(*c);
    
    if(lastPage == c){
        lastPage = 0;
    }
    
    if(wasExpanded == true){
        setCurrentlyExpandedItemsCount(_currentlyExpandedItemsCount - 1);
    } 
}

/*!
    Removes the item at position \a index from the toolbox. Note that
    the widget is \e not deleted.
*/

void QMultiToolBox::removeItem(int index)
{
    if (QWidget *w = widget(index)) {
        disconnect(w, SIGNAL(destroyed(QObject*)), this, SLOT(_q_widgetDestroyed(QObject*)));
        w->setParent(this);
        // destroy internal data
        _q_widgetDestroyed(w);
        itemRemoved(index);
    }
}


/*!
    \property QMultiToolBox::currentIndex
    \brief the index of the current item

    By default, for an empty toolbox, this property has a value of -1.

    \sa indexOf(), widget()
*/


int QMultiToolBox::lastIndex() const
{
    return lastPage ? indexOf(lastPage->widget) : -1;
}

/*!
    Returns a pointer to the current widget, or 0 if there is no such item.

    \sa currentIndex(), setCurrentWidget()
*/

QWidget * QMultiToolBox::lastWidget() const
{
    return lastPage ? lastPage->widget : 0;
}

/*!
  Makes\a widget the current widget. The \a widget must be an item in this tool box.

  \sa addItem(), setCurrentIndex(), currentWidget()
 */
//void QMultiToolBox::setCurrentWidget(QWidget *widget)
//{
//    int i = indexOf(widget);
//    if (i >= 0)
//        setCurrentIndex(i);
//    else
//        qWarning("QMultiToolBox::setCurrentWidget: widget not contained in tool box");
//}

void QMultiToolBox::setItemExpanded(QWidget *widget, bool expanded)
{
    int i = indexOf(widget);
    if (i >= 0)
        setItemExpanded(i, expanded);
    else
        qWarning("QMultiToolBox::setItemExpanded: widget not contained in tool box");
}

/*!
    Returns the widget at position \a index, or 0 if there is no such
    item.
*/

QWidget *QMultiToolBox::widget(int index) const
{
    if (index < 0 || index >= (int) pageList.size())
        return 0;
    return pageList.at(index).widget;
}

/*!
    Returns the index of \a widget, or -1 if the item does not
    exist.
*/

int QMultiToolBox::indexOf(QWidget *widget) const
{
    
    Page *c = (widget ? page(widget) : 0);
    return c ? pageList.indexOf(*c) : -1;
}

/*!
    If \a enabled is true then the item at position \a index is enabled; otherwise
    the item at position \a index is disabled.
*/

void QMultiToolBox::setItemEnabled(int index, bool enabled)
{
    Page *c = page(index);
    if (!c)
        return;

    c->button->setEnabled(enabled);
}


/*!
    Sets the text of the item at position \a index to \a text.

    If the provided text contains an ampersand character ('&'), a
    mnemonic is automatically created for it. The character that
    follows the '&' will be used as the shortcut key. Any previous
    mnemonic will be overwritten, or cleared if no mnemonic is defined
    by the text. See the \l {QShortcut#mnemonic}{QShortcut}
    documentation for details (to display an actual ampersand, use
    '&&').
*/

void QMultiToolBox::setItemText(int index, const QString &text)
{
    Page *c = page(index);
    if (c)
        c->setText(text);
}

/*!
    Sets the icon of the item at position \a index to \a icon.
*/

void QMultiToolBox::setItemIcon(int index, const QIcon &icon)
{
    Page *c = page(index);
    if (c)
        c->setIcon(icon);
}

/*!
    Sets the tooltip of the item at position \a index to \a toolTip.
*/

void QMultiToolBox::setItemToolTip(int index, const QString &toolTip)
{
    Page *c = page(index);
    if (c)
        c->setToolTip(toolTip);
}

/*!
    Returns true if the item at position \a index is enabled; otherwise returns false.
*/

bool QMultiToolBox::isItemEnabled(int index) const
{
    const Page *c = page(index);
    return c && c->button->isEnabled();
}

/*!
    Returns the text of the item at position \a index, or an empty string if
    \a index is out of range.
*/

QString QMultiToolBox::itemText(int index) const
{
    const Page *c = page(index);
    return (c ? c->text() : QString());
}

/*!
    Returns the icon of the item at position \a index, or a null
    icon if \a index is out of range.
*/

QIcon QMultiToolBox::itemIcon(int index) const
{
    const Page *c = page(index);
    return (c ? c->icon() : QIcon());
}

/*!
    Returns the tooltip of the item at position \a index, or an
    empty string if \a index is out of range.
*/

QString QMultiToolBox::itemToolTip(int index) const
{
    const Page *c = page(index);
    return (c ? c->toolTip() : QString());
}

/*! \reimp */
void QMultiToolBox::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
}

/*! \reimp */
void QMultiToolBox::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::StyleChange)
        updateTabs();
    QFrame::changeEvent(ev);
}

/*!
  This virtual handler is called after a new item was added or
  inserted at position \a index.

  \sa itemRemoved()
 */
void QMultiToolBox::itemInserted(int index)
{
    Q_UNUSED(index)
}

/*!
  This virtual handler is called after an item was removed from
  position \a index.

  \sa itemInserted()
 */
void QMultiToolBox::itemRemoved(int index)
{
    Q_UNUSED(index)
}

/*!
    \fn void QMultiToolBox::setItemLabel(int index, const QString &text)

    Use setItemText() instead.
*/

/*!
    \fn QString QMultiToolBox::itemLabel(int index) const

    Use itemText() instead.
*/

/*!
    \fn QWidget *QMultiToolBox::currentItem() const

    Use widget(currentIndex()) instead.
*/

/*!
    \fn void QMultiToolBox::setCurrentItem(QWidget *widget)

    Use setCurrentIndex(indexOf(widget)) instead.
*/

/*!
    \fn void QMultiToolBox::setItemIconSet(int index, const QIcon &icon)

    Use setItemIcon() instead.
*/

/*!
    \fn QIcon QMultiToolBox::itemIconSet(int index) const

    Use itemIcon() instead.
*/

/*!
    \fn int QMultiToolBox::removeItem(QWidget *widget)

    Use toolbox->removeItem(toolbox->indexOf(widget)) instead.
*/

/*!
    \fn QWidget *QMultiToolBox::item(int index) const

    Use widget() instead.
*/

/*!
    \fn void QMultiToolBox::setMargin(int margin)
    Sets the width of the margin around the contents of the widget to \a margin.

    Use QWidget::setContentsMargins() instead.
    \sa margin(), QWidget::setContentsMargins()
*/

/*!
    \fn int QMultiToolBox::margin() const
    Returns the width of the margin around the contents of the widget.

    Use QWidget::getContentsMargins() instead.
    \sa setMargin(), QWidget::getContentsMargins()
*/

/*! \reimp */
bool QMultiToolBox::event(QEvent *e)
{
    return QFrame::event(e);
}

// IMPLEMENTATION part END
