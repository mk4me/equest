#include "HmmLibPCH.h"
#include "EMGConfigFilter.h"
#include <corelib/PluginCommon.h>
//#include <plugins/newVdf/TreeBuilder.h>

static NamesDictionary getNames()
{
    NamesDictionary emgNames;
    emgNames["krawiecki_l"                ] = std::make_pair("krawiecki_l"                , "krawiecki_l"                );
    emgNames["krawiecki_p"                ] = std::make_pair("krawiecki_p"                , "krawiecki_p"                );
    emgNames["naramienny_l"               ] = std::make_pair("naramienny_l"               , "naramienny_l"               );
    emgNames["naramienny_p"               ] = std::make_pair("naramienny_p"               , "naramienny_p"               );
    emgNames["odwodziciel_krotki_kciuka_l"] = std::make_pair("odwodziciel_krotki_kciuka_l", "odwodziciel_krotki_kciuka_l");
    emgNames["odwodziciel_krotki_kciuka_p"] = std::make_pair("odwodziciel_krotki_kciuka_p", "odwodziciel_krotki_kciuka_p");
    emgNames["piersiowy_wiekszy_l"        ] = std::make_pair("piersiowy_wiekszy_l"        , "piersiowy_wiekszy_l"        );
    emgNames["piersiowy_wiekszy_p"        ] = std::make_pair("piersiowy_wiekszy_p"        , "piersiowy_wiekszy_p"        );
    emgNames["piszczelowy_przedni_l"      ] = std::make_pair("piszczelowy_przedni_l"      , "piszczelowy_przedni_l"      );
    emgNames["piszczelowy_przedni_p"      ] = std::make_pair("piszczelowy_przedni_p"      , "piszczelowy_przedni_p"      );
    emgNames["prosty_uda_l"               ] = std::make_pair("prosty_uda_l"               , "prosty_uda_l"               );
    emgNames["prosty_uda_p"               ] = std::make_pair("prosty_uda_p"               , "prosty_uda_p"               );
    emgNames["ramie3"                     ] = std::make_pair("ramie3"                     , "ramie3"                     );
    emgNames["ramie3R"                    ] = std::make_pair("ramie3R"                    , "ramie3R"                    );
    emgNames["strzalkowy_p"               ] = std::make_pair("strzalkowy_p"               , "strzalkowy_p"               );
    emgNames["strzalkowy_l"               ] = std::make_pair("strzalkowy_l"               , "strzalkowy_l"               );
    emgNames["przy_kstrzalkowej_piszczelowy_p"] = std::make_pair("przy_kstrzalkowej_piszczelowy_p", "przy_kstrzalkowej_piszczelowy_p");
    emgNames["przy_kstrzalkowej_piszczelowy_l"] = std::make_pair("przy_kstrzalkowej_piszczelowy_l", "przy_kstrzalkowej_piszczelowy_l");

    emgNames["brzuchaty_lydki_gb_l"       ] = std::make_pair("brzuchaty_lydki_gb_l"       , "brzuchaty_lydki_gb_l"       );
    emgNames["brzuchaty_lydki_gb_p"       ] = std::make_pair("brzuchaty_lydki_gb_p"       , "brzuchaty_lydki_gb_p"       );
    emgNames["brzuchaty_lydki_gprzysr_l"  ] = std::make_pair("brzuchaty_lydki_gprzysr_l"  , "brzuchaty_lydki_gprzysr_l"  );
    emgNames["brzuchaty_lydki_gprzysr_p"  ] = std::make_pair("brzuchaty_lydki_gprzysr_p"  , "brzuchaty_lydki_gprzysr_p"  );
    emgNames["czworoboczny_grzbietu_l"    ] = std::make_pair("czworoboczny_grzbietu_l"    , "czworoboczny_grzbietu_l"    );
    emgNames["czworoboczny_grzbietu_p"    ] = std::make_pair("czworoboczny_grzbietu_p"    , "czworoboczny_grzbietu_p"    );
    emgNames["dwuglowy_uda_l"             ] = std::make_pair("dwuglowy_uda_l"             , "dwuglowy_uda_l"             );
    emgNames["dwuglowy_uda_p"             ] = std::make_pair("dwuglowy_uda_p"             , "dwuglowy_uda_p"             );
    emgNames["polsciegnisty_l"            ] = std::make_pair("polsciegnisty_l"            , "polsciegnisty_l"            );
    emgNames["polsciegnisty_p"            ] = std::make_pair("polsciegnisty_p"            , "polsciegnisty_p"            );
    emgNames["posladkowy_wielki_l"        ] = std::make_pair("posladkowy_wielki_l"        , "posladkowy_wielki_l"        );
    emgNames["posladkowy_wielki_p"        ] = std::make_pair("posladkowy_wielki_p"        , "posladkowy_wielki_p"        );
    emgNames["prostownik_palcow_l"        ] = std::make_pair("prostownik_palcow_l"        , "prostownik_palcow_l"        );
    emgNames["prostownik_palcow_p"        ] = std::make_pair("prostownik_palcow_p"        , "prostownik_palcow_p"        );
    emgNames["ramie-tyl2"                 ] = std::make_pair("ramie-tyl2"                 , "ramie-tyl2"                 );
    emgNames["ramie-tyl2P"                ] = std::make_pair("ramie-tyl2P"                , "ramie-tyl2P"                );
    emgNames["ramie-tyl3"                 ] = std::make_pair("ramie-tyl3"                 , "ramie-tyl3"                 );
    emgNames["ramie-tyl3P"                ] = std::make_pair("ramie-tyl3P"                , "ramie-tyl3P"                );
    emgNames["most_obojcz_sutkowy_p"      ] = std::make_pair("rmost_obojcz_sutkowy_p"     , "most_obojcz_sutkowy_p"      );
    emgNames["most_obojcz_sutkowy_l"      ] = std::make_pair("rmost_obojcz_sutkowy_l"     , "most_obojcz_sutkowy_l"      );
    //emgNames["ramie-tyl6"                 ] = std::make_pair("ramie-tyl6"                 , "ramie-tyl6"                 );
    //emgNames["ramie-tyl6P"                ] = std::make_pair("ramie-tyl6P"                , "ramie-tyl6P"                );
    emgNames["trojglowy_ramienia_gdluga_l"] = std::make_pair("trojglowy_ramienia_gdluga_l", "trojglowy_ramienia_gdluga_l");
    emgNames["trojglowy_ramienia_gdluga_p"] = std::make_pair("trojglowy_ramienia_gdluga_p", "trojglowy_ramienia_gdluga_p");

    return emgNames;
}

EMGConfigFilter::EMGConfigFilter() :
BuilderConfiguredFilterCommand(getNames(), typeid(c3dlib::EMGChannel),
    QString::fromStdString(plugin::getResourcePath("images/muscular_front/muscular_front.xml").string()),
    QString::fromStdString(plugin::getResourcePath("images/muscular_back/muscular_back.xml").string()),
    QObject::tr("EMGs"), QIcon(QString::fromUtf8(":/mde/icons/emg1Small.png")))//,
    //TreeBuilder::getRootMarkersIcon(), TreeBuilder::getMarkersIcon())
{

}
