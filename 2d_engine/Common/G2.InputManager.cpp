//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// InputManager

#include <Windows.h>
#include "G2.InputManager.h"

using namespace std;

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

void InputManager::Update()
{
	// Keyboard debugging
	//for (int i = 0; i < EAPP_MAX_KEY; ++i)
	//{
	//	if (GetAsyncKeyState(i) & 0x8000)
	//	{
	//		printf("VK %d (0x%02X) is down\n", i, i);
	//	}
	//}

	m_hasEvent = false;
	memcpy(m_keyOld, m_keyNew, EAPP_MAX_KEY);
	for(int i = 0; i < EAPP_MAX_KEY; ++i)
	{
		if(i == 21)
			continue;
		m_keyNew[i] = (GetAsyncKeyState(i) & 0x8000) ? 1 : 0;
		if(0 == m_keyOld[i] && 0 == m_keyNew[i])		// no event
		{
			m_keyCur[i] = (int)EAPP_INPUT_NONE;
		}
		else if(0 == m_keyOld[i] && 1 == m_keyNew[i])		// down
		{
			m_keyCur[i] = (int)EAPP_INPUT_DOWN;
		}
		else if(1 == m_keyOld[i] && 1 == m_keyNew[i])		// pressed
		{
			m_keyCur[i] = (int)EAPP_INPUT_PRESS;
		}
		else if(1 == m_keyOld[i] && 0 == m_keyNew[i])		// up
		{
			m_keyCur[i] = (int)EAPP_INPUT_UP;
		}
		if(!m_hasEvent && m_keyCur[i] != (int)EAPP_INPUT_NONE)
			m_hasEvent = true;
	}
}
