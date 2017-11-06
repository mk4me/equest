#include "DicomPCH.h"
#include "DescriptionMaker.h"

QString dicom::DescriptionMaker::createDesc(const internalData::Patient& patient)
{
	QString desc;
	desc += QString("patientID : %1\n").arg(patient.patientID.c_str());
	desc += QString("patientName : %1\n").arg(patient.patientName.c_str());
	desc += QString("patientBirthdate : %1\n").arg(patient.patientBirthdate.c_str());
	desc += QString("patientSex : %1\n").arg(patient.patientSex.c_str());
	return desc;
}

QString dicom::DescriptionMaker::createDesc(const internalData::Study& study)
{
	QString desc;
	desc += QString("studyId: %1\n").arg(study.studyId.c_str());
	desc += QString("studyDesc: %1\n").arg(study.studyDesc.c_str());
	desc += QString("studyDate: %1\n").arg(study.studyDate.c_str());
	desc += QString("studyTime: %1\n").arg(study.studyTime.c_str());
	desc += QString("physicianName: %1\n").arg(study.physicianName.c_str());
	desc += QString("accesionNumber: %1\n").arg(study.accesionNumber.c_str());
	desc += QString("studyNumber: %1\n").arg(study.studyNumber);
	return desc;
}

QString dicom::DescriptionMaker::createDesc(const internalData::Serie& serie)
{
	QString desc;
	desc += QString("serieId : %1\n").arg(serie.serieId.c_str());
	desc += QString("modality : %1\n").arg(serie.modality.c_str());
	desc += QString("serieDate : %1\n").arg(serie.serieDate.c_str());
	desc += QString("serieTime : %1\n").arg(serie.serieTime.c_str());
	desc += QString("serieDesc : %1\n").arg(serie.serieDesc.c_str());
	desc += QString("serieNumber : %1\n").arg(serie.serieNumber.c_str());
	desc += QString("physicianName : %1\n").arg(serie.physicianName.c_str());
	return desc;
}

QString dicom::DescriptionMaker::createDesc(const internalData::Image& image)
{
	QString desc;
	desc += QString("imageFile : %1\n").arg(image.imageFile.c_str());
	//desc += QString("adnotationsFile : %1\n").arg(image.adnotationsFile.c_str());
	desc += QString("originFilePath : %1\n").arg(image.originFilePath.c_str());
	desc += QString("isPowerDoppler : %1\n").arg(image.isPowerDoppler == true ? QObject::tr("yes") : QObject::tr("no"));
	return desc;
}
