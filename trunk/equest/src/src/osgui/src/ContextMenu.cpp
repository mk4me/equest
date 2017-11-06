#include "PCH.h"
#include <osgui/ContextMenu.h>
#include <osgui/Utils2D.h>
#include <osgui/AspectRatioKeeper.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

const ContextMenu::MenuItem ContextMenu::constEmptyMenuItem = ContextMenu::MenuItem();
const ContextMenu::MenuSubmenu ContextMenu::constEmptyMenuSubmenu = ContextMenu::MenuSubmenu();
ContextMenu::MenuItem ContextMenu::emptyMenuItem = constEmptyMenuItem;
ContextMenu::MenuSubmenu ContextMenu::emptyMenuSubmenu = constEmptyMenuSubmenu;
const std::string ContextMenu::defaultPathSeparator = "/";

ContextMenu::ContextMenu(void) :
pathSeparator(new std::string(defaultPathSeparator))
{
    rootMenu = this;
	setStyle("osg.contextmenu.menu");
}

ContextMenu::ContextMenu(ContextMenu * parent) :
parentMenu(parent), pathSeparator(parent->pathSeparator)
{
    UTILS_ASSERT((parent != nullptr));
    rootMenu = parent->rootMenu;
    setStyle("osg.contextmenu.menu");
}

ContextMenu::~ContextMenu(void)
{
}

void ContextMenu::managed(osgWidget::WindowManager * wm){
    osgui::Grid::managed(wm);
    hide();
}

void ContextMenu::update(){
    if(getWindowManager() != nullptr){
        unsigned int i = getWindowManager()->getNumChildren() + 3;
        float range = 1.0f / (float)i;

        this->_z = -range;
        this->_zRange = range / 2.0;
    }

    Grid::update();
}

bool ContextMenu::addMenuItem(const std::string & path, bool checked, const ContextMenu::OnClickCallback & onClickCallback, const ContextMenu::OnHoverCallback & onHoverCallback){
	return addMenuItem(parseNextMenuItems(path), checked, onClickCallback, onHoverCallback);
}

bool ContextMenu::removeMenuItem(const std::string & path){
	return removeMenuItem(parseNextMenuItems(path));
}

bool ContextMenu::setMenuItemChecked(const std::string & path, bool checked){
	return setMenuItemChecked(parseNextMenuItems(path), checked);
}

bool ContextMenu::isSubmenuDirectionRight() const{
	if(parentMenu == nullptr || getWindowManager() == nullptr ||
		parentMenu->getAbsoluteOrigin().x() + parentMenu->getWidth() + getWidth() <= getWindowManager()->getWidth()){
		return true;
	}

	return false;
}

bool ContextMenu::setMenuOnCloseCallback(const std::string & path, const ContextMenu::OnCloseCallback & callback){
	if(path.empty() == true){
		this->closeMenuCallback = callback;
		return true;
	}

	return setMenuOnCloseCallback(parseNextMenuItems(path), callback);
}

bool ContextMenu::showMenu(){
    if(this->isVisible() == true){
        return true;
    }

    if(rootMenu == this){
        show();
        return true;
    }

    if(parentMenu->isVisible() == true){
        if(parentMenu->activeSubMenu != nullptr){
            parentMenu->activeSubMenu->hideMenu();
        }

        parentMenu->activeSubMenu = this;

        if(parentMenu->getWindowManager() != nullptr){
            parentMenu->getWindowManager()->addChild(this);
        }
        show();
        return true;
    }

    return false;
}

bool ContextMenu::showMenu(const osgWidget::XYCoord & pos){
	setOrigin(pos);
	resize();

    return showMenu();
}

bool ContextMenu::showMenu(osgWidget::point_type x, osgWidget::point_type y){
	return showMenu(osgWidget::XYCoord(x,y));
}

void ContextMenu::hideMenu(){
	//go down, and start hiding up to here
	if(isVisible() == true){
		osg::observer_ptr<ContextMenu> last = this;
		while(last->activeSubMenu.valid() == true){
			last = last->activeSubMenu;
		}

		while(last != this){
			if(last->closeMenuCallback){
				last->closeMenuCallback(last.get());
			}

			last->hide();
			last->activeSubMenu = nullptr;
			osg::observer_ptr<ContextMenu> paret = last->parentMenu;
			if(paret->getWindowManager() != nullptr){
				paret->getWindowManager()->removeChild(last.get());
			}

			last = paret;
		}

		if(closeMenuCallback){
			closeMenuCallback(this);
		}

		hide();
		activeSubMenu = nullptr;

		if(parentMenu.valid() == true){
			parentMenu->activeSubMenu = nullptr;
			if(parentMenu->getWindowManager() != nullptr){
				parentMenu->getWindowManager()->removeChild(this);
			}
		}
	}
}

