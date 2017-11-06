#include "HmmLibPCH.h"
#include "KineticFilter.h"
#include <corelib/PluginCommon.h>
//#include <plugins/newVdf/TreeBuilder.h>

static NamesDictionary getPowersNames()
{
    NamesDictionary powersNames;
    powersNames["LAnkle"    ] = std::make_pair("LAnklePower",    "Left Ankle");
    powersNames["LElbow"    ] = std::make_pair("LElbowPower",    "Left Elbow");
    powersNames["LHip"      ] = std::make_pair("LHipPower",      "Left Hip");
    powersNames["LKnee"     ] = std::make_pair("LKneePower",     "Left Knee");
    powersNames["LNeck"     ] = std::make_pair("LNeckPower",     "Left Neck");
    powersNames["LShoulder" ] = std::make_pair("LShoulderPower", "Left Shoulder");
    powersNames["LWaist"    ] = std::make_pair("LWaistPower",    "Left Waist");
    powersNames["LWrist"    ] = std::make_pair("LWristPower",    "Left Wrist");
    powersNames["RAnkle"    ] = std::make_pair("RAnklePower",    "Right Ankle");
    powersNames["RElbow"    ] = std::make_pair("RElbowPower",    "Right Elbow");
    powersNames["RHip"      ] = std::make_pair("RHipPower",      "Right Hip");
    powersNames["RKnee"     ] = std::make_pair("RKneePower",     "Right Knee");
    powersNames["RNeck"     ] = std::make_pair("RNeckPower",     "Right Neck");
    powersNames["RShoulder" ] = std::make_pair("RShoulderPower", "Right Shoulder");
    powersNames["RWaist"    ] = std::make_pair("RWaistPower",    "Right Waist");
    powersNames["RWrist"    ] = std::make_pair("RWristPower",    "Right Wrist");
    return powersNames;
}

static NamesDictionary getMomentsNames()
{
    NamesDictionary momentsNames;
    momentsNames["LAnkle"           ] = std::make_pair("LAnkleMoment",          "Left Ankle");
    momentsNames["LElbow"           ] = std::make_pair("LElbowMoment",          "Left Elbow");
    momentsNames["LGroundReaction"  ] = std::make_pair("LGroundReactionMoment", "Left GRF");
    momentsNames["LHip"             ] = std::make_pair("LHipMoment",            "Left Hip");
    momentsNames["LKnee"            ] = std::make_pair("LKneeMoment",           "Left Knee");
    momentsNames["LNeck"            ] = std::make_pair("LNeckMoment",           "Left Neck");
    momentsNames["LShoulder"        ] = std::make_pair("LShoulderMoment",       "Left Shoulder");
    momentsNames["LWaist"           ] = std::make_pair("LWaistMoment",          "Left Waist");
    momentsNames["LWrist"           ] = std::make_pair("LWristMoment",          "Left Wrist");
    momentsNames["RAnkle"           ] = std::make_pair("RAnkleMoment",          "Right Ankle");
    momentsNames["RElbow"           ] = std::make_pair("RElbowMoment",          "Right Elbow");
    momentsNames["RGroundReaction"  ] = std::make_pair("RGroundReactionMoment", "Right GRF");
    momentsNames["RHip"             ] = std::make_pair("RHipMoment",            "Right Hip");
    momentsNames["RKnee"            ] = std::make_pair("RKneeMoment",           "Right Knee");
    momentsNames["RNeck"            ] = std::make_pair("RNeckMoment",           "Right Neck");
    momentsNames["RShoulder"        ] = std::make_pair("RShoulderMoment",       "Right Shoulder");
    momentsNames["RWaist"           ] = std::make_pair("RWaistMoment",          "Right Waist");
    momentsNames["RWrist"           ] = std::make_pair("RWristMoment",          "Right Wrist");
    return momentsNames;
}

