#ifndef HEADER_GUARD__DFM_TESTPIN_H__
#define HEADER_GUARD__DFM_TESTPIN_H__

#include <dfmlib/Node.h>

class TestNode :
	public dflm::Node
{
public:
	TestNode(const std::string & nodeName);
	virtual ~TestNode(void);
};

#endif
