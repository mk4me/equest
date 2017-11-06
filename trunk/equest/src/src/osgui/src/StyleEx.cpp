#include "PCH.h"
#include <osgui/StyleEx.h>
#include <osgui/Buttonized.h>
#include <osg/Image>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

osg::ref_ptr<StyleEx::CustomStyleOptions> StyleEx::customStyleOptions = new CustomStyleOptions();

bool StyleEx::applyStyle(osgWidget::Widget* w, osgWidget::Reader r) {

	if(Style::applyStyle(w,r) == true){
		return true;
	}

	return handleExWidgets(w,r);
}

bool StyleEx::applyStyle(osgWidget::Label* label, osgWidget::Reader r) {
	if(Style::applyStyle(label,r) == true){
		return true;
	}

	std::string str;
	osg::Vec3   vec3;
	osg::Vec4   vec4;
	float       f;
	bool ret = true;

	//! Font color using 4x 0-255 integers.
	if(_match("font_color %i %i %i %i", r)) {
		r.readSequence(vec4);
		label->setFontColor(vec4 / 255.0f);
	}

	//! Font color using 3x 0-255 integers with a default alpha of 255.
	else if(_match("font_color %i %i %i", r)) {
		r.readSequence(vec3);
		label->setFontColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
	}

	//! Font color using 4x 0.0f-1.0f floats.
	else if(_match("font_color %f %f %f %f", r)) {
		r.readSequence(vec4);
		label->setFontColor(vec4);
	}

	//! Font color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
	else if(_match("font_color %f %f %f", r)) {
		r.readSequence(vec3);
		label->setFontColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
	}

	//! Font size using int or float.
	else if(_match("font_size %i", r) || _match("font_size %f", r)) {
		r.readSequence(f);
		label->setFontSize(f);
	}

	//! Font type using string.
	else if(_match("font_type %s", r)) {
		r.readSequence(str);
		label->setFont(str);
	}

	//fit to font settings
	else if(_match("font_fit %w", r)) {
		r.readSequence(str);
		
		LabelFontFitMode ffmode = strToFontFit(str);
		osgWidget::XYCoord textSize = label->getTextSize();
        bool fillable = label->canFill();
        label->setCanFill(false);
		
        switch(ffmode){
		case HorizontalFit:
			label->setHeight(textSize.y());
			label->addOrigin(0,0);
            textSize = label->getSize();
            if(label->getParent() != nullptr){
                //label->getParent()->resize(label->getWidth(), label->getHeight());
                label->getParent()->resize();
                if(fillable == true){
                    label->setCanFill(fillable);
                    label->addOrigin(0,0);
                    //label->setSize(textSize);
                    label->getParent()->resize();
                }
            }

			break;
		case VerticalFit:
			label->setWidth(textSize.x());
			label->addOrigin(0,0);
            textSize = label->getSize();
            if(label->getParent() != nullptr){
                //label->getParent()->resize(label->getWidth(), label->getHeight());
                label->getParent()->resize();
                if(fillable == true){
                    label->setCanFill(fillable);
                    label->addOrigin(0,0);
                    //label->setSize(textSize);
                    label->getParent()->resize();
                }
            }
			break;
		case AllFit:
			label->setSize(textSize);
			label->addOrigin(0,0);
            textSize = label->getSize();
            if(label->getParent() != nullptr){
                //label->getParent()->resize(label->getWidth(), label->getHeight());
                label->getParent()->resize();
                if(fillable == true){
                    label->setCanFill(fillable);
                    label->addOrigin(0,0);
                    //label->setSize(textSize);
                    label->getParent()->resize();
                }
            }
			break;

        default:
            label->setCanFill(fillable);
            break;
		}
	}

	//text alignment based on osgText::Text capabilities
	else if(label->getText() != 0 && _match("font_align %w", r)) {
		r.readSequence(str);

		//TODO FIX
		//label->getText()->setAlignment(strToFontAligment(str));
		//label->setLabel(label->getLabel());
		//label->setSize(label->getTextSize());

        osgWidget::warn() << "Font alignment in Label not implemented" <<
            "yet. Nothing has happened" << std::endl;

	}

	//! Font handled - continue
	else{
        ret = applyStyle((osgWidget::Widget*)label,r);
	}

    /*if(ret == false){
    return handleExWidgets(label,r);
    }else{
    return true;
    }*/

    return ret;

}

bool StyleEx::applyStyle(osgWidget::Input* input, osgWidget::Reader r) {
	if(Style::applyStyle(input,r) == true){
		return true;
	}

	return handleExWidgets(input,r);
}

