#include "Timer.h"
#include "Utility.h"

namespace Snail
{

	float getSeconds(const std::chrono::duration<float> &duration)
	{
		return duration.count();
	}

	TimeData::TimeData(float _actual, float _target)
		: actual(_actual), target(_target)
	{

	}

	std::string TimeData::stringify() const
	{
		return "{ actual: " + toStr(actual) + "; target: " + toStr(target) + "}";
	}

	ProfileData::ProfileData(float _actual, float _percent)
		: actual(_actual), percent(_percent)
	{

	}

	std::string ProfileData::stringify() const
	{
		return "{ actual: " + toStr(actual) + "; target: " + toStr(percent) + "}";
	}

	void Time::init()
	{

	}

	void Time::update()
	{
		for (Timer &timer : timers)
			if (timer.isActive)
			{
				timer.duration.actual += dt.actual;

				if (timer.duration.actual > timer.duration.target ||
					Util::isEqual(timer.duration.actual, timer.duration.target))
				{
					timer.callback();

					if (timer.isRecurring)
						timer.duration.actual = 0.f;
					else
						timer.isActive = false;
				}

			}
	}

	void Time::free()
	{

	}

	void Time::setFps(float _fps)
	{
		crashIf(_fps <= 0.f, "FPS must be more than 0.f");

		fps.target = _fps;
		dt.target = 1.f / fps.target;
	}

	TimeData Time::getFps()
	{
		return fps;
	}

	void Time::beginDt()
	{
		loopStart = NOW;
	}

	void Time::endDt()
	{
		prevProfileData = currProfileData;
		dt.actual = getSeconds(NOW - loopStart);

		// calculate percentage usage of each system
		for (auto &[label, data] : prevProfileData)
			data.percent = data.actual / dt.actual;

		// wait for target dt to be reached
		if (dt.target)
			while (dt.actual < dt.target)
				dt.actual = getSeconds(NOW - loopStart);
	}

	TimeData Time::getDt()
	{
		return dt;
	}

	void Time::beginProfile()
	{
		profileStart = NOW;
	}

	void Time::endProfile(const std::string &label)
	{
		currProfileData[label] = ProfileData{ getSeconds(NOW - profileStart) };
	}

	const std::unordered_map<std::string, ProfileData> &Time::getProfileData()
	{
		return prevProfileData;
	}

	Timer::Timer(float lifespan, std::function<void()> _callback, std::optional<EntityId> _entityId,
		bool _isRecurring)
		: isActive(true), isRecurring(_isRecurring), duration{ 0.f, lifespan }, entityId(_entityId), 
		callback(_callback)
	{

	}

	std::string Timer::stringify() const
	{
		std::ostringstream oss;
		oss << std::boolalpha << "{ isActive: " << isActive << "; isRecurring: " << isRecurring << "; duration: "
			<< duration << "; entityId: " << (entityId ? toStr(*entityId) : "-")
			<< "; callback: " << Util::getFunctionSignature(callback) << " }";
		return oss.str();
	}

	Timer *Time::addTimer(float lifespan, std::function<void()> callback, std::optional<EntityId> entityId, 
		bool isRecurring)
	{
		for (Timer &timer : timers)
			if (!timer.isActive)
			{
				timer = Timer(lifespan, callback, entityId, isRecurring);
				return &timer;
			}

		timers.emplace_back(lifespan, callback, entityId, isRecurring);
		return &timers.back();
	}

	void Time::stopTimer(EntityId id)
	{
		for (Timer &timer : timers)
			if (timer.entityId && *timer.entityId == id)
				timer.isActive = false;
	}

}