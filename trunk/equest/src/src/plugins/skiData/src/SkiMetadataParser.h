/********************************************************************
	created:  2016/07/18
	filename: SkiMetadataParser.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_SKIDATA__SKIMETADATAPARSER_H__
#define __HEADER_GUARD_SKIDATA__SKIMETADATAPARSER_H__

#include <corelib/IParser.h>

namespace skidata
{
	class SkiMetadataParser : public plugin::ISourceParser
	{
		UNIQUE_ID("33178925-FC36-BACA-BACA-25F5268BE1F2")
		CLASS_DESCRIPTION("Ski metadata parser", "Ski metadata parser")
	public:
		//! Konstruktor domyœlny
		SkiMetadataParser();
		//! Destruktor wirtualny
		virtual ~SkiMetadataParser();
		//! \retrun Lista rozszerzeñ, które parser obs³uguje. Musz¹ byæ oddzielone œrednikiem.
		//! Obs³ugiwany format rozszerzenia: [ { *. | . } ]ext
		virtual void acceptedExpressions(Expressions & expressions) const;
		//! \return Instancja tego samego typu.
		virtual IParser* create() const;
		//! \param object [out] Obiekty danych parsera w kolejnoœci jak w opisie ExpressionDescription::types dla danego Ÿród³a!!
		//! \param idx Indeks obiektu wg opisu dla danego Ÿród³a, który chcemy pobraæ
		virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const;
		//! Metoda zwalniaj¹ca sparsowane zasoby parsera
		virtual void reset();
		//! Przyporz¹dkowanie parsera do konkretnego pliku.
		//! Na ka¿dej instancji parsera ta metoda wywo³ywana jest maksymalnie jeden raz.
		//! \param source Œcie¿ka do Ÿród³a danych - plik, url, id urz¹dzenia, ...
		virtual void parse(const std::string & source);

	private:
		//! Dane
		utils::ObjectWrapperPtr data;
	};
}

#endif  // __HEADER_GUARD_SKIDATA__SKIMETADATAPARSER_H__