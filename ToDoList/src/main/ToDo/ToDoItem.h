#include <chrono>
#include <string>
#include <iostream>

class IToDoItem
{
public:
	IToDoItem(std::chrono::time_point<std::chrono::system_clock> timePoint_, std::string message_)
		: timePoint{ timePoint_ }, message{ message_ }
	{
	}
	std::chrono::time_point<std::chrono::system_clock> timePoint;
	std::string message;
	virtual bool IsPeriodic() const = 0;
	virtual std::chrono::seconds getPeriod() const = 0;
	virtual std::ostream& operator<<(std::ostream& os) = 0;
};

struct ToDoItem : public IToDoItem
{
	ToDoItem(std::chrono::time_point<std::chrono::system_clock> timePoint_, std::string message_)
		:IToDoItem(timePoint_, message_)
	{
	}

	bool IsReady() const { return timePoint <= std::chrono::system_clock::now(); }
	bool IsPeriodic() const { return false; }
	std::chrono::seconds getPeriod() const { return std::chrono::seconds{ 0 }; }
	bool operator > (const ToDoItem& item) const { return timePoint > item.timePoint; }

	virtual std::ostream& operator<<(std::ostream& os)
	{
		os << std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch()).count()
			<< ' ' << message << std::endl;
		return os;
	}


};

struct ToDoPeriodicItem : public ToDoItem
{
	ToDoPeriodicItem(std::chrono::time_point<std::chrono::system_clock> timePoint_,
		std::string message_,
		unsigned int  period_, char periodMeasure = 's')
		: ToDoItem(timePoint_, message_), period{ period_ }, periodMeasure{ periodMeasure }
	{
	}
	unsigned int period;
	char periodMeasure;

	bool IsPeriodic() const { return true; }
	std::chrono::seconds getPeriod() const
	{
		switch (periodMeasure)
		{
		case 's': return std::chrono::seconds{ period }; break;
		case 'm': return std::chrono::seconds{ period * 60}; break;
		case 'h': return std::chrono::seconds{ period * 60 * 60}; break;
		case 'd': return std::chrono::seconds{ period * 60 * 60 * 24}; break;
		default: return std::chrono::seconds{ 0 }; break;
		}
	}
	std::ostream& operator<<(std::ostream& os) override
	{
		os << std::chrono::system_clock::to_time_t(timePoint)
			<< ' ' << message << ' ' << period << std::endl;
		return os;
	}

};

