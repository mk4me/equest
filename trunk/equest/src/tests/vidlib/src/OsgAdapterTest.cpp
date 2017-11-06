#include <cppunit/extensions/HelperMacros.h>
#include <vidlib/Config.h>

#ifdef VIDLIB_ENABLE_OSG
#include <utils/Utils.h>
UTILS_DISABLE_DLL_INTERFACE_WARNING
#include <vidlib/osg/OsgAdapter.h>
#include <vidlib/FFmpegVideoStream.h>
#include <osg/Node>

#define FILE_PATH "video/1.avi"

using namespace vidlib;

class OsgAdapterTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(OsgAdapterTest);
    CPPUNIT_TEST(testInstances);
    CPPUNIT_TEST(testSetMaxWidth);
    CPPUNIT_TEST_SUITE_END();

    typedef OsgStream Stream;
    typedef osg::ref_ptr<Stream> StreamPtr;
    typedef osg::ref_ptr<const Stream> StreamConstPtr;
    typedef osg::ref_ptr<VideoImage> VideoImagePtr;
    typedef osg::ref_ptr<const VideoImage> VideoImageConstPtr;

public:
    void testInstances()
    {
        StreamPtr stream(new Stream(new FFmpegVideoStream(FILE_PATH)));
        stream->setTime(0);
        VideoImagePtr img0 = stream->getImage(PixelFormatRGB24);
        VideoImagePtr img1 = stream->getImage(PixelFormatRGB24);
        VideoImagePtr img2 = stream->getImage(PixelFormatRGB24);
        CPPUNIT_ASSERT(img0 == img1 && img1 == img2);

        img0 = stream->getImage(PixelFormatRGB24);
        img1 = stream->getImage(PixelFormatYV12);
        img2 = stream->getImage(PixelFormatBGRA);
        CPPUNIT_ASSERT(img0 != img1 && img1 != img2 && img0 != img2);
        CPPUNIT_ASSERT(img0->getStream() == img1->getStream());
    }

    void testSetMaxWidth()
    {
        StreamPtr stream(new Stream(new FFmpegVideoStream(FILE_PATH)));
        stream->setTime(0);
        VideoImagePtr img = stream->getImage(PixelFormatRGB24);
        CPPUNIT_ASSERT(img->getMaxWidth() == stream->getWidth());

        {
            osg::ref_ptr<osg::Node> node = new osg::Node();
            img->setMaxWidth(node, 16);
            CPPUNIT_ASSERT(img->getMaxWidth() == 16 && img->s() <= 16);
            node = nullptr; img->refresh();
            CPPUNIT_ASSERT(img->getMaxWidth() == stream->getWidth() && img->s() == stream->getWidth());
        }
        
        {
            osg::ref_ptr<osg::Node> node0 = new osg::Node();
            osg::ref_ptr<osg::Node> node1 = new osg::Node();
            img->setMaxWidth(node0, 16);
            img->setMaxWidth(node1, 24);
            CPPUNIT_ASSERT(img->getMaxWidth() == 24 && img->s() <= 24);
            img->removeMaxWidth(node1);
            CPPUNIT_ASSERT(img->getMaxWidth() == 16 && img->s() <= 16);
            node0 = nullptr; img->refresh();
            CPPUNIT_ASSERT(img->s() == stream->getWidth());
        }

        {
            osg::ref_ptr<osg::Node> node0 = new osg::Node();
            osg::ref_ptr<osg::Node> node1 = new osg::Node();
            img->setMaxWidth(node0, 16);
            img->setMaxWidth(node1, 24);
            img->setMaxWidth(node1, 8);
            CPPUNIT_ASSERT(img->getMaxWidth() == 16 && img->s() <= 16);
        }


    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( OsgAdapterTest );

#endif // VIDLIB_ENABLE_OSG
