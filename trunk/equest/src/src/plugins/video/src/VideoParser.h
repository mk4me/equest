/********************************************************************
	created:  2011/02/04
	created:  4:2:2011   15:01
	filename: VideoParser.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDEO__VIDEOPARSER_H__
#define __HEADER_GUARD_VIDEO__VIDEOPARSER_H__

#include <corelib/IParser.h>

class VideoParser : public plugin::ISourceParser, public plugin::IStreamParser
{
    UNIQUE_ID("{E81ED99E-6682-4FE7-9E52-0BBCABC8BCBC}");
	CLASS_DESCRIPTION("Video Parser", "Video Parser");
private:
    //! Obiekt wewnętrzny.
	utils::ObjectsVector data;
	std::string path;
	IStreamPtr stream;
    
public:
    VideoParser();
    virtual ~VideoParser();
// IParser
public:
	virtual void parse(const std::string & source);	
	virtual void parse(const IStreamPtr stream, const std::string & source);
    virtual plugin::IParser* create() const;
	virtual void acceptedExpressions(Expressions & expressions) const;
	virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const;
	virtual void reset();
};


#endif  // __HEADER_GUARD_VIDEO__VIDEOPARSER_H__
