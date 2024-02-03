#pragma once
#include "base/BaseCustomLayer.hpp"

class IntegerInputLayer;

struct IntegerInputInfo
{
	std::string_view title;
	std::string_view input_text;
	int min_value;
	int fallback_value;
	int max_value;
	int starting_value;
	std::function<void(IntegerInputLayer*)> onFinish;

	friend void swap(IntegerInputInfo& lhs, IntegerInputInfo& rhs)
	{
		std::swap(lhs.title, rhs.title);
		std::swap(lhs.input_text, rhs.input_text);
		std::swap(lhs.min_value, rhs.min_value);
		std::swap(lhs.fallback_value, rhs.fallback_value);
		std::swap(lhs.max_value, rhs.max_value);
		std::swap(lhs.starting_value, rhs.starting_value);
		std::swap(lhs.onFinish, rhs.onFinish);
	}
};

class IntegerInputLayer : public BaseCustomLayer
{
public:
	static IntegerInputLayer* create();
	bool init();

	void onClose(CCObject*);
	void onLeftButton(CCObject*);
	void onRightButton(CCObject*);

	void keyDown(enumKeyCodes) override;

	void setup(IntegerInputInfo iili) { m_tmp_iili = iili; };


	int m_integer;

private:
	static inline IntegerInputInfo m_tmp_iili;
	IntegerInputInfo m_iili;
};
