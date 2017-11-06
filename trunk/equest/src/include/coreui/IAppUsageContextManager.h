/********************************************************************
    created:  2011/12/16
    created:  16:12:2011   8:59
    filename: IAppUsageContextManager.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD___IAPPUSAGECONTEXTMANAGER_H__
#define HEADER_GUARD___IAPPUSAGECONTEXTMANAGER_H__

#include <coreui/IAppUsageContext.h>
#include <queue>
#include <vector>
#include <set>
#include <algorithm>

namespace coreUI {
class IAppUsageContextManager
{

public:
    typedef std::vector<IAppUsageContextPtr> ContextChain;

public:
    virtual ~IAppUsageContextManager() {}

    void addContext(const IAppUsageContextPtr & context, const IAppUsageContextPtr & parerntContext = IAppUsageContextPtr())
    {
        if(context == nullptr){
            throw loglib::runtime_error("Trying to register uninitialized context");
        }

        if(context == parerntContext){
            throw loglib::runtime_error("Trying to create context loop");
        }

        if(parerntContext != nullptr && contexts.find(parerntContext) == contexts.end()){
            throw loglib::runtime_error("Trying to register context within unregistered parent context");
        }

        auto contextIT = contexts.find(context);

        if(contextIT == contexts.end()){
            context->parent = parerntContext;
            contexts.insert(context);
        }else if(parerntContext != context->parentContext()){
            auto parent = context->parentContext();
            if(parent != nullptr){
                parent->childContexts.erase(context);
            }

            context->parent = parerntContext;
        }

        if(parerntContext != nullptr){
            parerntContext->childContexts.insert(context);
        }
    }

    void addWidgetToContext(const IAppUsageContextPtr & context, QWidget * contextWidget)
    {
        if(context == nullptr){
            throw loglib::runtime_error("Trying to register uninitialized context");
        }

        if(contextWidget == nullptr){
            throw loglib::runtime_error("Trying to register uninitialized widget");
        }

        auto contextIT = contexts.find(context);

        if(contextIT == contexts.end()){
            throw loglib::runtime_error("Trying to register widget to unregistered context");
        }

        auto contextWidgetIT = widgets.find(contextWidget);

        if(contextWidgetIT != widgets.end()){
            throw loglib::runtime_error("Trying to register already registered widget");
        }

        widgetContexts[contextWidget] = context;
        widgets.insert(contextWidget);

        context->registerContextWidget(contextWidget);
    }

    void removeContext(const IAppUsageContextPtr & context)
    {
        auto contextIT = contexts.find(context);

        if(contextIT == contexts.end()){
            throw loglib::runtime_error("Trying to remove unregistered context");
        }

        if(context->isActive() == true){
            limitCurrentChainToPosition(contextPositionInCurrentChain(context)-1);
        }

        //wyrejestrowujemy wszystkie widgety skojarzone z tym contextem i jego dziecmi
        std::set<IAppUsageContextPtr> contextsToErase;
        getAllSubContexts(context, contextsToErase);

        for(auto it = contextsToErase.begin(); it != contextsToErase.end(); ++it){
            for(auto widgetIT = (*it)->contextWidgets.begin(); widgetIT != (*it)->contextWidgets.end(); ++widgetIT){
                widgetContexts.erase(*widgetIT);
                widgets.erase(*widgetIT);
            }
            (*it)->clearContextWidgets();
            contexts.erase(*it);
        }
    }

    void removeContext(QWidget * contextWidget)
    {
        auto it = widgetContexts.find(contextWidget);

        if(it == widgetContexts.end()){
            throw loglib::runtime_error("Trying to remove unregistered context widget");
        }

        if(it->second->isActive() == true && it->second->getCurrentContextWidget() == contextWidget){
            refreshContext(it->second);
        }

        widgets.erase(it->first);
        it->second->unregisterContextWidget(contextWidget);
        widgetContexts.erase(it);
    }

    bool isContextWidget(QWidget * candidate) const
    {
        return widgets.find(candidate) != widgets.end();
    }

    QWidget * getParentContextWidget(QWidget * candidate) const
    {
        QWidget * ret = nullptr;

        if(candidate != nullptr && widgets.empty() == false){

            std::set<QWidget*> parentWidgets;
            std::vector<QWidget*> orderedParentWidgets;

            while(candidate->parentWidget() != nullptr){
                parentWidgets.insert(candidate->parentWidget());
                orderedParentWidgets.push_back(candidate->parentWidget());
                candidate = candidate->parentWidget();
            }

            std::vector<QWidget*> intersection((std::min)(widgets.size(), parentWidgets.size()));

            auto intIT = std::set_intersection(parentWidgets.begin(), parentWidgets.end(), widgets.begin(), widgets.end(), intersection.begin());

            //cos znaleziono!!
            if(intIT != intersection.begin()){
                // rev - intersection tworzone tutaj, ale tez brane z przestrzeni rodzica ?!
                /*std::set<QWidget*> intersection(intersection.begin(), intIT);
                for(auto it = orderedParentWidgets.begin(); it != orderedParentWidgets.end(); ++it){
                    if(intersection.find(*it) != intersection.end()){
                        ret = *it;
                        break;
                    }
                }*/
                std::set<QWidget*> intersection2(intersection.begin(), intIT);
                for(auto it = orderedParentWidgets.begin(); it != orderedParentWidgets.end(); ++it){
                    if(intersection2.find(*it) != intersection2.end()){
                        ret = *it;
                        break;
                    }
                }
            }
        }

        return ret;
    }

    bool isCurrentContextWidget(QWidget * candidate) const
    {
        auto it = widgetContexts.find(candidate);

        if(it != widgetContexts.end() && it->second->getCurrentContextWidget() == candidate){
            return true;
        }

        return false;
    }

    QWidget * getCurrentContextWidget() const
    {
        if(currentContextChain.empty() == false){
            return currentContextChain.back()->getCurrentContextWidget();
        }

        return nullptr;
    }

    IAppUsageContextPtr getCurrentContext() const
    {
        if(currentContextChain.empty() == false){
            return currentContextChain.back();
        }

        return IAppUsageContextPtr();
    }

    void setCurrentContext(const IAppUsageContextPtr & context, QWidget * contextWidget = nullptr)
    {
        //verify context and widget

        auto contextIT = contexts.find(context);

        if(contextIT == contexts.end()){
            throw loglib::runtime_error("Trying to set unregistered context");
        }

        if(contextWidget != nullptr && context->contextWidgets.find(contextWidget) == context->contextWidgets.end()){
            throw loglib::runtime_error("Trying to set widget not registered within context");
        }

        innerSetCurrentContext(context, contextWidget);
    }

    void setCurrentContext(QWidget * contextWidget)
    {
        if(contextWidget == nullptr){
            //chowamy caly łańcuch kontekstów
            if(currentContextChain.empty() == false){
                limitCurrentChainToPosition(-1);
                ContextChain().swap(currentContextChain);
            }

        }else if(contextWidget == getCurrentContextWidget()){

            refreshContext(currentContextChain.back(), contextWidget);

        }else {

            auto it = widgetContexts.find(contextWidget);

            if(it == widgetContexts.end()){
                throw loglib::runtime_error("Trying to activate unregistered context widget");
            }

            innerSetCurrentContext(it->second, contextWidget);
        }
    }

    const ContextChain & getCurrentContextChain() const
    {
        return currentContextChain;
    }

