#include "PCH.h"
#include "VisualizationScheme.h"
#include "viconformatslib/VskParser.h"

using namespace std;
using namespace osg;
using namespace boost;

vicon::VskPtr Vsk::Count16 = vicon::VskPtr();
vicon::VskPtr Vsk::Count39 = vicon::VskPtr();
vicon::VskPtr Vsk::Count53 = vicon::VskPtr();

vicon::VskConstPtr Vsk::get( MarkersCount count )
{
    switch(count)
    {
    case MarkersCount15:
    case MarkersCount16:
        if (!Count16) {
			std::ifstream fs(plugin::getResourcePath("trial/CZD.vsk").string());
			Count16 = utils::make_shared<vicon::Vsk>(vicon::VskParser::parse(fs));
        }
        return Count16;
    case MarkersCount39:
        if (!Count39) {
			std::ifstream fs(plugin::getResourcePath("trial/M39.vsk").string());
			Count39 = utils::make_shared<vicon::Vsk>(vicon::VskParser::parse(fs));
        }
        return Count39;

    case MarkersCount53:
        if (!Count53) {
			std::ifstream fs(plugin::getResourcePath("trial/M53.vsk").string());
			Count53 = utils::make_shared<vicon::Vsk>(vicon::VskParser::parse(fs));			
        }
        return Count53;

    default:
        throw loglib::runtime_error("Wrong VSK scheme was requested");
    }
}
