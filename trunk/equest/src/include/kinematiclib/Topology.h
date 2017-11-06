/********************************************************************
	created:  2015/02/24	18:59:39
	filename: Topology.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATIC__TOPOLOGY_H__
#define __HEADER_GUARD_KINEMATIC__TOPOLOGY_H__

#include <string>
#include <treecontainerlib/NodeT.h>
#include <utils/SmartPtr.h>

namespace kinematic
{
	//! Struktur opisuj¹ca wêz³y topologii 
	struct TopologyData
	{
		//! Nazwa wêz³a
		std::string name;
	};

	//! Typ wêz³a topologii szkieletu
	using TopologyNode = treeContainer::NodeT<TopologyData>;

	DEFINE_SMART_POINTERS(TopologyNode);
}

#endif	// __HEADER_GUARD_KINEMATIC__TOPOLOGY_H__