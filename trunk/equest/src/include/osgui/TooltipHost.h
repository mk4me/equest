#ifndef HEADER_GUARD__TOOLTIPHOST_H__
#define HEADER_GUARD__TOOLTIPHOST_H__

#include <osgWidget/Widget>
#include <osgui/Tooltip.h>
#include <boost/tokenizer.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

class TooltipHost
{
public:
	TooltipHost(void);
	virtual ~TooltipHost(void);

	void setTooltipText(const std::string & text);
	const std::string & getTooltipText() const;
	void clearTooltip();

	void showTooltip();
	void showTooltip(const osgWidget::XYCoord & pos);
	void showTooltip(osgWidget::point_type x, osgWidget::point_type y);

	void hideTooltip();

	void setMaxSignsPerLine(unsigned int signs = 40);
	unsigned int getMaxSignsPerLine() const;

	const osgui::Tooltip * getTooltip() const;

	bool isTooltipVisible() const;

private:
	bool addTooltipToWM();
	//void strataForegroundToNone();
	//void restoreStrataForeground();
	std::string rebuildTooltipText(const std::string & text) const;

private:
	//typedef std::set<osg::observer_ptr<osgWidget::Window> > RESTORE;
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

private:
	osg::ref_ptr<osgui::Tooltip> tooltip;
	std::string tooltipOriginalText;
	//RESTORE toRestore;

	std::string firstLevelSeparators;
	std::string secondLevelSeparators;

	unsigned int maxSignsPerLine;

};

}

#endif

