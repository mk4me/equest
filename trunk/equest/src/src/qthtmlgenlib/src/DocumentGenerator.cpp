#include <qthtmlgenlib/DocumentGenerator.h>

namespace htmlGen {

void DocumentGenerator::openDocument(QString & html)
{
    html =  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">" \
            "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">" \
            "p, li { white-space: pre-wrap; }" \
            "</style></head>";
}

void DocumentGenerator::closeDocument(QString & html)
{
    html += "</html>";
}

void DocumentGenerator::openBody(QString & html, const QString & style)
{
    html +=  "<body" + (style.isEmpty() == false ? " style=\"" + style + "\"" : "") + ">";
}

void DocumentGenerator::closeBody(QString & html)
{
    html += "</body>";
}

}