bool ContextMenu::isEmpty() const{
    if(items.empty() && submenus.empty()){
        return true;
    }

    return false;
}

void ContextMenu::clearMenu(){
    UTILS_FAIL("Nie zaimplementowano");
    //chowaj menu, usun z WMa wszystko co niepotrzebne
    hideMenu();

    int rows = getNumRows();
    for(int i = rows; i >= 0; i--){
       // removeRow(i);
    }

    //usun elementy menu
    items = Items();
    submenus = Submenus();
    activeSubMenu = nullptr;
}

std::vector<std::string> ContextMenu::parseNextMenuItems(const std::string & path) const{
	std::vector<std::string> ret;

	Tokenizer tokens(path, boost::char_separator<char>((*pathSeparator).c_str()));
	for(Tokenizer::iterator it = tokens.begin(); it != tokens.end(); ++it){
		ret.push_back(*it);
	}

	return ret;
}

bool ContextMenu::onItemPush(osgWidget::Event& ev){
	//call callback function

	if(getWindowManager()->isLeftMouseButtonDown() == false){
		return false;
	}

	MenuItem * menuItem = static_cast<MenuItem *>(ev.getData());

	menuItem->checked = ! menuItem->checked;
	if(menuItem->onClickCallback){
		menuItem->onClickCallback(menuItem->menuItem->getLabel(), menuItem->checked);
	}

	refreshMenuItemCheckedStyle(*menuItem);

	//hide context menu
	rootMenu->hideMenu();
	return false;
}

bool ContextMenu::onItemEnter(osgWidget::Event& ev){

	MenuItem * menuItem = static_cast<MenuItem *>(ev.getData());

	menuItem->menuItem->setStyle("osg.contextmenu.item.hovered");

	//if(getWindowManager()->getStyleManager() != nullptr){
		getWindowManager()->getStyleManager()->applyStyles(menuItem->menuItem);
	//}

	refreshMenuItemCheckedStyle(*menuItem, true);

	if(menuItem->onHoverCallback){
		menuItem->onHoverCallback(menuItem->menuItem->getLabel(), true);
	}

	return false;
}

bool ContextMenu::onItemLeave(osgWidget::Event& ev){

	MenuItem * menuItem = static_cast<MenuItem *>(ev.getData());

	menuItem->menuItem->setStyle("osg.contextmenu.item.normal");

	//if(getWindowManager() != nullptr && getWindowManager()->getStyleManager() != nullptr){
		getWindowManager()->getStyleManager()->applyStyles(menuItem->menuItem);
	//}

	refreshMenuItemCheckedStyle(*menuItem);

	if(menuItem->onHoverCallback){
		menuItem->onHoverCallback(menuItem->menuItem->getLabel(), false);
	}

	return false;
}

bool ContextMenu::onSubmenuEnter(osgWidget::Event& ev){

	MenuSubmenu * submenuItem = static_cast<MenuSubmenu *>(ev.getData());

	submenuItem->submenuItem->setStyle("osg.contextmenu.submenuitem.hovered");
	submenuItem->emptyWidget->setStyle("osg.contextmenu.submenuitem.hovered");
	//if(getWindowManager()->getStyleManager() != nullptr){
		getWindowManager()->getStyleManager()->applyStyles(submenuItem->submenuItem);
		getWindowManager()->getStyleManager()->applyStyles(submenuItem->emptyWidget);
	//}

    osg::ref_ptr<ContextMenu> actSubmenu(activeSubMenu);
	if(submenuItem->submenu == actSubmenu){
		return false;
	}

	ContextMenu* submenu = dynamic_cast<ContextMenu*>(ev.getWindow());

	osgWidget::XYCoord pos = submenu->getAbsoluteOrigin();
	pos.y() += submenuItem->submenuItem->getOrigin().y() + submenuItem->submenuItem->getHeight() - submenuItem->submenu->getHeight();

	if(submenuItem->submenu->isSubmenuDirectionRight() == true){
		pos.x() += submenu->getWidth();
	}else{
		pos.x() -= submenuItem->submenu->getWidth();
	}

	submenuItem->submenu->showMenu(pos);
	return false;
}

