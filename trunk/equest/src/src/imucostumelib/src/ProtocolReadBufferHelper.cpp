#include <imucostumelib/ProtocolReadBufferHelper.h>
#include <imucostumelib/CostumeCANopenIO.h>
#include <utils/Debug.h>

using namespace imuCostume;

ProtocolReadBufferHelper::CANopenFrameProxy::CANopenFrameProxy(const uint8_t * base)
	: base_(base)
{

}


ProtocolReadBufferHelper::CANopenFrameProxy::~CANopenFrameProxy()
{

}

ProtocolReadBufferHelper::HeaderProxy ProtocolReadBufferHelper::CANopenFrameProxy::header() const
{	
	ProtocolReadBufferHelper::HeaderProxy ret;
	ret.base = base_;
	return ret;
}

const uint8_t * ProtocolReadBufferHelper::CANopenFrameProxy::base() const
{
	return base_;
}

const uint8_t * ProtocolReadBufferHelper::CANopenFrameProxy::data() const
{
	return base_ + 2;
}

bool ProtocolReadBufferHelper::CANopenFrameProxy::empty() const
{
	return base_ == nullptr;
}

ProtocolReadBufferHelper::ProtocolReadBufferHelper()
{

}

ProtocolReadBufferHelper::ProtocolReadBufferHelper(const std::vector<const uint8_t*> & frameBasis)
	: frameBasis(frameBasis)
{

}

ProtocolReadBufferHelper::ProtocolReadBufferHelper(ProtocolReadBufferHelper & Other)
	: frameBasis(Other.frameBasis)
{

}

ProtocolReadBufferHelper::ProtocolReadBufferHelper(ProtocolReadBufferHelper && Other)
	: frameBasis(std::move(Other.frameBasis))
{

}

ProtocolReadBufferHelper ProtocolReadBufferHelper::create(const void * buffer, const uint16_t length)
{
	std::vector<const uint8_t*> frameBasis;
	if (length > 0){
		//pomijam nag³ówek danych - seqNumber + typ ramki!!
		auto pos = static_cast<const uint8_t*>(buffer);
		const auto end = pos + length;

		//! wyci¹gam kolejne pozycje ramek
		while (pos < end){
			frameBasis.push_back(pos);
			const auto length = (2 + (*(pos + 1) >> 4));
			UTILS_ASSERT(length <= CANopenFrame::SizeLimits::MaxSize);
			pos += length;
		}
	}

	return ProtocolReadBufferHelper(frameBasis);
}

std::vector<const uint8_t*>::size_type ProtocolReadBufferHelper::size() const
{
	return frameBasis.size();
}

bool ProtocolReadBufferHelper::empty() const
{
	return frameBasis.empty();
}

ProtocolReadBufferHelper::CANopenFrameProxy ProtocolReadBufferHelper::operator[](const uint16_t idx) const
{
	return CANopenFrameProxy(frameBasis[idx]);
}