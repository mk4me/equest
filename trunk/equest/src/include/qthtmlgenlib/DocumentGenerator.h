/********************************************************************
    created:  2012/03/01
    created:  1:3:2012   11:14
    filename: DocumentGenerator.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DOCUMENTGENERATOR_H__
#define HEADER_GUARD___DOCUMENTGENERATOR_H__

#include <QtCore/QString>

namespace htmlGen {

//! Klasa generująca dokumenty HTML - dodaje podstawowe znaczniki otwierające i zamykające
class DocumentGenerator
{
public:

    static void openDocument(QString & html);
    static void closeDocument(QString & html);

    static void openBody(QString & html, const QString & style = QString());
    static void closeBody(QString & html);
};

}

#endif	//	HEADER_GUARD___DOCUMENTGENERATOR_H__
