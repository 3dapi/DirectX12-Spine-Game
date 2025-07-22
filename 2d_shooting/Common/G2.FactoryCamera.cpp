#include <algorithm>
#include <filesystem>
#include "G2.ConstantsWin.h"
#include "G2.FactoryCamera.h"
#include "G2.Util.h"

using namespace std;
namespace G2 {

FactoryCamera* FactoryCamera::instance()
{
	static FactoryCamera inst;
	return &inst;
}

TD3D_CAMERA* FactoryCamera::ResourceLoad(const std::string& name, EG2CAMERA type)
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}

	// load
	auto camera = IG2Camera::create(name, type);
	if (!camera)
		return {};

	auto pItem = std::make_unique<TD3D_CAMERA>();
	pItem->name = name;
	pItem->r    = camera;

	auto [it, success] = m_db.insert({ name, std::move(pItem) });
	auto ret = it->second.get();
	return ret;
}


TD3D_CAMERA* FactoryCamera::ResourceFind(const std::string& name) const
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get();
	}
	static TD3D_CAMERA dummy{ "<none>"};
	return &dummy;
}

IG2Camera* FactoryCamera::FindRes(const std::string& name) const
{
	auto itr = this->m_db.find(name);
	if (itr != this->m_db.end())
	{
		return itr->second.get()->r;
	}
	return {};
}

} // namespace G2
