#pragma once

#include <cmath>

#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/cocos/misc_nodes/CCClippingNode.h>
#include <Geode/cocos/draw_nodes/CCDrawNode.h>
#include <Geode/cocos/label_nodes/CCLabelBMFont.h>

class RLProgressBar : public cocos2d::CCSprite
{
public:
	static RLProgressBar* create();
	static RLProgressBar* create(float, bool);

	virtual bool init() override;
	bool init(float, bool);

	virtual void setColor(const cocos2d::ccColor3B&) override;

	float getProgress() const { return m_progress; }
	void setProgress(float);

	void playFillingAnimation();

private:
	// filling and its clipper
	cocos2d::CCClippingNode* m_clipper;
	cocos2d::CCDrawNode* m_stencil;
	cocos2d::CCSprite* m_filling;

	cocos2d::CCLabelBMFont* m_label;

	float m_progress = .0f;

	// animation members and methods
	float m_animationDuration = .0f;
	float m_animationElapsed = .0f;
	bool m_isAnimationFirstTick = false;
	virtual void update(float) override;

	void updateClipper(float = -1.f);


	static constexpr float calcAnimationDuration(float progress)
	{
		// starts at (0, 0.174) and eases to (1, 1.569)
		// f(p) = 1 - (1.1 - p)^3 + 0.505
		return std::clamp(1.f - std::pow(1.1f - progress, 3.f) + .505f, .1f, 1.5f);
	}

	static constexpr float calcAnimationFactorAtTime(float time)
	{
		// wrap time into [0; 1.5]
		time = std::fmod(time, 1.5f);
		if (time < .0f) time += 1.5f;
 
		// f(t) = ((1/.75) * t)^2 * (3 - 2((1/.75) * t)) for x [0; 0.75[ (smoothstep function)
		// f(t) = 4(1 - (2/3) * t)^2 * (3 - 4(1 - (2/3) * t)) for x [0.75; 1.5] (mirror of smoothstep function)
		float factor;
		if (time < .75f)
			factor = std::pow((4.f / 3.f) * time, 2.f) * (3.f - 2.f * ((4.f / 3.f) * time));
		else
			factor = 4.f * std::pow(1.f - (2.f / 3.f) * time, 2.f) * (3.f - 4.f * (1.f - (2.f / 3.f) * time));

		// clamp the factor since we multiply delta by it (multiply by 0 and the animation will never end)
		return std::clamp(factor, .2f, 1.5f);
	}
};
