
#pragma once
#ifndef __G2_FACTORY_CAMERA_H__
#define __G2_FACTORY_CAMERA_H__

#include <Windows.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "G2.Factory.h"
#include "G2.Camera.h"

using namespace std;

namespace G2 {

struct TD3D_CAMERA
{
	std::string				name;	// camera name
	IG2Camera*				r{};	// resource
	~TD3D_CAMERA() {
		if(r) {
			delete r;
			r = {};
		}
	}
};

class FactoryCamera : public IG2Factory<FactoryCamera, TD3D_CAMERA>
{
public:
	static FactoryCamera* instance();
public:
	TD3D_CAMERA* ResourceLoad(const std::string& name, EG2CAMERA type);
	TD3D_CAMERA* ResourceFind(const std::string& name) const override;
	IG2Camera* FindRes(const std::string& name) const;
};

} // namespace G2

#endif // __G2_FACTORY_CAMERA_H__
