//#include "DicomImporterPCH.h"
#include "dicomlib/DicomImporter.h"
#include <QtGui/QPixmap>
#include <QtGui/QPixmap>
#include <dcmtk/ofstd/ofbmanip.h>
#include <dcmtk/dcmimage/diregist.h>
#include "dicomlib/Dicom.h"
//
using namespace dicomImporter;
//
//QPixmap dicomImporter::convertToPixmap( DicomImagePtr image )
//{
//	QPixmap result;
//	// kod z forum : http://forum.dcmtk.org/viewtopic.php?t=120
//	// rozszerzony o obs³ugê kolorów
//	if ((image != NULL) && (image->getStatus() == EIS_Normal))
//	{
//		/* get image extension */
//		const int width = (int)(image->getWidth());
//		const int height = (int)(image->getHeight());
//		//const int depth = (int)(image->getDepth());
//		char header[32];
//		/* create PGM header */
//		sprintf(header, "P%i\n%i %i\n255\n", image->isMonochrome() ? 5 : 6, width, height);
//		const int offset = strlen(header);
//		const unsigned int length = image->getOutputDataSize(8) + offset;//width * height + offset;
//		/* create output buffer for DicomImage class */
//		Uint8 *buffer = new Uint8[length];
//		if (buffer != NULL)
//		{
//			/* copy PGM header to buffer */
//			OFBitmanipTemplate<Uint8>::copyMem((const Uint8 *)header, buffer, offset);
//			if (image->getOutputData((void *)(buffer + offset), length))
//			{
//				result.loadFromData((const unsigned char *)buffer, length, image->isMonochrome() ? "PGM" : "PMM", Qt::AvoidDither);
//			}
//			/* delete temporary pixel buffer */
//			delete[] buffer;
//		}
//	}
//
//	return result;
//
//
//	// ponizszy kod przeksztalcil 16 bitowe monochromatyczne obrazy
//
//	//if ((image != NULL) && (image->getStatus() == EIS_Normal)) {
//	//    const int width = (int)(image->getWidth());
//	//    const int height = (int)(image->getHeight());
//	//    const int depth = (int)(image->getDepth());
//
//	//	QImage qi(width, height, QImage::Format_RGB32);
//	//	const unsigned int length = image->getOutputDataSize(16);
//	//	double maxv = 0, minv = 65535;
//	//	//image->getMinMaxValues(minv, maxv);
//	//    /* create output buffer for DicomImage class */
//	//    Uint16 *buffer = new Uint16[length];
//	//    if (buffer != NULL)
//	//    {
//	//        if (image->getOutputData((void *)(buffer), length,16))
//	//        {
//	//			for (int y = 0; y < height; ++y) {
//	//				for (int x = 0; x < width; ++x) {
//	//					Uint16 p = buffer[(y*width + x)];
//	//					if (p < minv) {
//	//						minv = p;
//	//					}
//	//					if (p > maxv) {
//	//						maxv = p;
//	//					}
//	//				}
//	//			}
//
//	//			for (int y = 0; y < height; ++y) {
//	//				for (int x = 0; x < width; ++x) {
//	//					Uint16 p = buffer[(y*width + x)];
//	//					auto val = 255.0 * (p-minv)/(maxv-minv);
//	//					qi.setPixel(x, y, qRgb(val, val, val));
//	//				}
//	//			}
//	//        }
//	//        /* delete temporary pixel buffer */
//	//        delete[] buffer;
//	//    }
//
//	//	return QPixmap::fromImage(qi);
//	//}
//	//return QPixmap();
//
//}
