
#pragma once
#ifndef __G2_FACTORY_SPINE_H__
#define __G2_FACTORY_SPINE_H__

#include <Windows.h>
#include <spine/spine.h>
#include "G2.Factory.h"

namespace G2 {

struct TD3D_SPINE
{
	std::string				name		;	// spine name
	std::string				fileAtlas	;	// atlas file
	std::string				fileSkel	;	// skelecton file
	spine::Atlas*			atlas		{};
	spine::SkeletonData*	skelData	{};
	~TD3D_SPINE() {
		if (atlas) {
			// crash 방지.
			atlas->_textureLoader = nullptr;
			delete atlas;
			atlas = {};
		}
		if (skelData) {
			delete skelData;
			skelData = {};
		}
	}
};

class FactorySpine : public IG2Factory<FactorySpine, TD3D_SPINE>, public spine::TextureLoader
{
public:
	static FactorySpine* instance();
public:
	TD3D_SPINE* ResourceLoad(const std::string& name, const std::string& fileAtlas, const std::string& fileSkel);
	TD3D_SPINE* ResourceFind(const std::string& name) const override;
	int ResourceUnLoad(const std::string& name)         override;
	int ResourceUnLoadAll()                             override;
	// find ID3D12Resource*
	TD3D_SPINE* FindRes(const std::string& name) const;

protected:
	void	load(spine::AtlasPage& page, const spine::String& path) override;
	void	unload(void* texture) override;
};

} // namespace G2

#endif // __G2_FACTORY_SPINE_H__

