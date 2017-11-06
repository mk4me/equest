/********************************************************************
	created:  2010/06/10
	created:  10:6:2010   10:36
	filename: Button.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef HEADER_GUARD__BUTTON_H__
#define HEADER_GUARD__BUTTON_H__

#include <osgWidget/Widget>
#include <osgWidget/Label>
#include <type_traits>
#include <utils/Debug.h>
#include <osgui/macroHacks.h>
#include <osgWidget/WindowManager>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

class DefaultStylePolicy;

template <class Base, class StylePolicy = DefaultStylePolicy>
class Buttonized : public Base, public StylePolicy
{
	static_assert((std::is_base_of<osgWidget::Widget, Base>::value), "Base class should inherit from osgWidget::Widget");

public:
    //META_Object(osgui, Buttonized);
	//META_Widget(osgui, Buttonized);
    META_ObjectBase(osgui, Buttonized);

private:
    //! Czy jest wciśnięty?
    bool pushed;
    //! Czy mysz jest nad widgetem?
    bool hovered;
    //! Czy można wciskać?
    bool toggleEnabled;
    //! Czy wciśnięty?
    bool toggled;

public:
    //!
    //! \param name
    //! \param color
    //! \param width
    //! \param height
    Buttonized(const std::string & name = "") :
    Base(name),
    StylePolicy(),
    pushed(false),
    hovered(false),
    toggleEnabled(false),
    toggled(false)
    {
        this->onStyleChanged();
    }

    //! Konstruktor dla obiektu o cechach osgWidget::Widget
    Buttonized(const std::string & name, osgWidget::point_type width, osgWidget::point_type height = 0) :
    Base(name, width, height),
    StylePolicy(),
    pushed(false),
    hovered(false),
    toggleEnabled(false),
    toggled(false)
    {
        this->onStyleChanged();
    }

    //! Konstruktor dla obiektu o cechach osgWidget::Label
    Buttonized(const std::string & name, const std::string& label) :
    Base(name, label),
    StylePolicy(),
    pushed(false),
    hovered(false),
    toggleEnabled(false),
    toggled(false)
    {
        this->onStyleChanged();
    }



    //!
    //! \param button
    //! \param copyop
    Buttonized(const Buttonized& button, const osg::CopyOp& copyop) :
        Base(button, copyop), StylePolicy(button, copyop),
        pushed(button.pushed),
        hovered(button.hovered),
        toggleEnabled(button.toggleEnabled),
        toggled(button.toggled)
    {
        this->onStyleChanged();
    }

    virtual ~Buttonized() {}

public:
    //!
    //! \param x
    //! \param y
    //! \param manager
    virtual bool mousePush(double x, double y, const osgWidget::WindowManager* manager)
    {
        pushed = true;
        this->onStyleChanged();
        return Base::mousePush(x, y , manager);
    }
    //!
    //! \param x
    //! \param y
    //! \param manager
    virtual bool mouseRelease(double x, double y, const osgWidget::WindowManager* manager)
    {
        pushed = false;
        toggled = toggleEnabled ? !toggled : toggled;
        this->onStyleChanged();
        return Base::mouseRelease(x, y , manager);
    }
    //!
    //! \param x
    //! \param y
    //! \param manager
    virtual bool mouseEnter(double x, double y, const osgWidget::WindowManager* manager)
    {
        hovered = true;
        this->onStyleChanged();
        return Base::mouseEnter(x, y , manager);
    }
    //!
    //! \param x
    //! \param y
    //! \param manager
    virtual bool mouseLeave(double x, double y, const osgWidget::WindowManager* manager)
    {
        hovered = false;
        this->onStyleChanged();
        return Base::mouseLeave(x, y , manager);
    }

public:
    //! \return
    bool isPushed() const
    {
        return pushed;
    }
    //! \return
    bool isHovered() const
    {
        return hovered;
    }
    //! \return
    bool isToggleEnabled() const
    {
        return toggleEnabled;
    }
    //! \param checkable
    void setToggleEnabled(bool triggerEnabled)
    {
        this->toggleEnabled = triggerEnabled;
    }
    //! \return
    bool isToggled() const
    {
        return toggled;
    }
    //! \param triggered
    void setToggled(bool triggered)
    {
        this->toggled = triggered;
        this->onStyleChanged();
    }

protected:
    //! Odświeża wygląd przycisku.
    void onStyleChanged()
    {
        StylePolicy::applyStyle(this, isToggled(), isPushed(), isHovered());
    }
};

class NullStylePolicy
{
	NullStylePolicy()
	{}
	NullStylePolicy(const NullStylePolicy& policy, const osg::CopyOp& copyop)
	{}
	void applyStyle(osgWidget::Widget* widget, bool toggled, bool pushed, bool hovered)
	{}
};

class DefaultStylePolicy
{
private:
	//! Kolor normalny.
	osgWidget::Color normalColor;
	//! Kolor przy najechaniu.
	osgWidget::Color hoverColor;
	//! Kolor podczas kliknięcia.
	osgWidget::Color pushedColor;
	//! Kolor podczas kliknięcia.
	osgWidget::Color toggleColor;
public:
	DefaultStylePolicy()
	{}
	DefaultStylePolicy(const DefaultStylePolicy& policy, const osg::CopyOp& copyop) :
	normalColor(policy.normalColor),
		hoverColor(policy.hoverColor),
		pushedColor(policy.pushedColor),
		toggleColor(policy.toggleColor)
	{}

	virtual ~DefaultStylePolicy(){}

public:
	//! \param hoverColor
	void setAllColors(osgWidget::Color color)
	{
		normalColor = hoverColor = pushedColor = toggleColor = color;
		this->onStyleChanged();
	}
	//! \return
	osgWidget::Color getHoverColor() const
	{
		return hoverColor;
	}
	//! \param hoverColor
	void setHoverColor(osgWidget::Color hoverColor)
	{
		this->hoverColor = hoverColor;
		this->onStyleChanged();
	}
	//! \return
	osgWidget::Color getPushedColor() const
	{
		return pushedColor;
	}
	//! \param clickedColor
	void setPushedColor(osgWidget::Color clickedColor)
	{
		this->pushedColor = clickedColor;
		this->onStyleChanged();
	}
	//! \return
	osgWidget::Color getNormalColor() const
	{
		return normalColor;
	}
	//! \param normalColor
	void setNormalColor(osgWidget::Color normalColor)
	{
		this->normalColor = normalColor;
		this->onStyleChanged();
	}
	//! \return
	osgWidget::Color getToggleColor() const
	{
		return toggleColor;
	}
	//! \param toggleColor
	void setToggleColor(osgWidget::Color toggleColor)
	{
		this->toggleColor = toggleColor;
		this->onStyleChanged();
	}

protected:

	//! Metoda do implementacji przez klasę pochodną. Powinna wywoływać metodę applyStyle.
	virtual void onStyleChanged() = 0;
	//!
	//! \param widget
	//! \param toggled
	//! \param pushed
	//! \param hovered
	void applyStyle(osgWidget::Widget* widget, bool toggled, bool pushed, bool hovered)
	{
		if ( toggled ) {
			widget->setColor(toggleColor);
		} else {
			widget->setColor(normalColor);
		}

		if ( hovered ) {
			osgWidget::Color src = pushed ? pushedColor : hoverColor;
			float srcA = src.a();
			osgWidget::Color dst = widget->getColor();
			float dstA = dst.a();

			// robimy tutaj mieszanie alpha
			// wyprowadzone z wzoru out = srca * src + (1-srca)*dst
			float a = dstA + srcA - dstA*srcA;
			osgWidget::Color out = (src*srcA + dst*dstA - dst*srcA*dstA)/a;
			out.a() = a;

			// ustawienie koloru
			widget->setColor(out);
		}
	}
};

class LabelStylePolicy : public DefaultStylePolicy
{
private:
	//! Kolor normalny.
	osgWidget::Color normalColor;
	//! Kolor przy najechaniu.
	osgWidget::Color hoverColor;
	//! Kolor podczas kliknięcia.
	osgWidget::Color pushedColor;
	//! Kolor podczas kliknięcia.
	osgWidget::Color toggleColor;
public:
	LabelStylePolicy()
	{}
	LabelStylePolicy(const LabelStylePolicy& policy, const osg::CopyOp& copyop) :
	DefaultStylePolicy(policy),
		normalColor(policy.normalColor),
		hoverColor(policy.hoverColor),
		pushedColor(policy.pushedColor),
		toggleColor(policy.toggleColor)
	{}

	virtual ~LabelStylePolicy(){}

public:
	//! \param hoverColor
	void setLabelAllColors(osgWidget::Color color)
	{
		normalColor = hoverColor = pushedColor = toggleColor = color;
		this->onStyleChanged();
	}
	//! \return
	osgWidget::Color getLabelHoverColor() const
	{
		return hoverColor;
	}
	//! \param hoverColor
	void setLabelHoverColor(osgWidget::Color hoverColor)
	{
		this->hoverColor = hoverColor;
		this->onStyleChanged();
	}
	//! \return
	osgWidget::Color getLabelPushedColor() const
	{
		return pushedColor;
	}
	//! \param clickedColor
	void setLabelPushedColor(osgWidget::Color clickedColor)
	{
		this->pushedColor = clickedColor;
		this->onStyleChanged();
	}
	//! \return
	osgWidget::Color getLabelNormalColor() const
	{
		return normalColor;
	}
	//! \param normalColor
	void setLabelNormalColor(osgWidget::Color normalColor)
	{
		this->normalColor = normalColor;
		this->onStyleChanged();
	}
	//! \return
	osgWidget::Color getLabelToggleColor() const
	{
		return toggleColor;
	}
	//! \param toggleColor
	void setLabelToggleColor(osgWidget::Color toggleColor)
	{
		this->toggleColor = toggleColor;
		this->onStyleChanged();
	}

protected:

	//! Metoda do implementacji przez klasę pochodną. Powinna wywoływać metodę applyStyle.
	virtual void onStyleChanged() = 0;
	//!
	//! \param widget
	//! \param toggled
	//! \param pushed
	//! \param hovered
	void applyStyle(osgWidget::Widget* widget, bool toggled, bool pushed, bool hovered)
	{
		DefaultStylePolicy::applyStyle(widget, toggled, pushed, hovered);
		osgWidget::Label* label = dynamic_cast<osgWidget::Label*>(widget);
		UTILS_ASSERT(label);
		if ( toggled ) {
			label->setFontColor(toggleColor);
		} else {
			label->setFontColor(normalColor);
		}
		if ( hovered ) {
			if ( pushed ) {
				label->setFontColor( pushedColor );
			} else {
				label->setFontColor( hoverColor );
			}
		}
	}
};


class TextStylePolicy
{
private:
	//! normalny styl
	std::string normalStyle;
	//! hovered styl
	std::string hoverStyle;
	//! pushed styl
	std::string pushedStyle;
	//! toggle styl
	std::string toggleStyle;

public:
	TextStylePolicy()
	{}
	TextStylePolicy(const TextStylePolicy& policy, const osg::CopyOp& copyop) :
	normalStyle(policy.normalStyle),
		hoverStyle(policy.hoverStyle),
		pushedStyle(policy.pushedStyle),
		toggleStyle(policy.toggleStyle)
	{}

	virtual ~TextStylePolicy() {}

public:
	//! \param hoverColor
	void setAllStyles(const std::string & allStyle)
	{
		normalStyle = hoverStyle = pushedStyle = toggleStyle = allStyle;
		this->onStyleChanged();
	}
	//! \return
	const std::string & getHoverStyle() const
	{
		return hoverStyle;
	}
	//! \param hoverColor
	void setHoverStyle(const std::string & hoverStyle)
	{
		this->hoverStyle = hoverStyle;
		this->onStyleChanged();
	}
	//! \return
	const std::string & getPushedStyle() const
	{
		return pushedStyle;
	}
	//! \param clickedColor
	void setPushedStyle(const std::string & pushedStyle)
	{
		this->pushedStyle = pushedStyle;
		this->onStyleChanged();
	}
	//! \return
	const std::string & getNormalStyle() const
	{
		return normalStyle;
	}
	//! \param normalColor
	void setNormalStyle(const std::string &  normalStyle)
	{
		this->normalStyle = normalStyle;
		this->onStyleChanged();
	}
	//! \return
	const std::string & getToggleColor() const
	{
		return toggleStyle;
	}
	//! \param toggleColor
	void setToggleStyle(const std::string &  toggleStyle)
	{
		this->toggleStyle = toggleStyle;
		this->onStyleChanged();
	}

protected:

	//! Metoda do implementacji przez klasę pochodną. Powinna wywoływać metodę applyStyle.
	virtual void onStyleChanged() = 0;
	//!
	//! \param widget
	//! \param toggled
	//! \param pushed
	//! \param hovered
	void applyStyle(osgWidget::Widget* widget, bool toggled, bool pushed, bool hovered)
	{
		if ( toggled ) {
			widget->setStyle(toggleStyle);
		} else {
			widget->setStyle(normalStyle);
		}

		if ( hovered ) {
			widget->setStyle(pushed ? pushedStyle : hoverStyle);
		}

		if(widget->getWindowManager() != 0 && widget->getWindowManager()->getStyleManager() != 0){
			widget->getWindowManager()->getStyleManager()->applyStyles(widget);
		}
	}
};

class IBorderStylePolicy {
public:
	virtual ~IBorderStylePolicy() {}
	virtual osgWidget::Color getBorderNormalColor() const { return osgWidget::Color(); };
	virtual void setBorderNormalColor(osgWidget::Color borderNormalColor) {};
	virtual osgWidget::Color getBorderHoverColor() const { return osgWidget::Color(); };
	virtual void setBorderHoverColor(osgWidget::Color borderHoverColor) {};
	virtual osgWidget::Color getBorderPushedColor() const { return osgWidget::Color(); };
	virtual void setBorderPushedColor(osgWidget::Color borderPushedColor) {};
	virtual osgWidget::Color getBorderToggleColor() const { return osgWidget::Color(); };
	virtual void setBorderToggleColor(osgWidget::Color borderToggleColor) {}
	virtual void setBorderAllColors(osgWidget::Color color) {
		setBorderNormalColor(color);
		setBorderHoverColor(color);
		setBorderPushedColor(color);
		setBorderToggleColor(color);
	};

};

template <class Base>
class BorderStylePolicy : public Base, public IBorderStylePolicy
{
private:
	//! Kolor normalny.
	osgWidget::Color borderNormalColor;
	//! Kolor przy najechaniu.
	osgWidget::Color borderHoverColor;
	//! Kolor podczas kliknięcia.
	osgWidget::Color borderPushedColor;
	//! Kolor podczas kliknięcia.
	osgWidget::Color borderToggleColor;
public:    //!
	//! \param name
	BorderStylePolicy() :
		   Base()
		   {}

		   //!
		   //! \param borderized
		   //! \param copyop
		   BorderStylePolicy(const BorderStylePolicy& border, const osg::CopyOp& copyop) :
		   Base(border, copyop),
			   borderNormalColor(border.borderNormalColor),
			   borderHoverColor(border.borderHoverColor),
			   borderToggleColor(border.borderToggleColor),
			   borderPushedColor(border.borderPushedColor)
		   {}

	virtual ~BorderStylePolicy() {}

public:
	//! \return
	osgWidget::Color getBorderNormalColor() const
	{
		return borderNormalColor;
	}
	//! \param borderNormalColor
	void setBorderNormalColor(osgWidget::Color borderNormalColor)
	{
		this->borderNormalColor = borderNormalColor;
		this->onStyleChanged();
	}
	//! \return
	osgWidget::Color getBorderHoverColor() const
	{
		return borderHoverColor;
	}
	//! \param borderHoverColor
	void setBorderHoverColor(osgWidget::Color borderHoverColor)
	{
		this->borderHoverColor = borderHoverColor;
		this->onStyleChanged();
	}
	//! \return
	osgWidget::Color getBorderPushedColor() const
	{
		return borderPushedColor;
	}
	//! \param borderPushedColor
	void setBorderPushedColor(osgWidget::Color borderPushedColor)
	{
		this->borderPushedColor = borderPushedColor;
		this->onStyleChanged();
	}
	//! \return
	osgWidget::Color getBorderToggleColor() const
	{
		return borderToggleColor;
	}
	//! \param borderToggleColor
	void setBorderToggleColor(osgWidget::Color borderToggleColor)
	{
		this->borderToggleColor = borderToggleColor;
		this->onStyleChanged();
	}
	//!
	void setBorderAllColors(osgWidget::Color color)
	{
		borderNormalColor = borderHoverColor = borderPushedColor = borderToggleColor = color;
		this->onStyleChanged();
	}

protected:

	//!
	//! \param widget
	//! \param toggled
	//! \param pushed
	//! \param hovered
	template <class Borderized>
	void applyStyle(Borderized* widget, bool toggled, bool pushed, bool hovered)
	{
		Base::applyStyle(widget, toggled, pushed, hovered);
		if ( toggled ) {
			widget->setBorderColor( borderToggleColor );
		} else {
			widget->setBorderColor( borderNormalColor );
		}
		if ( hovered ) {
			if ( pushed ) {
				widget->setBorderColor( borderPushedColor );
			} else {
				widget->setBorderColor( borderHoverColor );
			}
		}
	}
};


typedef Buttonized< osgWidget::Widget > Button;


//
///**
// *
// */
//class Button : public osgWidget::Widget
//{
//private:
//  //! Kolor normalny.
//  osgWidget::Color normalColor;
//  //! Kolor przy najechaniu.
//  osgWidget::Color hoverColor;
//  //! Kolor podczas kliknięcia.
//  osgWidget::Color pushedColor;
//  //! Kolor podczas kliknięcia.
//  osgWidget::Color toggleColor;
//
//  //! Czy jest wciśnięty?
//  bool pushed;
//  //! Czy jest przeciągany?
//  bool dragged;
//  //! Czy mysz jest nad widgetem?
//  bool hovered;
//  //! Czy można wciskać?
//  bool toggleEnabled;
//  //! Czy wciśnięty?
//  bool toggled;
//
//
//public:
//  META_Object(osgui, Button);
//
//  //!
//  //! \param name
//  //! \param color
//  //! \param width
//  //! \param height
//  Button(const std::string & name = "",
//    double width = 0.0, double height = 0.0);
//  //!
//  //! \param button
//  //! \param copyop
//  Button(const Button& button, const osg::CopyOp& copyop);
//
//  //!
//  //! \param x
//  //! \param y
//  //! \param manager
//  virtual bool mousePush(double x, double y, const osgWidget::WindowManager* manager);
//  //!
//  //! \param x
//  //! \param y
//  //! \param manager
//  virtual bool mouseRelease(double x, double y, const osgWidget::WindowManager* manager);
//  //!
//  //! \param x
//  //! \param y
//  //! \param manager
//  virtual bool mouseEnter(double x, double y, const osgWidget::WindowManager* manager);
//  //!
//  //! \param x
//  //! \param y
//  //! \param manager
//  virtual bool mouseLeave(double x, double y, const osgWidget::WindowManager* manager);
//  //!
//  //! \param x
//  //! \param y
//  //! \param manager
//  virtual bool mouseDrag(double x, double y, const osgWidget::WindowManager* manager);
//
//  //! \return
//  osgWidget::Color getHoverColor() const
//  {
//    return hoverColor;
//  }
//  //! \param hoverColor
//  void setHoverColor(osgWidget::Color hoverColor)
//  {
//    this->hoverColor = hoverColor;
//    refresh();
//  }
//  //! \return
//  osgWidget::Color getPushedColor() const
//  {
//    return pushedColor;
//  }
//  //! \param clickedColor
//  void setPushedColor(osgWidget::Color clickedColor)
//  {
//    this->pushedColor = clickedColor;
//    refresh();
//  }
//  //! \return
//  osgWidget::Color getNormalColor() const
//  {
//    return normalColor;
//  }
//  //! \param normalColor
//  void setNormalColor(osgWidget::Color normalColor)
//  {
//    this->normalColor = normalColor;
//    refresh();
//  }
//  //! \return
//  osgWidget::Color getToggleColor() const
//  {
//      return toggleColor;
//  }
//  //! \param toggleColor
//  void setToggleColor(osgWidget::Color toggleColor)
//  {
//      this->toggleColor = toggleColor;
//      refresh();
//  }
//
//  //! \return
//  bool isPushed() const
//  {
//    return pushed;
//  }
//  //! \return
//  bool isDragged() const
//  {
//    return dragged;
//  }
//  //! \return
//  bool isHovered() const
//  {
//    return hovered;
//  }
//
//  //! \return
//  bool isToggleEnabled() const
//  {
//      return toggleEnabled;
//  }
//  //! \param checkable
//  void setToggleEnabled(bool triggerEnabled)
//  {
//      this->toggleEnabled = triggerEnabled;
//  }
//
//  //! \return
//  bool isToggled() const
//  {
//      return toggled;
//  }
//  //! \param triggered
//  void setToggled(bool triggered)
//  {
//      this->toggled = triggered;
//      refresh();
//  }
//
//
//public:
//    virtual void parented(osgWidget::Window* window);
//    virtual void unparented(osgWidget::Window* window);
//    virtual void positioned();
//
//protected:
//    //! Odświeża wygląd przycisku.
//    void refresh();
//    //! Wywoływane w pierwszej kolejności.
//    virtual void applyState(bool triggered);
//    //! Wywoływane w drugiej kolejności.
//    virtual void applyHover(bool pushed);
//};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__BUTTON_H__
