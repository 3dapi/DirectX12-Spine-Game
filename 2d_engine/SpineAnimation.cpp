#include <map>
#include <string>
#include <vector>
#include "Common/G2.Util.h"
#include "RenderSpine.h"

using namespace std;

vector<string> detachSlotSpineBoy = {
	"spineboy-torso", "head", "eyes-open", "mouth-smile", "visor",
	"gun", "front-arm", "front-bracer", "front-hand",
	"back-arm", "back-hand", "back-bracer", "back-knee",
	"lower-leg", "front-thigh",
	"stirrup-front", "stirrup-back", "stirrup-strap",
	"raptor-saddle",
	"raptor-saddle-strap-front",
	"raptor-saddle-strap-back",
	//"raptor-jaw-inside",
	//"raptor-mouth-inside",
	//"raptow-jaw-tooth",
	"raptor-horn-back",
	//"raptor-tongue",
	//"raptor-hindleg-back",
	//"raptor-back-arm",
	"back-thigh",
	//"raptor-body",
	//"raptor-jaw",
	//"raptor-front-arm",
	//"raptor-front-leg",
	"neck",
	"raptor-horn",

};
vector<string> detachSlotHero = {
	"weapon-morningstar-path",
	"chain-ball", "chain-round", "chain-round2", "chain-round3",
	"chain-flat", "chain-flat2", "chain-flat3", "chain-flat4", "handle"
};

map<string, SPINE_ATTRIB> spine_rsc =
{
	{"raptor"       , {"raptor"       , "raptor.atlas"           , "raptor-pro.json"       , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.45F, {-840.0F, -300.0F}, "walk", "default", detachSlotSpineBoy}  },
	{"goblins"      , {"goblins"      , "goblins-pma.atlas"      , "goblins-pro.json"      , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 1.25F, {-400.0F, -300.0F}, "walk", "goblin" , {} }                 },
	{"hero"         , {"hero"         , "hero-pro.atlas"         , "hero-pro.json"         , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 1.00F, {-120.0F, -300.0F}, "walk", "weapon/sword", detachSlotHero} },
	{"spineboy"     , {"spineboy"     , "spineboy-pma.atlas"     , "spineboy-pro.json"     , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.49F, { 260.0F, -300.0F}, "walk", "default", {} }                 },
	{"stretchyman"  , {"stretchyman"  , "stretchyman-pma.atlas"  , "stretchyman-pro.json"  , randomRange(0.0F, 1.0F), 6.0F, 1.0F, 0.60F, { 550.0F, -300.0F}, "walk", "default", {} }                 },
	{"alien"        , {"alien"        , "alien.atlas"            , "alien-pro.json"        , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.60F, { 800.0F, -300.0F}, "run" , "default", {} }                 },
};

SPINE_ATTRIB* FindSpineAttribute(const std::string& spineName)
{
	auto itr = spine_rsc.find(spineName);
	if (itr == spine_rsc.end())
		return nullptr;

	return &itr->second;
}

