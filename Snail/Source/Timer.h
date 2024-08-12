#pragma once

#include "System.h"
#include "Utility.h"

#include <unordered_map>
#include <chrono>
#include <optional>
#include <functional>

namespace Snail
{

	struct TimeData : public Debugger::Printable
	{
		float actual;
		float target;

		TimeData(float _actual = 0.f, float _target = 0.f);

		std::string stringify() const override;
	};

	struct ProfileData : public Debugger::Printable
	{
		float actual;
		float percent;

		ProfileData(float _actual = 0.f, float target = 0.f);

		std::string stringify() const override;
	};

	struct Timer : public Debugger::Printable
	{
		bool isActive;
		bool isRecurring;
		TimeData duration;
		std::optional<EntityId> entityId;
		std::function<void()> callback;

		Timer(float lifespan, std::function<void()> _callback, std::optional<EntityId> _entityId = std::nullopt,
			bool _isRecurring = false);

		std::string stringify() const override;
	};

	class Time : public System
	{
		TimeData fps;
		TimeData dt{ 1.f / 60.f };

		std::unordered_map<std::string, ProfileData> prevProfileData; // one frame before
		std::unordered_map<std::string, ProfileData> currProfileData; // current frame
		std::chrono::steady_clock::time_point loopStart;
		std::chrono::steady_clock::time_point profileStart;

		std::vector<Timer> timers;

	public:

		void init() override;
		void update() override;
		void free() override;

		void setFps(float _fps);
		TimeData getFps();
		
		void beginDt();
		void endDt();
		TimeData getDt();

		void beginProfile();
		void endProfile(const std::string &label);
		const std::unordered_map<std::string, ProfileData> &getProfileData();

		Timer *addTimer(float lifespan, std::function<void()> callback,
			std::optional<EntityId> entityId = std::nullopt, bool isRecurring = false);
		void stopTimer(EntityId id);
	};

}