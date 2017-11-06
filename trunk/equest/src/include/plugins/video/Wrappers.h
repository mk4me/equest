/********************************************************************
	created:  2011/02/04
	created:  4:2:2011   13:02
	filename: DataWrappers.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDEO__WRAPPERS_H__
#define __HEADER_GUARD_VIDEO__WRAPPERS_H__

#include <utils/PtrPolicyStd.h>
#include <utils/ClonePolicies.h>
#include <corelib/BaseDataTypes.h>
#include <vidlib/osg/VideoImageStream.h>
#include <vidlib/osg/OsgAdapter.h>
#include <vidlib/FFmpegVideoStream.h>
#include <dataaccessorlib/Accessors.h>
#include <dataaccessorlib/BoundedArgumentsFeature.h>
#include <dataaccessorlib/UniformArgumentsFeature.h>
#include <dataaccessorlib/AccessorsCollection.h>


typedef osg::ref_ptr<vidlib::VideoImage> VideoImageOsgPtr;
typedef vidlib::OsgStream VideoStream;
typedef osg::ref_ptr<VideoStream> VideoStreamPtr;
typedef osg::ref_ptr<const VideoStream> VideoStreamConstPtr;
typedef dataaccessor::IIndependentDiscreteAccessorT<VideoImageOsgPtr, float> VideoChannelReaderInterface;

class VideoChannel : public VideoChannelReaderInterface
{
public:
	enum View { Unknown, Left, Right, Front, Back };

private:
	mutable VideoStreamPtr videoStream;
	View type;
    float samplesPerSecond;
    float invSamplesPerSecond;
    std::string name;

public:
	VideoChannel(VideoStreamPtr video) : videoStream(video), samplesPerSecond(static_cast<float>(videoStream->getFramerate()))
	{
		attachFeature(dataaccessor::IFeaturePtr(new dataaccessor::BoundedArgumentsFeature<float>(0.0, video->getDuration())));
		attachFeature(dataaccessor::IFeaturePtr(new dataaccessor::UniformArgumentsFeature<float>(1.0f / samplesPerSecond)));        
    }

	virtual ~VideoChannel()
	{

	}

	argument_type getLength() const
	{
		return videoStream->getDuration();
	}
	//! \return Liczba punktów pomiarowych.
	virtual size_type size() const override
	{
		return videoStream->getFrameCount();
	}    

    //! \param idx Indeks próbki
    //! \return Wartość czasu dla danego indeksu
    virtual argument_type argument(const size_type idx) const override
    {
        UTILS_ASSERT((idx >= 0),"Błędny indeks dla kanały - musi być większy lub równy 0");
        return videoStream->getFrameDuration() * idx;
    }

    //! \param idx Indeks próbki
    //! \return Wartość próbki dla danego indeksu
    virtual value_type value(const size_type idx) const override
    {
        videoStream->setTime(idx * videoStream->getFrameDuration());
        return videoStream->getImage(vidlib::PixelFormatARGB);
    }

	VideoStreamPtr getVideoStream() {
		return videoStream;
	}

	VideoStreamConstPtr getVideoStream() const {
		return videoStream;
	}

	VideoChannel::View getType() const { return type; }

protected:
	//! \param time
	//! \return Zinterpolowana wartość dla zadanego czasu.
	virtual value_type innerGetValue(argument_type time) const
	{
		videoStream->setTime(time);
		return videoStream->getImage(vidlib::PixelFormatARGB);
	}
};
typedef utils::shared_ptr<VideoChannel> VideoChannelPtr;
typedef utils::shared_ptr<const VideoChannel> VideoChannelConstPtr;

//utils::DataChannelTimeAccessor<VideoChannel::point_type, VideoChannel::time_type, utils::DiscreteInterpolator, utils::BorderExtrapolator>

class VideoCollection : public dataaccessor::AccessorsCollection<VideoChannel>
{

};

typedef utils::shared_ptr<VideoCollection> VideoCollectionPtr;
typedef utils::shared_ptr<const VideoCollection> VideoCollectionConstPtr;

DEFINE_WRAPPER(VideoChannel, utils::PtrPolicyStd, utils::ClonePolicyForbidden);
DEFINE_WRAPPER(vidlib::Picture, utils::PtrPolicyStd, utils::ClonePolicyForbidden);
DEFINE_WRAPPER(VideoStream, utils::PtrPolicyOSG, utils::ClonePolicyVirtualCloneMethod);

#endif  // __HEADER_GUARD_VIDEO__WRAPPERS_H__