bool ContextMenu::onSubmenuLeave(osgWidget::Event& ev){

	MenuSubmenu * submenuItem = static_cast<MenuSubmenu *>(ev.getData());

	submenuItem->submenuItem->setStyle("osg.contextmenu.submenuitem.normal");
    submenuItem->emptyWidget->setStyle("osg.contextmenu.submenuitem.normal");
    //if(getWindowManager() != nullptr && getWindowManager()->getStyleManager() != nullptr){
		getWindowManager()->getStyleManager()->applyStyles(submenuItem->submenuItem);
		getWindowManager()->getStyleManager()->applyStyles(submenuItem->emptyWidget);
	//}

	return false;
}

void ContextMenu::refreshMenuItemCheckedStyle(const MenuItem & menuItem, bool hovered){
	if(menuItem.checked == true){
		if(hovered == true){
			menuItem.checkedWidget->setStyle("osg.contextmenu.checked.hovered");
		}else{
			menuItem.checkedWidget->setStyle("osg.contextmenu.checked.normal");
		}
	}else{
		if(hovered == true){
			menuItem.checkedWidget->setStyle("osg.contextmenu.unchecked.hovered");
		}else{
			menuItem.checkedWidget->setStyle("osg.contextmenu.unchecked.normal");
		}
	}
}


const ContextMenu::MenuSubmenu & ContextMenu::findMenu(const std::string & path) const{
	return findMenu(parseNextMenuItems(path));
}

const ContextMenu::MenuItem & ContextMenu::findMenuItem(const std::string & path) const{
	return findMenuItem(parseNextMenuItems(path));
}

ContextMenu::MenuSubmenu & ContextMenu::findMenu(const std::string & path){
	return findMenu(parseNextMenuItems(path));
}

ContextMenu::MenuItem & ContextMenu::findMenuItem(const std::string & path){
	return findMenuItem(parseNextMenuItems(path));
}

template<class Collection>
bool ContextMenu::addMenuItem(const Collection & path, bool checked, const OnClickCallback & onClickCallback, const OnHoverCallback & onHoverCallback){
	return addMenuItem(path.begin(), path.end(), checked, onClickCallback, onHoverCallback);
}

