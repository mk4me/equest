#include "HmmLibPCH.h"
#include "MarkersFilter.h"
#include <corelib/PluginCommon.h>

//#include <plugins/newVdf/TreeBuilder.h>

static NamesDictionary getNames( )
{
    NamesDictionary names;
    names["RFHD"] = std::make_pair("RFHD", "Right front of head");
    names["LFHD"] = std::make_pair("LFHD", "Left front of head");
    names["CLAV"] = std::make_pair("CLAV", "Clavicle");
    names["STRN"] = std::make_pair("STRN", "Sternum");
    names["LSHO"] = std::make_pair("LSHO", "Left shoulder");
    names["RSHO"] = std::make_pair("RSHO", "Right shoulder");
    names["LUPA"] = std::make_pair("LUPA", "Left upper arm");
    names["RUPA"] = std::make_pair("RUPA", "Right upper arm");
    names["RELB"] = std::make_pair("RELB", "Right elbow");
    names["LELB"] = std::make_pair("LELB", "Left elbow");
    names["RFRM"] = std::make_pair("RFRM", "Right forearm");
    names["LRFM"] = std::make_pair("LRFM", "Left forearm");
    names["RWRB"] = std::make_pair("RWRB", "Right wrist marker B");
    names["RWRA"] = std::make_pair("RWRA", "Right wrist marker A");
    names["RFIN"] = std::make_pair("RFIN", "Right finger");
    names["LWRA"] = std::make_pair("LWRA", "Left wrist marker A");
    names["LWRB"] = std::make_pair("LWRB", "Left wrist marker B");
    names["LFIN"] = std::make_pair("LFIN", "Left finger");
    names["RASI"] = std::make_pair("RASI", "Right asis");
    names["LASI"] = std::make_pair("LASI", "Left asis");
    names["RTOE"] = std::make_pair("RTOE", "Right toe");
    names["RANK"] = std::make_pair("RANK", "Right ankle");
    names["LANK"] = std::make_pair("LANK", "Left ankle");
    names["LTOE"] = std::make_pair("LTOE", "Left toe");
    names["LTHI"] = std::make_pair("LTHI", "Left thigh");
    names["RTHI"] = std::make_pair("RTHI", "Right thigh");
    names["RKNE"] = std::make_pair("RKNE", "Right knee");
    names["LKNE"] = std::make_pair("LKNE", "Left knee");
    names["RTIB"] = std::make_pair("RTIB", "Right tibial wand marker");
    names["LTIB"] = std::make_pair("LTIB", "Left tibial wand marker");
    names["LPSI"] = std::make_pair("LPSI", "Left psis");
    names["RPSI"] = std::make_pair("RPSI", "Right psis");
    names["LBHD"] = std::make_pair("LBHD", "Left back of head");
    names["RBHD"] = std::make_pair("RBHD", "Right back of head");
    names["RBAK"] = std::make_pair("RBAK", "Right back");
    names["C7"  ] = std::make_pair("C7"  , "Cervical Vertebra");
    names["T10" ] = std::make_pair("T10" , "Thoracic Vertebra");
    return names;
}

MarkersFilter::MarkersFilter() : 
BuilderConfiguredFilterCommand(getNames(), typeid(c3dlib::MarkerChannel),
        QString::fromStdString(plugin::getResourcePath("images/skeleton_front/skeleton_markers.xml").string()),
        QString::fromStdString(plugin::getResourcePath("images/skeleton_back/skeleton_markers.xml").string()),
        QObject::tr("Markers"), QIcon(QString::fromUtf8(":/mde/icons/markerSmall.png")))//,
        //TreeBuilder::getRootMarkersIcon(), TreeBuilder::getMarkersIcon())
{

}

