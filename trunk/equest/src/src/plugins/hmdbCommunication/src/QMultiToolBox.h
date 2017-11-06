/********************************************************************
    created:  2011/12/20
    created:  20:12:2011   1:11
    filename: QMultiToolBox.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_QT_EXT__QMULTITOOLBOX_H__
#define HEADER_GUARD_QT_EXT__QMULTITOOLBOX_H__

#include <QtWidgets/QFrame>
#include <QtGui/QIcon>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStyleOption>
#include <QtCore/QList>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QPainter>
#include <QtWidgets/QStyle>
#include <QtWidgets/QToolTip>
#include <QtWidgets/QApplication>

class QMultiToolBoxButton : public QAbstractButton
{
    Q_OBJECT
public:
    QMultiToolBoxButton(QWidget *parent)
        : QAbstractButton(parent), selected(false), indexInPage(-1)
    {
        setBackgroundRole(QPalette::Window);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        setFocusPolicy(Qt::NoFocus);
    }

    inline void setSelected(bool b) { selected = b; update(); }
    inline bool isSelected() const { return selected; }
    inline void setIndex(int newIndex) { indexInPage = newIndex; }

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

protected:
    void initStyleOption(QStyleOptionToolBox *opt) const;
    void paintEvent(QPaintEvent *);

private:
    bool selected;
    int indexInPage;
};

class QMultiToolBox : public QFrame
{
    Q_OBJECT
        Q_PROPERTY(int currentlyExpandedItemsCount READ currentlyExpandedItemsCount NOTIFY currentlyExpandedItemsCountChanged)
        Q_PROPERTY(int count READ count)

private:

    struct Page
    {
        QMultiToolBoxButton *button;
        QScrollArea *sv;
        QWidget *widget;

        inline void setText(const QString &text) { button->setText(text); }
        inline void setIcon(const QIcon &is) { button->setIcon(is); }

        inline void setToolTip(const QString &tip) { button->setToolTip(tip); }
        inline QString toolTip() const { return button->toolTip(); }

        inline QString text() const { return button->text(); }
        inline QIcon icon() const { return button->icon(); }

        inline bool operator==(const Page& other) const
        {
            return widget == other.widget;
        }
    };

    typedef QList<Page> PageList;

public:
    explicit QMultiToolBox(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~QMultiToolBox();

    int addItem(QWidget *widget, const QString &text, bool expanded = false);
    int addItem(QWidget *widget, const QIcon &icon, const QString &text, bool expanded = false);
    int insertItem(int index, QWidget *widget, const QString &text, bool expanded = false);
    int insertItem(int index, QWidget *widget, const QIcon &icon, const QString &text, bool expanded = false);

    void removeItem(int index);

    int currentlyExpandedItemsCount() const;
    
    bool isItemExpanded(int index) const;

    void setItemEnabled(int index, bool enabled);
    bool isItemEnabled(int index) const;

    void setItemText(int index, const QString &text);
    QString itemText(int index) const;

    void setItemIcon(int index, const QIcon &icon);
    QIcon itemIcon(int index) const;

    void setItemToolTip(int index, const QString &toolTip);
    QString itemToolTip(int index) const;

    int lastIndex() const;
    QWidget *lastWidget() const;
    QWidget *widget(int index) const;
    int indexOf(QWidget *widget) const;
    int count() const;

    public slots:
        void expandAllItems(bool expanded = true);
        void setItemExpanded(int index, bool expanded = true);
        void setItemExpanded(QWidget *widget, bool expanded = true);

    private slots:
        void _q_widgetDestroyed(QObject*);
        void _q_buttonClicked();

    signals:
        void expansionChanged(int index, bool expanded);
        void currentlyExpandedItemsCountChanged(int prevExpandedItemsCount, int currentExpandedItemsCount);

protected:
    bool event(QEvent *e);
    virtual void itemInserted(int index);
    virtual void itemRemoved(int index);
    void showEvent(QShowEvent *e);
    void changeEvent(QEvent *);

private:
    void setCurrentlyExpandedItemsCount(int currentlyExpandedCount);

    Page *page(QWidget *widget) const;
    const Page *page(int index) const;
    Page *page(int index);

    void updateTabs();
    void relayout();

private:
    int _lastIndex;
    int _currentlyExpandedItemsCount;
    PageList pageList;
    QVBoxLayout *_layout;
    Page *lastPage;
};

inline int QMultiToolBox::addItem(QWidget *item, const QString &text, bool expanded)
{ return insertItem(-1, item, QIcon(), text, expanded); }
inline int QMultiToolBox::addItem(QWidget *item, const QIcon &iconSet,
    const QString &text, bool expanded)
{ return insertItem(-1, item, iconSet, text, expanded); }
inline int QMultiToolBox::insertItem(int index, QWidget *item, const QString &text, bool expanded)
{ return insertItem(index, item, QIcon(), text, expanded); }

#endif HEADER_GUARD_QT_EXT__QMULTITOOLBOX_H__