template<class Iter>
bool ContextMenu::addMenuItem(Iter begin, Iter end, bool checked, const OnClickCallback & onClickCallback, const OnHoverCallback & onHoverCallback){
	if(begin == end){
		return false;
	}

	Iter prev = begin;
	ContextMenu* currentMenu = this;
	while(++begin != end){
		Submenus::iterator it = currentMenu->submenus.find(*prev);
		if(it == currentMenu->submenus.end()){
			//add new submenu
			MenuSubmenu submenuItem;
			//create new submenu
			submenuItem.submenu = new ContextMenu(currentMenu);

			//create widget
			submenuItem.submenuItem = new osgWidget::Label("submenu" + *prev);
			submenuItem.submenuItem->setStyle("osg.contextmenu.submenuitem.normal");

			submenuItem.emptyWidget = new osgWidget::Widget();
			submenuItem.emptyWidget->setStyle("osg.contextmenu.submenuitem.normal");

            if(getWindowManager() != nullptr){
                getWindowManager()->getStyleManager()->applyStyles(submenuItem.submenuItem);
                getWindowManager()->getStyleManager()->applyStyles(submenuItem.emptyWidget);
            }else{
                submenuItem.submenuItem->setFontSize(1);
            }

            submenuItem.submenuItem->setLabel(*prev);
            submenuItem.emptyWidget->setSize(submenuItem.submenuItem->getHeight(), submenuItem.submenuItem->getHeight());

			//configure widget events - onEnter, onLeave
			osgWidget::Callback * mc = new osgWidget::Callback(&ContextMenu::onSubmenuEnter, currentMenu, osgWidget::EVENT_MOUSE_ENTER, &(it->second));
			osgWidget::Callback * md = new osgWidget::Callback(&ContextMenu::onSubmenuLeave, currentMenu, osgWidget::EVENT_MOUSE_LEAVE, &(it->second));

			submenuItem.submenuItem->setEventMask(osgWidget::EVENT_MASK_MOUSE_MOVE);
			submenuItem.submenuItem->addCallback(mc);
			submenuItem.submenuItem->addCallback(md);

			submenuItem.emptyWidget->setEventMask(osgWidget::EVENT_MASK_MOUSE_MOVE);
			submenuItem.emptyWidget->addCallback(mc);
			submenuItem.emptyWidget->addCallback(md);

			unsigned int row = currentMenu->getNumRows();

			currentMenu->setNumRows(row + 1);

			if(row == 0){
				currentMenu->setNumColumns(2);
				//currentMenu->setColumnWeight(0,0);
				//currentMenu->setColumnWeight(1,1);
			}

			currentMenu->addWidget(submenuItem.submenuItem, row, 1);
			currentMenu->addWidget(new osgui::AspectRatioKeeper(submenuItem.emptyWidget), row, 0);

            //add widget
			it = currentMenu->submenus.insert({ *prev, submenuItem }).first;
		}

		currentMenu = it->second.submenu;
		prev = begin;
	}

	//try to add menu item
	if(currentMenu->items.find(*prev) == currentMenu->items.end()){
		//create widget

		MenuItem item;

		item.menuItem = new osgWidget::Label("subitem" + *prev);
		item.menuItem->setStyle("osg.contextmenu.item.normal");

		//configure widget events - onPush

		item.checked = checked;
		item.onClickCallback = onClickCallback;
		item.onHoverCallback = onHoverCallback;

		//checked operation
		item.checkedWidget = new osgWidget::Widget();
        refreshMenuItemCheckedStyle(item);

        if(getWindowManager() != nullptr){
            getWindowManager()->getStyleManager()->applyStyles(item.menuItem);
            getWindowManager()->getStyleManager()->applyStyles(item.checkedWidget);
        }else{
            item.menuItem->setFontSize(1);
        }

        item.menuItem->setLabel(*prev);
        item.checkedWidget->setSize(item.menuItem->getHeight(), item.menuItem->getHeight());

		//add widget
		Items::iterator it = currentMenu->items.insert({ *prev, item }).first;

		osgWidget::Callback * cc = new osgWidget::Callback(&ContextMenu::onItemPush, currentMenu, osgWidget::EVENT_MOUSE_PUSH, &(it->second));
		osgWidget::Callback * ec = new osgWidget::Callback(&ContextMenu::onItemEnter, currentMenu, osgWidget::EVENT_MOUSE_ENTER, &(it->second));
		//osgWidget::Callback * lc = new osgWidget::Callback(&ContextMenu::onItemLeave, currentMenu, osgWidget::EVENT_MOUSE_LEAVE, &(it->second));

		item.menuItem->setEventMask(osgWidget::EVENT_MASK_MOUSE_MOVE | osgWidget::EVENT_MASK_MOUSE_CLICK);
		item.menuItem->addCallback(cc);
		item.menuItem->addCallback(ec);
		//item.menuItem->addCallback(lc);

		item.checkedWidget->setEventMask(osgWidget::EVENT_MASK_MOUSE_MOVE | osgWidget::EVENT_MASK_MOUSE_CLICK);
		item.checkedWidget->addCallback(cc);
		item.checkedWidget->addCallback(ec);
		//item.checkedWidget->addCallback(lc);

		unsigned int row = currentMenu->getNumRows();

		currentMenu->setNumRows(row +1);

		if(row == 0){
			currentMenu->setNumColumns(2);
			//currentMenu->setColumnWeight(0,0);
			//currentMenu->setColumnWeight(1,1);
		}

		currentMenu->addWidget(item.menuItem, row, 1);
		currentMenu->addWidget(new osgui::AspectRatioKeeper(item.checkedWidget), row, 0);

		return true;
	}

	return false;
}

template<class Collection>
bool ContextMenu::removeMenuItem(const Collection & path){
	return removeMenuItem(path.begin(), path.end());
}

template<class Iter>
bool ContextMenu::removeMenuItem(Iter begin, Iter end){
	UTILS_FAIL("Nie zaimplementowano");

    MenuItem menuItem = findMenuItem(begin, end);

	if(menuItem.menuItem == nullptr){
		return false;
	}



	//TODO
	//implement
	return false;
}

