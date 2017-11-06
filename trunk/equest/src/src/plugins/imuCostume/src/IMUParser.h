/********************************************************************
	created:	2014/06/23
	created:	23:6:2014   12:44
	filename: 	IMUParser.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_IMU__IMUPARSER_H__
#define HEADER_GUARD_IMU__IMUPARSER_H__

#include <utils/SmartPtr.h>
#include <utils/Filesystem.h>
#include <corelib/IParser.h>
#include <plugins/c3d/C3DChannels.h>


namespace IMU {

    struct IMUConfig;

    class IMUParser : public plugin::ISourceParser
    {
        UNIQUE_ID("{122EC6AC-AA67-4320-A240-6530FCB7E11C}")
        CLASS_DESCRIPTION("IMU dat parser", "IMU dat parser")
    public:
        IMUParser() {}
        virtual ~IMUParser() {}


    public:
        //! Parsowanie pliku c3d
        //! \param path poprawna ścieżka do pliku
        virtual void parse(const std::string & source);

	c3dlib::VectorChannelReaderInterfacePtr createChannel(int hz, const IMUConfig& config, int i);

        //! \return pusty obiekt nowego parsera
        virtual plugin::IParser* create() const;
        virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const;
        virtual void reset();
        //! Zwraca rozszerzenia, które są obsługiwane przez parser (tylko c3d)
        //! \param extensions kolecja z roszerzeniami
        virtual void acceptedExpressions(Expressions & expressions) const;

    private:
        utils::ObjectWrapperPtr accWrapper;
        utils::ObjectWrapperPtr magWrapper;
        utils::ObjectWrapperPtr gyrWrapper;
        utils::ObjectWrapperPtr quatWrapper;
	};
    DEFINE_SMART_POINTERS(IMUParser);

}
#endif
 
