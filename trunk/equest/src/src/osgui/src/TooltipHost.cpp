#include "PCH.h"
#include <osgui/TooltipHost.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

TooltipHost::TooltipHost(void) : tooltip(new osgui::Tooltip()), maxSignsPerLine(40), firstLevelSeparators(" \t\n.,!?\\/-=*-+")
{
}


TooltipHost::~TooltipHost(void)
{
}

void TooltipHost::setMaxSignsPerLine(unsigned int signs){
	if(signs != maxSignsPerLine){
		maxSignsPerLine = signs;
		tooltip->setText(rebuildTooltipText(tooltipOriginalText));
	}
}

unsigned int TooltipHost::getMaxSignsPerLine() const{
	return maxSignsPerLine;
}

std::string TooltipHost::rebuildTooltipText(const std::string & text) const{
	
	if(text.size() <= maxSignsPerLine){
		return text;
	}

	std::string ret(text);

	std::string::size_type prev = maxSignsPerLine;
	std::string::size_type currBreak = 0;
	std::string::size_type pos = text.find_first_of(firstLevelSeparators.c_str());

	bool finished = false;

	while(finished == false){
		if(pos == std::string::npos){
			while( (currBreak += (1 + maxSignsPerLine)) < ret.size()){
				if(ret[currBreak] == ' '){
					ret[currBreak--] = '\n';
				}else{
					ret.insert(currBreak, "\n");
				}
			}

			finished = true;
		}else if(ret[pos] == '\n'){
			currBreak = pos;
			++pos;
		}else if(pos - currBreak > maxSignsPerLine){
			//lam w prev
			if(ret[prev] == ' '){
				ret[prev] = '\n';
			}else{
				ret.insert(prev, "\n");
			}
			currBreak = prev;
		}

		//szukaj dalej
		if(pos - currBreak <= maxSignsPerLine){
			prev = pos;
		}else{
			prev = currBreak + maxSignsPerLine + 1;
		}

		pos = ret.find_first_of(firstLevelSeparators.c_str(), prev+1);
	}

	return ret;
}

void TooltipHost::setTooltipText(const std::string & text){
	tooltipOriginalText = text;
	tooltip->setText(rebuildTooltipText(tooltipOriginalText));
}

const std::string & TooltipHost::getTooltipText() const{
	return tooltipOriginalText;
}

void TooltipHost::clearTooltip(){
	tooltip->clear();
}

const osgui::Tooltip * TooltipHost::getTooltip() const{
	return tooltip;
}

void TooltipHost::showTooltip(){
	if(addTooltipToWM() == true){
		//strataForegroundToNone();
		tooltip->osgWidget::Box::show();
        tooltip->getWindowManager()->setFocused(tooltip);
	}
}

void TooltipHost::showTooltip(const osgWidget::XYCoord & pos){
	if(addTooltipToWM() == true){
		//strataForegroundToNone();
		tooltip->show(pos);
	}
}

void TooltipHost::showTooltip(osgWidget::point_type x, osgWidget::point_type y){
	if(addTooltipToWM() == true){
		//strataForegroundToNone();
		tooltip->show(x,y);
	}
}

void TooltipHost::hideTooltip(){
	if(addTooltipToWM() == true){
		//restoreStrataForeground();
		tooltip->hide();
	}
}

bool TooltipHost::isTooltipVisible() const{
	if(tooltip->getWindowManager() != nullptr && tooltip->isVisible() == true){
		return true;
	}

	return false;
}

bool TooltipHost::addTooltipToWM(){
	
	if(tooltip->getWindowManager() == nullptr){
		osgWidget::WindowManager& wm = dynamic_cast<osgWidget::WindowManager&>(*this);
		return wm.addChild(tooltip);
	}

	return true;
}

//void TooltipHost::strataForegroundToNone(){
//	osgWidget::WindowManager& wm = dynamic_cast<osgWidget::WindowManager&>(*this);
//	for(osgWidget::UIObjectParent<osgWidget::Window>::Iterator it = wm.begin(); it != wm.end(); it++){
//		if( it->valid() == true && it->get() != tooltip && it->get()->getStrata() == osgWidget::Window::STRATA_FOREGROUND){
//			toRestore.insert(*it);
//			it->get()->setStrata(osgWidget::Window::STRATA_NONE);
//		}
//	}
//
//	wm.setFocused(tooltip);
//}
//
//void TooltipHost::restoreStrataForeground(){
//	if(toRestore.empty() == false){
//		bool f = false;
//		osgWidget::Window * newFocused = nullptr;
//		for(RESTORE::iterator it = toRestore.begin(); it != toRestore.end(); it++){
//			if(it->valid() == true){
//				it->get()->setStrata(osgWidget::Window::STRATA_FOREGROUND);
//				if(f == false){
//					f = true;
//					newFocused = it->get();
//				}
//			}
//		}
//
//		if(f == true){
//			osgWidget::WindowManager& wm = dynamic_cast<osgWidget::WindowManager&>(*this);
//			wm.setFocused(newFocused);
//		}
//
//		toRestore.swap(RESTORE());
//	}
//}

}
