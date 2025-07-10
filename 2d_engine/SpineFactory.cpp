#include <map>
#include <string>
#include <vector>
#include "Common/G2.Util.h"
#include "SpineFactory.h"

using namespace std;

static vector<string> detachSlotSpineBoy = {
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
static vector<string> detachSlotHero = {
	"weapon-morningstar-path",
	"chain-ball", "chain-round", "chain-round2", "chain-round3",
	"chain-flat", "chain-flat2", "chain-flat3", "chain-flat4", "handle"
};

static map<EAPP_MODEL, SPINE_ATTRIB> spine_rsc =
{
	{ EAPP_MODEL::EMODEL_KNIGHT	 , {"hero"         , "hero-pro.atlas"         , "hero-pro.json"         , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 1.00F, {0.0F, 0.0F}, "walk", "weapon/sword", detachSlotHero} },
	{ EAPP_MODEL::EMODEL_BOY	 , {"spineboy"     , "spineboy-pma.atlas"     , "spineboy-pro.json"     , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.49F, {0.0F, 0.0F}, "walk", "default", {} }                 },
	{ EAPP_MODEL::EMODEL_RAPTOR	 , {"raptor"       , "raptor.atlas"           , "raptor-pro.json"       , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.45F, {0.0F, 0.0F}, "walk", "default", detachSlotSpineBoy}  },
	{ EAPP_MODEL::EMODEL_GOBLIN	 , {"goblins"      , "goblins-pma.atlas"      , "goblins-pro.json"      , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 1.25F, {0.0F, 0.0F}, "walk", "goblin" , {} }                 },
	{ EAPP_MODEL::EMODEL_ALIEN	 , {"alien"        , "alien.atlas"            , "alien-pro.json"        , randomRange(0.0F, 1.0F), 1.0F, 1.0F, 0.60F, {0.0F, 0.0F}, "walk", "default", {} }                 },
	{ EAPP_MODEL::EMODEL_STMAN	 , {"stretchyman"  , "stretchyman-pma.atlas"  , "stretchyman-pro.json"  , randomRange(0.0F, 1.0F), 6.0F, 1.0F, 0.60F, {0.0F, 0.0F}, "walk", "default", {} }                 },
};

SPINE_ATTRIB* FactorySpineObject::FindSpineAttribute(EAPP_MODEL model)
{
	auto itr = spine_rsc.find(model);
	if (itr == spine_rsc.end())
		return nullptr;

	return &itr->second;
}


SpineRender* FactorySpineObject::CreateSpine(EAPP_MODEL model, float scale)
{
	auto attr = FindSpineAttribute(model);
	if (!attr)
		return {};

	auto spineObj = new(std::nothrow) SpineRender;
	if (!spineObj)
		return {};

	if (FAILED(spineObj->Init(*attr)))
	{
		delete spineObj;
		return {};
	}

	spineObj->Position({ 0.0F, 0.F });

	return spineObj;
}