static NamesDictionary getForcesNames() 
{
    NamesDictionary forcesNames;
    forcesNames["LAnkle"          ] = std::make_pair("LAnkleForce",          "Left Ankle");
    forcesNames["LElbow"          ] = std::make_pair("LElbowForce",          "Left Elbow");
    forcesNames["LGroundReaction" ] = std::make_pair("LGroundReactionForce", "Left GRF");
    forcesNames["LHip"            ] = std::make_pair("LHipForce",            "Left Hip");
    forcesNames["LKnee"           ] = std::make_pair("LKneeForce",           "Left Knee");
    forcesNames["LNeck"           ] = std::make_pair("LNeckForce",           "Left Neck");
    forcesNames["LShoulder"       ] = std::make_pair("LShoulderForce",       "Left Shoulder");
    forcesNames["LWaist"          ] = std::make_pair("LWaistForce",          "Left Waist");
    forcesNames["LWrist"          ] = std::make_pair("LWristForce",          "Left Wrist");
    forcesNames["RAnkle"          ] = std::make_pair("RAnkleForce",          "Right Ankle");
    forcesNames["RElbow"          ] = std::make_pair("RElbowForce",          "Right Elbow");
    forcesNames["RGroundReaction" ] = std::make_pair("RGroundReactionForce", "Right GRF");
    forcesNames["RHip"            ] = std::make_pair("RHipForce",            "Right Hip");
    forcesNames["RKnee"           ] = std::make_pair("RKneeForce",           "Right Knee");
    forcesNames["RNeck"           ] = std::make_pair("RNeckForce",           "Right Neck");
    forcesNames["RShoulder"       ] = std::make_pair("RShoulderForce",       "Right Shoulder");
    forcesNames["RWaist"          ] = std::make_pair("RWaistForce",          "Right Waist");
    forcesNames["RWrist"          ] = std::make_pair("RWristForce",          "Right W rist");
    forcesNames["RNormalizedGR"   ] = std::make_pair("RNormalisedGRF",       "Right, normalised GRF");
    forcesNames["LNormalizedGR"   ] = std::make_pair("LNormalisedGRF",       "Left, normalised GRF");
    return forcesNames;
}

ForcesFilter::ForcesFilter() : 
BuilderConfiguredFilterCommand(getForcesNames(), typeid(c3dlib::ForceChannel),
    QString::fromStdString(plugin::getResourcePath("images/skeleton_front/skeleton_front.xml").string()),
    QString::fromStdString(plugin::getResourcePath("images/skeleton_back/skeleton_back.xml").string()),
    QObject::tr("Forces"), QIcon(QString::fromUtf8(":/mde/icons/forcesSmall.png")))//,
    //TreeBuilder::getRootMarkersIcon(), TreeBuilder::getMarkersIcon())
{

}


MomentsFilter::MomentsFilter() : 
BuilderConfiguredFilterCommand(getMomentsNames(), typeid(c3dlib::MomentChannel),
    QString::fromStdString(plugin::getResourcePath("images/skeleton_front/skeleton_front.xml").string()),
    QString::fromStdString(plugin::getResourcePath("images/skeleton_back/skeleton_back.xml").string()),
    QObject::tr("Moments"),QIcon(QString::fromUtf8(":/mde/icons/momentSmall.png")))//,
    //TreeBuilder::getRootMarkersIcon(), TreeBuilder::getMarkersIcon())
{

}


PowersFilter::PowersFilter() : 
BuilderConfiguredFilterCommand(getPowersNames(), typeid(c3dlib::PowerChannel),
    QString::fromStdString(plugin::getResourcePath("images/skeleton_front/skeleton_front.xml").string()),
    QString::fromStdString(plugin::getResourcePath("images/skeleton_back/skeleton_back.xml").string()),
    QObject::tr("Powers"), QIcon(QString::fromUtf8(":/mde/icons/powerSmall.png")))//,
    //TreeBuilder::getRootMarkersIcon(), TreeBuilder::getMarkersIcon())
{

}




