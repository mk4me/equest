/********************************************************************
	created:	2014/05/22
	created:	22:5:2014   10:07
	filename: 	ValueLayer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__VALUELAYER_H__
#define HEADER_GUARD_DICOM__VALUELAYER_H__

namespace dicom {
	//! Interfejs powsta³, by da³o siê przeiterowaæ po layerach z wartoœciami
	//! nie znaj¹c implementacji i typów z którymi s¹ skompilowane 
	class IValueLayer : public ILayerItem
	{
	public:
		IValueLayer(const int val) : ILayerItem(val) {}
		virtual ~IValueLayer() {}
		//! \return wartoœæ przedstawiona w formie stringu
		virtual std::string valueAsString() const = 0;
	};
	DEFINE_SMART_POINTERS(IValueLayer);
}
#endif
