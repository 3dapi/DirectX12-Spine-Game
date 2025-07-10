#pragma once
#ifndef _SpineFactory_H_
#define _SpineFactory_H_

#include "AppCommon.h"
#include "SpineRender.h"

class FactorySpineObject
{
public:
	static SPINE_ATTRIB* FindSpineAttribute(EAPP_MODEL model);
	SpineRender* CreateSpine(EAPP_MODEL model, float scale = 1.0F);
};

#endif // _SpineFactory_H_