bool StyleEx::applyStyle(osgWidget::Window* window, osgWidget::Reader r){
	if(osgWidget::Style::applyStyle(window, r) == true){
		return true;
	}

	return handleExWindows(window, r);
}

StyleEx::LabelFontFitMode StyleEx::strToFontFit(const std::string & fontFit){
	std::string ff = osgWidget::lowerCase(fontFit);

	if(ff == "vertical") return VerticalFit;

	else if(ff == "horizontal") return HorizontalFit;

	else if(ff == "all") return AllFit;

	else {
		osgWidget::warn()
			<< "Unknown FontFit mode [" << ff
			<< "]; using none." << std::endl
			;

		return NoFit;
	}
}

osgText::Text::AlignmentType StyleEx::strToFontAligment(const std::string & fontAlign){
	std::string fa = osgWidget::lowerCase(fontAlign);

	if(fa == "left_top"){
		return osgText::Text::LEFT_TOP;
	}else if(fa == "left_center"){
		return osgText::Text::LEFT_CENTER;
	}else if(fa == "left_bottom"){
		return osgText::Text::LEFT_BOTTOM;
	}else if(fa == "center_top"){
		return osgText::Text::CENTER_TOP;
	}else if(fa == "center_center"){
		return osgText::Text::CENTER_CENTER;
	}else if(fa == "center_bottom"){
		return osgText::Text::CENTER_BOTTOM;
	}else if(fa == "right_top"){
		return osgText::Text::RIGHT_TOP;
	}else if(fa == "right_center"){
		return osgText::Text::RIGHT_CENTER;
	}else if(fa == "right_bottom"){
		return osgText::Text::RIGHT_BOTTOM;
	}else if(fa == "left_base_line"){
		return osgText::Text::LEFT_BASE_LINE;
	}else if(fa == "center_base_line"){
		return osgText::Text::CENTER_BASE_LINE;
	}else if(fa == "right_base_line"){
		return osgText::Text::RIGHT_BASE_LINE;
	}else if(fa == "left_bottom_base_line"){
		return osgText::Text::LEFT_BOTTOM_BASE_LINE;
	}else if(fa == "center_bottom_base_line"){
		return osgText::Text::CENTER_BOTTOM_BASE_LINE;
	}else if(fa == "right_bottom_base_line"){
		return osgText::Text::RIGHT_BOTTOM_BASE_LINE;
	}else if(fa == "base_line"){
		return osgText::Text::BASE_LINE;
	}else{
		osgWidget::warn()
			<< "Unknown font_align mode [" << fa
			<< "]; using base_line." << std::endl
			;

		return osgText::Text::BASE_LINE;
	}
}

bool StyleEx::handleExWindows(osgWidget::Window* window, osgWidget::Reader r){
	//! Allow window background styling as it is Widget
    if(window->getBackground() != nullptr){
		return applyStyle(window->getBackground(), r);
	}

	return false;
}


