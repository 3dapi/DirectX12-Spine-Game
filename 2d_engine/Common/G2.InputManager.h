
#pragma once
#include <bitset>
#include "Common/G2.Constants.h"
#include "Common/G2.SingletonBase.h"
using std::bitset;
using namespace G2;

class InputManager : public SingletonBase<InputManager>
{
protected:
	uint8_t		m_keyOld[EAPP_MAX_KEY]{};
	uint8_t		m_keyNew[EAPP_MAX_KEY]{};
	uint8_t		m_keyCur[EAPP_MAX_KEY]{};
	bool		m_hasEvent	{};
public:
	InputManager();
	virtual ~InputManager();
	void	Update();
	bool	hasEvent() const { return m_hasEvent; }
	const uint8_t* Key() const {return m_keyCur; }

};
