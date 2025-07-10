#pragma once
#ifndef _SceneBegin_H_
#define _SceneBegin_H_

#include <map>
#include <vector>
#include <any>

#include "Common/G2.Constants.h"
#include "Common/G2.Geometry.h"
#include "common/G2.ConstantsWin.h"
#include "common/G2.FactorySpine.h"
#include "common/G2.Util.h"
#include "AppCommon.h"
#include "AppCommonXTK.h"
#include "SpineFactory.h"
#include "UiBegin.h"

using namespace std;
using namespace G2;

class SceneBegin: public G2::IG2Scene
{
protected:
	vector<unique_ptr<SpineRender> >	m_char	;
	UiBegin*							m_pUi	{};

public:
	SceneBegin();
	virtual ~SceneBegin();

	// IG2Scene
	int		Type()						override { return (int)EAPP_SCENE::EAPP_SCENE_BEGIN; }
	int		Init(const std::any& ={})	override;
	int		Destroy()					override;
	int		Update(const std::any& t)	override;
	int		Render()					override;
	int		Notify(const std::string& name, const std::any& t)	override;
};

#endif
