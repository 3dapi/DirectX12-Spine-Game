#pragma once
#ifndef __G2_SINGLETONBASE_H__
#define __G2_SINGLETONBASE_H__

namespace G2 {

template<typename T>
class SingletonBase {
protected:
	inline static T* _inst{};
protected:
	SingletonBase() = default;
	virtual ~SingletonBase() = default;
public:
	static T* instance() {
		return _inst;
	}

	static void setInstance(T* inst) {
		if(_inst)
			delete _inst; // 기존 객체 제거 (선택적)
		_inst = inst;
	}

	static void releaseInstance() {
		delete _inst;
		_inst = nullptr;
	}

	SingletonBase(const SingletonBase&) = delete;
	SingletonBase& operator=(const SingletonBase&) = delete;
};

} // namespace G2

#endif // __G2_FRAME_CONSTS_H__
