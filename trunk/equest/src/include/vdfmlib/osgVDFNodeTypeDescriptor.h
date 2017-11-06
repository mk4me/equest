#ifndef HEADER_GUARD__VDFNODETYPEDESCRIPTOR_H__
#define HEADER_GUARD__VDFNODETYPEDESCRIPTOR_H__

#include <osg/Image>
#include <vdfmlib/osgVDFBaseNode.h>
#include <dfmlib/DFLMTypes.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

class osgVDFNodeTypeDescriptor
{
public:
	~osgVDFNodeTypeDescriptor(void);

	virtual osgVDFBaseNode::NodeType getNodeType() const = 0;
	virtual osg::Image * getButtonImage() const = 0;
	virtual const std::string & getButtonText() const = 0;
	virtual osg::Image * getNodeDefaultImage() const = 0;
	virtual dflm::NPtr createNode() const = 0;
	virtual const std::string & getName() const = 0;

protected:
	osgVDFNodeTypeDescriptor(void);

};

}

#endif
