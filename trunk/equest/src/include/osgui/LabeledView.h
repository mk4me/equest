/********************************************************************
	created:  2010/11/17
	created:  17:11:2010   17:16
	filename: LabeledView.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD__LABELEDVIEW_H__
#define HEADER_GUARD__LABELEDVIEW_H__

#include <osgWidget/Window>
#include <osgWidget/Label>
#include <osg/ref_ptr>
#include <osgui/Borderized.h>
#include <osgui/Buttonized.h>
#include <osgui/macroHacks.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

// TODO: zamienić na bez szablonu

template <class Overlay = Buttonized< Borderized<osgWidget::Widget>, BorderStylePolicy<DefaultStylePolicy> > >
class LabeledView : public osgWidget::Window
{
private:
    osg::ref_ptr<Overlay> overlay;
    osg::ref_ptr<osgWidget::Label> label;

public:

    //META_Object(osgui, LabeledView);
	META_Window(osgui, LabeledView);

    LabeledView(const std::string& name = "")  :
    osgWidget::Window(name)
    {
        overlay = new Overlay("overlay");
        overlay->setBorderWidth(1);
        overlay->setHoverColor( osgWidget::Color(1,1,1,0.5) );
        overlay->setBorderAllColors( osgWidget::Color(1,1,1,1) );
        overlay->setBorderToggleColor( osgWidget::Color(1,0,0,1) );
        overlay->setToggleEnabled(true);
        label = new osgWidget::Label("label", name);
        label->setLayer(osgWidget::Widget::LAYER_HIGH);
        addWidget(overlay.get());
        addWidget(label.get());
    }

    LabeledView(const LabeledView& view, const osg::CopyOp& copyop) :
    osgWidget::Window(view, copyop)
    {
        overlay = dynamic_cast<Overlay*>(getByName("overlay"));
        if ( !overlay ) {
            overlay = dynamic_cast<Overlay*>(copyop(overlay));
        }
        label = dynamic_cast<osgWidget::Label*>(getByName("label"));
        if ( !label ) {
            label = dynamic_cast<osgWidget::Label*>(copyop(label));
        }
    }

    //! \return
    Overlay* getOverlay()
    {
        return overlay.get();
    }
    //! \return
    const Overlay* getOverlay() const
    {
        return overlay.get();
    }
    //!
    void setOverlay(Overlay* overlay)
    {
        if ( this->overlay ) {
            removeWidget(this->overlay);
        }
        this->overlay = overlay;
        addWidget(overlay);
    }
    //!
    osgWidget::Label* getLabel()
    {
        return label.get();
    }
    //!
    const osgWidget::Label* getLabel() const
    {
        return label.get();
    }

    bool isLabelVisible() const
    {
        return label->getParent() == this;
    }

    void setLabelVisible(bool visible)
    {
        if ( isLabelVisible() ) {
            if ( !visible ) {
                removeWidget(label);
            }
        } else if ( visible ) {
            addWidget(label);
        }
    }

protected:
    //! Przeładowanie metody rozmieszczającej elementy.
    //! \param diffWidth
    //! \param diffHeight
    virtual void _resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight )
    {
        // aktualizacja rozmiaru prostokata z obrazkiem
        float height = getHeight() + diffHeight;
        float width = getWidth() + diffWidth;

        overlay->setDimensions(0, 0, width, height);
        //overlay->setOrigin(0, 0);
        //_positionWidget(overlay, width, height);

        float margin = overlay->getBorderWidth();
        label->setSize( width - 2*margin, label->getHeight() );
        label->setOrigin( margin, height - label->getHeight() - margin);
    }

    //! Przeładowanie metody obliczającej rozmiar.
    virtual osgWidget::Window::Sizes _getWidthImplementation() const
    {
        osgWidget::Window::Sizes result;
        result.minimum = 2*getOverlay()->getBorderWidth();
        result.current = result.minimum > getOverlay()->getWidth() ? result.minimum : getOverlay()->getWidth();
        return result;
    }
    //! Przeładowanie metody obliczającej rozmiar.
    virtual osgWidget::Window::Sizes _getHeightImplementation() const
    {
        osgWidget::Window::Sizes result;
        result.minimum = 2*getOverlay()->getBorderWidth() /*+ (isLabelVisible() ? getLabel()->getHeight() : 0.0f )*/;
        result.current = result.minimum > getOverlay()->getHeight() ? result.minimum : getOverlay()->getHeight();
        return result;
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////


#endif  // HEADER_GUARD__LABELEDVIEW_H__