template<class Collection>
bool ContextMenu::setMenuItemChecked(const Collection & path, bool checked){
	return setMenuItemChecked(path.begin(), path.end(), checked);
}

template<class Iter>
bool ContextMenu::setMenuItemChecked(Iter begin, Iter end, bool checked){
	MenuItem & menuItem = findMenuItem(begin, end);

	if(menuItem.menuItem == nullptr){
		return false;
	}

	if(menuItem.checked != checked){
		menuItem.checked = checked;

		refreshMenuItemCheckedStyle(menuItem);

        if(getWindowManager() != nullptr){
            getWindowManager()->getStyleManager()->applyStyles(menuItem.checkedWidget);
        }

		return true;
	}

	return false;
}

template<class Collection>
bool ContextMenu::setMenuItemOnClickCallback(const Collection & path, const OnClickCallback & callback){
	return setMenuItemOnClickCallback(path.begin(), path.end(), callback);
}

template<class Iter>
bool ContextMenu::setMenuItemOnClickCallback(Iter begin, Iter end, const OnClickCallback & callback){
	MenuItem & menuItem = findMenuItem(begin, end);
    // rev - nie da się porównywać funkcji!
	if(menuItem.menuItem == nullptr ) { //|| menuItem.onClickCallback == callback){
		return false;
	}

	menuItem.onClickCallback = callback;

	return true;
}

template<class Collection>
bool ContextMenu::setMenuItemOnHoverCallback(const Collection & path, const OnHoverCallback & callback){
	return setMenuItemOnHoverCallback(path.begin(), path.end(), callback);
}

template<class Iter>
bool ContextMenu::setMenuItemOnHoverCallback(Iter begin, Iter end, const OnHoverCallback & callback){
	MenuItem & menuItem = findMenuItem(begin, end);

    // rev - nie da się porownywać funkcji!
	if(menuItem.menuItem == nullptr) { // || menuItem.onHoverCallback == callback){
		return false;
	}

	menuItem.onHoverCallback = callback;

	return true;
}

template<class Collection>
bool ContextMenu::setMenuOnCloseCallback(const Collection & path, const OnCloseCallback & callback){
	return setMenuOnCloseCallback(path.begin(), path.end(), callback);
}

template<class Iter>
bool ContextMenu::setMenuOnCloseCallback(Iter begin, Iter end, const OnCloseCallback & callback){
	if(begin == end){
		this->closeMenuCallback = callback;
		return true;
	}

	MenuSubmenu & submenu = findMenu(begin, end);

	if(submenu.submenu == nullptr){
		return false;
	}

	submenu.submenu->closeMenuCallback = callback;

	return true;
}

template<class Collection>
const ContextMenu::OnClickCallback & ContextMenu::getMenuItemOnClickCallback(const Collection & path) const{
    // rev - co to jest callback?
	//return getMenuItemOnClickCallback(path.begin(), path.end(), callback);
	//return getMenuItemOnClickCallback(path.begin(), path.end(), nullptr);
	return getMenuItemOnClickCallback(path.begin(), path.end());
}

template<class Iter>
const ContextMenu::OnClickCallback & ContextMenu::getMenuItemOnClickCallback(Iter begin, Iter end) const{
	return findMenuItem(begin, end).onClickCallback;
}

template<class Collection>
const ContextMenu::OnHoverCallback & ContextMenu::getMenuItemOnHoverCallback(const Collection & path) const{
    // rev - co to jest callback?
	//return getMenuItemOnHoverCallback(path.begin(), path.end(), callback);
	//return getMenuItemOnHoverCallback(path.begin(), path.end(), nullptr);
	return getMenuItemOnHoverCallback(path.begin(), path.end());
}

template<class Iter>
const ContextMenu::OnHoverCallback & ContextMenu::getMenuItemOnHoverCallback(Iter begin, Iter end) const{
	return findMenuItem(begin, end).onHoverCallback;
}

template<class Collection>
const ContextMenu::OnCloseCallback & ContextMenu::getMenuOnCloseCallback(const Collection & path) const{
	// rev - co to jest callback?
	//return getMenuOnCloseCallback(path.begin(), path.end(), callback);
	//return getMenuOnCloseCallback(path.begin(), path.end(), nullptr);
	return getMenuOnCloseCallback(path.begin(), path.end());
}