private:

    static void refreshContext(const IAppUsageContextPtr & context, QWidget * nextContextWidget = nullptr)
    {
        context->tryDeactivate(nextContextWidget, true);
        context->tryActivate(nextContextWidget);
    }

    void innerSetCurrentContext(const IAppUsageContextPtr & context = IAppUsageContextPtr(), QWidget * contextWidget = nullptr)
    {

        if(context == nullptr){
            //chowamy caly łańcuch kontekstów
            if(currentContextChain.empty() == false){
                limitCurrentChainToPosition(-1);
                ContextChain().swap(currentContextChain);
            }
        }else{
            //sprawdzamy czy kontekst nie jest już aktywny - jeśli tak to wszystkie nizsze od niego są dezaktywowane
            int pos = contextPositionInCurrentChain(context);

            if(pos > -1){
                limitCurrentChainToPosition(pos);

                //przeładowanie kontekstu / odświeżenie, dla widgetów które zmieniają stan ale nie ze względu na inne widgety dzieci (np. QTreeWidget na zmianę aktualnego wiersza)
                refreshContext(currentContextChain.back(), contextWidget);
            }else{
                //nowy kontest wydaje się niezalezny od aktualnie aktywnych - szukamy zalezności

                //szukamy zalezności względem aktualnego lancucha idac w gore od zadanego kontekstu
                auto parent = context->parentContext();
                ContextChain newContextChain;
                newContextChain.push_back(context);
                while(parent != nullptr){
                    if(parent->isActive() == true){
                        break;
                    }else{
                        newContextChain.push_back(parent);
                        parent = parent->parentContext();
                    }
                }

                if(parent != nullptr){
                    //mamy punkt przeciecia nowego kontekstu z aktualnie aktywnymi

                    //ograniczamy aktualny lancuch do punktu przeciecia
                    limitCurrentChainToPosition(contextPositionInCurrentChain(parent));

                    //jeśli ostatni ma widget to musimy go przelączyć
                    auto tmpCurrent = currentContextChain.back();
                    if(tmpCurrent->getCurrentContextWidget() != nullptr){

                        refreshContext(tmpCurrent);
                    }

                }else{
                    //mamy calkowicie nowy kontekst - zamykamy caly obecny i budujemy od zera nowy, mamy jego przebieg w newContext
                    limitCurrentChainToPosition(-1);
                }

				std::reverse(newContextChain.begin(), newContextChain.end());

                int lastPos = currentContextChain.size();

                currentContextChain.insert(currentContextChain.end(), newContextChain.begin(), newContextChain.end());

                int newLastPos = currentContextChain.size()-1;

                for(int i = lastPos; i < newLastPos; ++i){
                    currentContextChain[i]->activate();
                }

                bool active = currentContextChain.back()->tryActivate(contextWidget);
            }
        }
    }

    void limitCurrentChainToPosition(int limitPos)
    {
        for(int pos = currentContextChain.size() - 1; pos > limitPos; --pos){
            currentContextChain[pos]->tryDeactivate();
            currentContextChain.pop_back();
        }
    }

    void getAllSubContexts(const IAppUsageContextPtr & context, std::set<IAppUsageContextPtr> & contexts)
    {
        contexts.insert(context);
        for(auto it = context->childContexts.begin(); it != context->childContexts.end(); ++it){
            getAllSubContexts(*it, contexts);
        }
    }

protected:

    int contextPositionInCurrentChain(QWidget * contextWidget)
    {
        int ret = -1;
        for(unsigned int i = 0; i < currentContextChain.size(); ++i){
            if(currentContextChain[i]->getCurrentContextWidget() == contextWidget){
                ret = i;
                break;
            }
        }

        return ret;
    }

    int contextPositionInCurrentChain(const IAppUsageContextPtr & context)
    {
        int ret = -1;
        auto it = std::find(currentContextChain.begin(), currentContextChain.end(), context);

        if(it != currentContextChain.end()){
            ret = std::distance(currentContextChain.begin(), it);
        }

        return ret;
    }

private:
    std::set<IAppUsageContextPtr> contexts;
    ContextChain currentContextChain;
    std::map<QWidget*, IAppUsageContextPtr> widgetContexts;
    std::set<QWidget*> widgets;
};
}
#endif //HEADER_GUARD___IAPPUSAGECONTEXTMANAGER_H__