bool StyleEx::handleExWidgets(osgWidget::Widget* w, osgWidget::Reader r){
	std::string str;
	bool ret = false;

    //use caching
    if(_match("image_cache %s", r)){
        r.readSequence(str);
        osg::Image * img = dynamic_cast<osg::Image*>(osgDB::Registry::instance()->getFromObjectCache(str));
        if(img == nullptr){
            img = osgDB::readImageFile(str, customStyleOptions);
        }

        w->setImage(img);
        ret = true;
    }else if(_match("rect_image_cache %s", r)){
        r.readSequence(str);
        osg::Image * img = dynamic_cast<osg::Image*>(osgDB::Registry::instance()->getFromObjectCache(str));
        if(img == nullptr){
            img = osgDB::readImageFile(str, customStyleOptions);
        }

        w->setImage(img,true, true);
        ret = true;
    }else
	//use optimized image setting method
	if(_match("rect_image %s", r)) {
		r.readSequence(str);
		w->setImage(str, true, true);
        ret = true;
	}else{
		osg::Vec2   vec2;
		osg::Vec3   vec3;
		osg::Vec4   vec4;
		float f;

		if(dynamic_cast<IBorderized*>(w) != nullptr){
            ret = true;
			IBorderized* borderized = dynamic_cast<IBorderized*>(w);
			//! Border color using 4x 0-255 integers.
			if(_match("border_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				borderized->setBorderColor(vec4 / 255.0f);
			}

			//! Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("border_color %i %i %i", r)) {
				r.readSequence(vec3);
				borderized->setBorderColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Border color using 4x 0.0f-1.0f floats.
			else if(_match("border_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				borderized->setBorderColor(vec4);
			}

			//! Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("border_color %f %f %f", r)) {
				r.readSequence(vec3);
				borderized->setBorderColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			//! Border thickness using int or float
			else if(_match("border_width %i", r) || _match("border_width %f", r)) {
				r.readSequence(f);
				borderized->setBorderWidth(f);
			}

			//did not find anything - continue
			else{
				ret = false;
			}

			//was not an extended window - continue
		}
		
        //! check if style already recognized and applied
        //! if not test if known interface implemented and try to apply style with this interface
		if(ret == false && dynamic_cast<osgui::DefaultStylePolicy*>(w) != nullptr){
			osgui::DefaultStylePolicy* def = dynamic_cast<osgui::DefaultStylePolicy*>(w);
			ret = true;

			//! Hover color using 4x 0-255 integers.
			if(_match("hover_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				def->setHoverColor(vec4 / 255.0f);
			}

			//! Hover color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("hover_color %i %i %i", r)) {
				r.readSequence(vec3);

				def->setHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Hover color using 4x 0.0f-1.0f floats.
			else if(_match("hover_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				def->setHoverColor(vec4);
			}

			//! Hover color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("hover_color %f %f %f", r)) {
				r.readSequence(vec3);
				def->setHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

            //! Toggle color using 4x 0-255 integers.
			else if(_match("toggle_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				def->setToggleColor(vec4 / 255.0f);
			}

			//! Toggle color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("toggle_color %i %i %i", r)) {
				r.readSequence(vec3);
				def->setToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Toggle color using 4x 0.0f-1.0f floats.
			else if(_match("toggle_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				def->setToggleColor(vec4);
			}

			//! Toggle color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("toggle_color %f %f %f", r)) {
				r.readSequence(vec3);
				def->setToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

            //! Pushed color using 4x 0-255 integers.
			else if(_match("pushed_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				def->setPushedColor(vec4 / 255.0f);
			}

			//! Pushed color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("pushed_color %i %i %i", r)) {
				r.readSequence(vec3);
				def->setPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Pushed color using 4x 0.0f-1.0f floats.
			else if(_match("pushed_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				def->setPushedColor(vec4);
			}

			//! Pushed color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("pushed_color %f %f %f", r)) {
				r.readSequence(vec3);
				def->setPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

            //! Normal color using 4x 0-255 integers.
			else if(_match("normal_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				def->setNormalColor(vec4 / 255.0f);
			}

			//! Normal color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("normal_color %i %i %i", r)) {
				r.readSequence(vec3);
				def->setNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Normal color using 4x 0.0f-1.0f floats.
			else if(_match("normal_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				def->setNormalColor(vec4);
			}

			//! Normal color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("normal_color %f %f %f", r)) {
				r.readSequence(vec3);
				def->setNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			//did not find anything - continue
			else{
				ret = false;
			}
			
		}
		
        //! check if style already recognized and applied
        //! if not test if known interface implemented and try to apply style with this interface
		if(ret == false && dynamic_cast<osgui::LabelStylePolicy*>(w) != nullptr){
			osgui::LabelStylePolicy* lab = dynamic_cast<osgui::LabelStylePolicy*>(w);

			ret = true;

            //! Label text hover color using 4x 0-255 integers.
			if(_match("label_hover_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				lab->setLabelHoverColor(vec4 / 255.0f);
			}

			//! Label text hover color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("label_hover_color %i %i %i", r)) {
				r.readSequence(vec3);
				lab->setLabelHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Label text hover color using 4x 0.0f-1.0f floats.
			else if(_match("label_hover_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				lab->setLabelHoverColor(vec4);
			}

			//! Label text hover color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("label_hover_color %f %f %f", r)) {
				r.readSequence(vec3);
				lab->setLabelHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

            //! Label text toggle color using 4x 0-255 integers.
			else if(_match("label_toggle_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				lab->setLabelToggleColor(vec4 / 255.0f);
			}

			//! Label text toggle color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("label_toggle_color %i %i %i", r)) {
				r.readSequence(vec3);
				lab->setLabelToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Label text toggle color using 4x 0.0f-1.0f floats.
			else if(_match("label_toggle_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				lab->setLabelToggleColor(vec4);
			}

			//! Label text toggle color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("label_toggle_color %f %f %f", r)) {
				r.readSequence(vec3);
				lab->setLabelToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

            //! Label text push color using 4x 0-255 integers.
			else if(_match("label_pushed_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				lab->setLabelPushedColor(vec4 / 255.0f);
			}

			//! Label text push color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("label_pushed_color %i %i %i", r)) {
				r.readSequence(vec3);
				lab->setLabelPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Label text push color using 4x 0.0f-1.0f floats.
			else if(_match("label_pushed_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				lab->setLabelPushedColor(vec4);
			}

			//! Label text push color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("label_pushed_color %f %f %f", r)) {
				r.readSequence(vec3);
				lab->setLabelPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

            //! Label text normal color using 4x 0-255 integers.
			else if(_match("label_normal_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				lab->setLabelNormalColor(vec4 / 255.0f);
			}

			//! Label text normal color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("label_normal_color %i %i %i", r)) {
				r.readSequence(vec3);
				lab->setLabelNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Label text normal color using 4x 0.0f-1.0f floats.
			else if(_match("label_normal_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				lab->setLabelNormalColor(vec4);
			}

			//! Label text normal color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("label_normal_color %f %f %f", r)) {
				r.readSequence(vec3);
				lab->setLabelNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			//did not find anything - continue
			else{
				ret = false;
			}
		}

        //! check if style already recognized and applied
        //! if not test if known interface implemented and try to apply style with this interface
		if(ret == false && dynamic_cast<osgui::IBorderStylePolicy*>(w) != nullptr){
			osgui::IBorderStylePolicy* bord = dynamic_cast<osgui::IBorderStylePolicy*>(w);

			ret = true;

            //! Borderized buttonized border hover color using 4x 0-255 integers.
			if(_match("border_hover_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				bord->setBorderHoverColor(vec4 / 255.0f);
			}

			//! Borderized buttonized border hover color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("border_hover_color %i %i %i", r)) {
				r.readSequence(vec3);
				bord->setBorderHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Borderized buttonized border hover color using 4x 0.0f-1.0f floats.
			else if(_match("border_hover_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				bord->setBorderHoverColor(vec4);
			}

			//! Borderized buttonized border hover color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("border_hover_color %f %f %f", r)) {
				r.readSequence(vec3);
				bord->setBorderHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

            //! Borderized buttonized border toggle color using 4x 0-255 integers.
			else if(_match("border_toggle_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				bord->setBorderToggleColor(vec4 / 255.0f);
			}

			//! Borderized buttonized border toggle color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("border_toggle_color %i %i %i", r)) {
				r.readSequence(vec3);
				bord->setBorderToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Borderized buttonized border toggle color using 4x 0.0f-1.0f floats.
			else if(_match("border_toggle_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				bord->setBorderToggleColor(vec4);
			}

			//! Borderized buttonized border toggle color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("border_toggle_color %f %f %f", r)) {
				r.readSequence(vec3);
				bord->setBorderToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

            //! Borderized buttonized border push color using 4x 0-255 integers.
			else if(_match("border_pushed_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				bord->setBorderPushedColor(vec4 / 255.0f);
			}

			//! Borderized buttonized border push color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("border_pushed_color %i %i %i", r)) {
				r.readSequence(vec3);
				bord->setBorderPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Borderized buttonized border push color using 4x 0.0f-1.0f floats.
			else if(_match("border_pushed_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				bord->setBorderPushedColor(vec4);
			}

			//! Borderized buttonized border push color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("border_pushed_color %f %f %f", r)) {
				r.readSequence(vec3);
				bord->setBorderPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

            //! Borderized buttonized border normal color using 4x 0-255 integers.
			else if(_match("border_normal_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				bord->setBorderNormalColor(vec4 / 255.0f);
			}

			//! Borderized buttonized border normal color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("border_normal_color %i %i %i", r)) {
				r.readSequence(vec3);
				bord->setBorderNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			//! Borderized buttonized border normal color using 4x 0.0f-1.0f floats.
			else if(_match("border_normal_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				bord->setBorderNormalColor(vec4);
			}

			//! Borderized buttonized border normal color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("border_normal_color %f %f %f", r)) {
				r.readSequence(vec3);
				bord->setBorderNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			//did not find anything - continue
			else{
				ret = false;
			}
		}
		
        //! check if style already recognized and applied
        //! if not test if known interface implemented and try to apply style with this interface
		if(ret == false && dynamic_cast<osgui::TextStylePolicy*>(w) != nullptr){
			osgui::TextStylePolicy* txt = dynamic_cast<osgui::TextStylePolicy*>(w);

			ret = true;

            //! Buttonized on hover style name
			if(_match("hover_style %s", r)) {
				r.readSequence(str);
				txt->setHoverStyle(str);
			}

			//! Buttonized on toggle style name
			else if(_match("toggle_style %s", r)) {
				r.readSequence(str);
				txt->setToggleStyle(str);
			}

			//! Buttonized on push style name
			else if(_match("pushed_style %s", r)) {
				r.readSequence(str);
				txt->setPushedStyle(str);
			}

			//! Buttonized normal style name
			else if(_match("normal_style %s", r)) {
				r.readSequence(str);
				txt->setNormalStyle(str);
			}

			else{
				ret = false;
			}
		}else{
			ret = false;
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////
