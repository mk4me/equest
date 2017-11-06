#include "btkWriteTest.h"
#include <btkAcquisitionFileReader.h>
#include <btkMergeAcquisitionFilter.h>
#include <btkAcquisitionUnitConverter.h>
#include <btkAcquisition.h>
#include <btkAcquisitionFileWriter.h>
#include "c3dlib/C3DWriter.h"
#include "c3dlib/C3DTypes.h"


CPPUNIT_TEST_SUITE_REGISTRATION(btkWriteTest);

void btkWriteTest::testBasicWrite()
{
	// Writer
	btk::AcquisitionFileWriter::Pointer C3DWriter = btk::AcquisitionFileWriter::New();
	btk::Acquisition::Pointer acq = btk::Acquisition::New();
	acq->Init(10, 200, 20, 10); // 10 points, 200 frames, 20 analog channels with a number of frames 10 times greater
	acq->SetPointFrequency(100.0); // 100 Hz. Analog's frequency is automatically set to 1000 Hz.

	// Link the processes together to create the pipeline
	C3DWriter->SetInput(acq); // From the converter to the C3D Writer;

	// No option for the merger
	// Use the default units for the converter: millimeter (position), degree (angle), newton (force), newton-millimeter (moment) and watt (power).
	C3DWriter->SetFilename("OneAcquisition.c3d");

	// To execute the pipeline, it is only necessary to update the last process.
	// The pipeline will check if the previous are already updated an do it if necessary.
	C3DWriter->Update();

	// Now check in your folder and you will find a new C3D file!

}

void btkWriteTest::testScalarChannelWrite()
{
	//c3dlib::ScalarChannelPtr channel = utils::make_shared<c3dlib::ScalarChannel>(100);
	//for (int i = 0; i < 10000; ++i) {
	//	channel->addPoint(static_cast<float>(sin(i / 100.0f)));
	//}
	//
	//c3dlib::C3DWriter writer;
	//writer.addScalarChannel(channel);
	//writer.write("ScalarTest.c3d");
}
