#include "C3DPCH.h"
#include <plugins/c3d/EventSerieBase.h>

c3dlib::C3DEventsCollection::Context EventSerieBase::getContext() const
{
	return context;
}

void EventSerieBase::setContext(c3dlib::C3DEventsCollection::Context val)
{
	context = val;
}