template<class Iter>
const ContextMenu::OnCloseCallback & ContextMenu::getMenuOnCloseCallback(Iter begin, Iter end) const{
	return findMenu(begin, end).second->m_fCloseMenuCallback;
}

template<class Collection>
bool ContextMenu::getMenuItemChecked(const Collection & path) const{
	// rev - co to jest callback?
	//return getMenuItemChecked(path.begin(), path.end(), callback);
	//return getMenuItemChecked(path.begin(), path.end(), nullptr);
	return getMenuItemChecked(path.begin(), path.end());
}

template<class Iter>
bool ContextMenu::getMenuItemChecked(Iter begin, Iter end) const{
	return findMenuItem(begin, end).checked;
}

template<class Collection>
ContextMenu::MenuItem & ContextMenu::findMenuItem(const Collection & path){
	return findMenuItem(path.begin(), path.end());
}

template<class Iter>
ContextMenu::MenuItem & ContextMenu::findMenuItem(Iter begin, Iter end){
	if(std::distance(begin, end) == 1){
		//check only this level
		Items::iterator it = items.find(*begin);
		if(it != items.end()){
			return it->second;
		}
	}else{
		Iter tmpEnd = end;
		--tmpEnd;
		MenuSubmenu submenu = findMenu(begin, tmpEnd);

		if(submenu.submenu != nullptr){
			Items::iterator it = submenu.submenu->items.find(*tmpEnd);
			if(it != submenu.submenu->items.end()){
				return it->second;
			}
		}
	}

	emptyMenuItem = constEmptyMenuItem;
	return emptyMenuItem;
}

template<class Collection>
const ContextMenu::MenuItem & ContextMenu::findMenuItem(const Collection & path) const{
	return findMenuItem(path.begin(), path.end());
}

template<class Iter>
const ContextMenu::MenuItem & ContextMenu::findMenuItem(Iter begin, Iter end) const{

	if(std::distance(begin, end) == 1){
		//check only this level
		Items::const_iterator it = items.find(*begin);
		if(it != items.end()){
			return it->second;
		}
	}else{
		Iter tmpEnd = end;
		--tmpEnd;
		MenuSubmenu submenu = findMenu(begin, tmpEnd);

		if(submenu.submenu != nullptr){
			Items::const_iterator it = submenu.submenu->items.find(*tmpEnd);
			if(it != submenu.submenu->items.end()){
				return it->second;
			}
		}
	}

	return constEmptyMenuItem;
}

template<class Collection>
ContextMenu::MenuSubmenu & ContextMenu::findMenu(const Collection & path){
	return findMenu(path.begin(), path.end());
}

template<class Iter>
ContextMenu::MenuSubmenu & ContextMenu::findMenu(Iter begin, Iter end){
	emptyMenuSubmenu = constEmptyMenuSubmenu;

	if(begin == end){
		return emptyMenuSubmenu;
	}

	Submenus::iterator it;
	osg::observer_ptr<ContextMenu> currentMenu(this);
	while(++begin != end){
		it = currentMenu->submenus.find(*begin);
		if(it != currentMenu->submenus.end()){
			currentMenu = it->second.submenu;
		}else{
			return emptyMenuSubmenu;
		}
	}

	return it->second;
}

template<class Collection>
const ContextMenu::MenuSubmenu & ContextMenu::findMenu(const Collection & path) const{
	return findMenu(path.begin(), path.end());
}

template<class Iter>
const ContextMenu::MenuSubmenu & ContextMenu::findMenu(Iter begin, Iter end) const{
	if(begin == end){
		return constEmptyMenuSubmenu;
	}

	Submenus::const_iterator it;
	osg::observer_ptr<const ContextMenu> currentMenu(this);
	while(++begin != end){
		it = currentMenu->submenus.find(*begin);
		if(it != currentMenu->submenus.end()){
			currentMenu = it->second.submenu.get();
		}else{
			return constEmptyMenuSubmenu;
		}
	}

	return it->second;
}

const std::string& ContextMenu::getPathSeparators() const
{
    return *pathSeparator;
}

void ContextMenu::setPathSeparators( const std::string& separators )
{
    *(this->pathSeparator) = separators;
}

const std::string & ContextMenu::getDefaultPathSeparator(){
    return defaultPathSeparator;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////
