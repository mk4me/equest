#include <c3dlib/C3DWriter.h>
#include "btkAcquisitionFileWriter.h"
#include "btkAcquisition.h"
#include "btkAnalog.h"
#include "btkPoint.h"
#include <algorithm>

void c3dlib::C3DWriter::addScalarChannel(ScalarChannelReaderInterfaceConstPtr channel)
{
	scalars.push_back(channel);
}

void c3dlib::C3DWriter::addVectorChannel(VectorChannelReaderInterfaceConstPtr channel)
{
	vectors.push_back(channel);
}

void c3dlib::C3DWriter::write(const std::string& path)
{
	int maxA = 0;
	int maxP = 0;
	for (auto& scalar : scalars) {
		maxA = (std::max)(maxA, (int)scalar->size());
	}
	for (auto& vector : vectors) {
		maxP = (std::max)(maxP, (int)vector->size());
	}

	// Writer
	btk::AcquisitionFileWriter::Pointer C3DWriter = btk::AcquisitionFileWriter::New();
	btk::Acquisition::Pointer acq = btk::Acquisition::New();
	acq->Init(vectors.size(), maxP == 0 ? maxA / 10 : maxP, scalars.size(), 10); // 10 points, 200 frames, 20 analog channels with a number of frames 10 times greater
	

	for (int i = 0; i < scalars.size(); ++i) {	
		auto scalar = scalars[i];
		auto df = scalar->feature<dataaccessor::DescriptorFeature>();
		std::string name = df != nullptr && df->name().empty() == false ? df->name() : "Unnamed scalar " + std::to_string(i);
		btk::Analog::Pointer analog = btk::Analog::New(name);
		int count = scalar->size();
		analog->SetFrameNumber(count);
		for (int j = 0; j < count; ++j) {
			analog->SetDataSlice(j, scalar->value(j));
		}
		acq->SetAnalog(i, analog);
	}

	for (int i = 0; i < vectors.size(); ++i) {
		auto vector = vectors[i];
		auto df = vector->feature<dataaccessor::DescriptorFeature>();
		std::string name = df != nullptr && df->name().empty() == false ? df->name() : "Unnamed vector " + std::to_string(i);
		btk::Point::Pointer point = btk::Point::New(name);
		int count = vector->size();
		point->SetFrameNumber(count);
		for (int j = 0; j < count; ++j) {
			auto v = vector->value(j);
			point->SetDataSlice(j, v.x(), v.y(), v.z());
		}
		acq->SetPoint(i, point);
	}


	// Link the processes together to create the pipeline
	C3DWriter->SetInput(acq); // From the converter to the C3D Writer;

	// No option for the merger
	// Use the default units for the converter: millimeter (position), degree (angle), newton (force), newton-millimeter (moment) and watt (power).
	C3DWriter->SetFilename(path);

	// To execute the pipeline, it is only necessary to update the last process.
	// The pipeline will check if the previous are already updated an do it if necessary.
	C3DWriter->Update();
}
