//#include "DicomImporterPCH.h"
#include "DicomParser.h"
#include <utils/Filesystem.h>
#include <dicomlib/Dicom.h>
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/cmdlnarg.h>
#include <dcmtk/ofstd/ofconapp.h>
#include <dcmtk/dcmdata/dcuid.h>    
#include <dcmtk/ofstd/ofstd.h>
#include <dcmtk/dcmdata/dcistrmz.h> 

#include "dcmtk/ofstd/ofstdinc.h"


#include "dcmtk/dcmdata/dctk.h"          /* for various dcmdata headers */
#include "dcmtk/dcmdata/cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcmtk/dcmdata/dcuid.h"         /* for dcmtk version name */
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */

#include "dcmtk/dcmimgle/dcmimage.h"     /* for DicomImage */
#include "dcmtk/dcmimgle/digsdfn.h"      /* for DiGSDFunction */
#include "dcmtk/dcmimgle/diciefn.h"      /* for DiCIELABFunction */

#include "dcmtk/ofstd/ofconapp.h"        /* for OFConsoleApplication */
#include "dcmtk/ofstd/ofcmdln.h"         /* for OFCommandLine */

#include "dcmtk/dcmimage/diregist.h"     /* include to support color images */
#include "dcmtk/ofstd/ofstd.h"           /* for OFStandard */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmjpeg/djdecode.h"

using namespace dicomImporter;

DicomParser::DicomParser()
{
    
}

DicomParser::~DicomParser()
{

}


void DicomParser::parse( const std::string & source  )
{
	/*utils::Filesystem::Path path(source);
    const char *opt_ifname = path.string().c_str();
    E_FileReadMode opt_readMode = ERM_autoDetect;
    E_TransferSyntax opt_ixfer = EXS_Unknown;

    DcmFileFormat fileformat;
    DcmDataset * dataset = fileformat.getDataset();


    OFCondition error = fileformat.loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);

    if (!error.bad())
    {
        wrapper->set(dataset);
    }*/

    
   // utils::Filesystem::Path path(source);
   // auto str = path.string();
   // const char *opt_ifname = str.c_str();
   // E_FileReadMode opt_readMode = ERM_autoDetect;
   // E_TransferSyntax opt_ixfer = EXS_Unknown;
   // OFCmdUnsignedInt    opt_frame = 1;                    /* default: first frame */
   // OFCmdUnsignedInt    opt_frameCount = 1;               /* default: one frame */
   // unsigned long       opt_compatibilityMode = CIF_MayDetachPixelData | CIF_TakeOverExternalDataset;


   // E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;
   // // register JPEG decompression codecs
   // DJDecoderRegistration::registerCodecs(opt_decompCSconversion);

   // // register RLE decompression codec
   //// DcmRLEDecoderRegistration::registerCodecs();

   // DcmFileFormat *dfile = new DcmFileFormat();
   // OFCondition cond = dfile->loadFile(opt_ifname);//, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);

   // if (cond.bad())
   // {
   //     //
   // }

   // 
   // DcmDataset *dataset = dfile->getDataset();
   // E_TransferSyntax xfer = dataset->getOriginalXfer();

   // Sint32 frameCount;
   // if (dataset->findAndGetSint32(DCM_NumberOfFrames, frameCount).bad())
   //     frameCount = 1;
   // if ((opt_frameCount == 0) || ((opt_frame == 1) && (opt_frameCount == OFstatic_cast(Uint32, frameCount))))
   // {
   //     // since we process all frames anyway, decompress the complete pixel data (if required)
   //     opt_compatibilityMode |= CIF_DecompressCompletePixelData;
   // }
   // if (frameCount > 1)
   // {
   //     // use partial read access to pixel data (only in case of multiple frames)
   //     opt_compatibilityMode |= CIF_UsePartialAccessToPixelData;
   // }
	image = utils::ObjectWrapper::create<DicomImage>();
    DicomImagePtr di = utils::make_shared<DicomImage>(source.c_str());//dfile, xfer, opt_compatibilityMode, opt_frame - 1, opt_frameCount);
    if (di && di->getStatus() == EIS_Normal)  {
        di->processNextFrames();
        auto count = di->getFrameCount();
        image->set(di);
    }
}

plugin::IParser* DicomParser::create() const
{
    return new DicomParser();
}

void DicomParser::acceptedExpressions(Expressions & expressions) const
{
	ExpressionDescription expDesc;
	expDesc.description = "Dicom format";
	expDesc.objectsTypes.reserve(2);
	expDesc.objectsTypes.push_back(typeid(DcmDataset));
	expDesc.objectsTypes.push_back(typeid(DicomImage));

	expressions.insert(Expressions::value_type(".*\.dcm$", expDesc));
	// HACK: regex pasuje do plikow, ktore nie maja kropki w sciezce...
	// przez to mozna skojarzyc pliki bez rozszerzenia z parserem dicoma
	// TODO, jak lepiej rozwiazac ten 
	expressions.insert(Expressions::value_type("^[^.]*$", expDesc));
}

void DicomParser::getObject(core::Variant& object, const core::VariantsVector::size_type idx) const
{
	if (idx == 0) {
		object.set(image);
	}
}

void dicomImporter::DicomParser::reset()
{
	image = utils::ObjectWrapperPtr();
}


