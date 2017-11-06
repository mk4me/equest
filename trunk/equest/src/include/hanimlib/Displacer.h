/********************************************************************
	created:  2014/12/05	14:11:19
	filename: Displacer.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HANIMLIB__DISPLACER_H__
#define __HEADER_GUARD_HANIMLIB__DISPLACER_H__

#include <hanimlib/Object.h>
#include <vector>

namespace hAnim
{
	struct Displacer : public Object
	{
		Displacer();
		Displacer(const Displacer & other);
		Displacer(Displacer && other);
		virtual ~Displacer();

		std::vector<int> coordIndex;
		std::vector<float> displacements;
		float weight;
	};
}

#endif	// __HEADER_GUARD_HANIMLIB__DISPLACER_H__