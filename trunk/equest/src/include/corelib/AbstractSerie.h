/********************************************************************
	created:	2013/10/11
	created:	11:10:2013   10:13
	filename: 	AbstractSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORELIB__ABSTRACTSERIE_H__
#define HEADER_GUARD_CORELIB__ABSTRACTSERIE_H__


#include <corelib/IVisualizer.h>

namespace plugin {

    class CORELIB_EXPORT AbstractSerie : public plugin::IVisualizer::ISerie
    {
    public:
		AbstractSerie();
        virtual ~AbstractSerie() {}

    public:
        virtual void setName( const std::string & name );
        virtual const std::string getName() const;
        virtual const core::VariantConstPtr & getData() const;
        virtual const utils::TypeInfo & getRequestedDataType() const;
        virtual void setData( const utils::TypeInfo & requestedDataType, const core::VariantConstPtr & data );

    public:
        virtual void setupData( const core::VariantConstPtr & data ) = 0;

    protected:
        //! wrapper przekazany serii
        core::VariantConstPtr data;
        utils::TypeInfo requestedType;
        //! nazwa serii
        std::string name;
    };
    DEFINE_SMART_POINTERS(AbstractSerie);
}
#endif
