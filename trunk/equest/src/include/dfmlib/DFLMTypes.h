#ifndef HEADER_GUARD__DFMTYPES_H__
#define HEADER_GUARD__DFMTYPES_H__

#include <utils/SmartPtr.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
	////////////////////////////////////////////////////////////////////////////////

	class ConnectionManager;
	class Node;
	class DFNode;
	class DFSourceNode;
	class Connection;
	class Pin;
	class DFPin;
	class Model;
	class DFModel;

	typedef utils::shared_ptr<Node> NPtr;
	typedef utils::shared_ptr<const Node> CNPtr;
	typedef utils::weak_ptr<Node> WNPtr;
	typedef utils::weak_ptr<const Node> CWNPtr;

	typedef utils::shared_ptr<DFNode> DFNPtr;
	typedef utils::shared_ptr<const DFNode> CDFNPtr;
	typedef utils::weak_ptr<DFNode> WDFNPtr;
	typedef utils::weak_ptr<const DFNode> CWDFNPtr;

	typedef utils::shared_ptr<DFSourceNode> DFSNPtr;
	typedef utils::shared_ptr<const DFSourceNode> CDFSNPtr;
	typedef utils::weak_ptr<DFSourceNode> WDFSNPtr;
	typedef utils::weak_ptr<const DFSourceNode> CWDFSNPtr;

	typedef utils::shared_ptr<Connection> ConnPtr;
	typedef utils::shared_ptr<const Connection> CConnPtr;
	typedef utils::weak_ptr<Connection> WConnPtr;
	typedef utils::weak_ptr<const Connection> CWConnPtr;

	typedef utils::shared_ptr<Pin> PinPtr;
	typedef utils::shared_ptr<const Pin> CPinPtr;
	typedef utils::weak_ptr<Pin> WPinPtr;
	typedef utils::weak_ptr<const Pin> CWPinPtr;

	typedef utils::shared_ptr<DFPin> DFPinPtr;
	typedef utils::shared_ptr<const DFPin> CDFPinPtr;
	typedef utils::weak_ptr<DFPin> WDFPinPtr;
	typedef utils::weak_ptr<const DFPin> CWDFPinPtr;

	typedef utils::shared_ptr<Model> MPtr;
	typedef utils::shared_ptr<const Model> CMPtr;
	typedef utils::weak_ptr<Model> WMPtr;
	typedef utils::weak_ptr<const Model> CWMPtr;

	typedef utils::shared_ptr<DFModel> DFMPtr;
	typedef utils::shared_ptr<const DFModel> CDFMPtr;
	typedef utils::weak_ptr<DFModel> WDFMPtr;
	typedef utils::weak_ptr<const DFModel> CWDFMPtr;
}

#endif
