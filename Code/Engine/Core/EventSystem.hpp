#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/NamedProperties.hpp"

//#include <algorithm>
#include <mutex>
//#include <cctype>
#include <string>
#include <vector>

//! \file EventSystem.hpp

//! An alias for NamedStrings
typedef NamedProperties EventArgs;

//! An alias for functions that can be used as callback functions for event subscriptions
typedef bool (*EventCallbackFunction)(EventArgs& args);

struct EventSubscriptionBase
{
public:
	virtual ~EventSubscriptionBase() = default;
	virtual bool Execute(EventArgs& args) = 0;

	virtual bool IsMethodSubscription() const { return false; }
	virtual void* GetObjectPointer() const { return nullptr; }
};

/*! \brief A structure for EventSubscriptions
* 
* Currently contains only a pointer to the #EventCallbackFunction, but should later contain a pointer to the object on which a member function should be called.
* No support for member functions right now, only supports global functions or static member functions
* 
*/
struct EventSubscription : public EventSubscriptionBase
{
public:
	virtual bool Execute(EventArgs& args)
	{
		bool consumedEvent = m_callbackFunctionPtr(args);
		return consumedEvent;
	}

public:
	EventCallbackFunction		m_callbackFunctionPtr;
};

template<typename T>
struct EventSubscription_Method : public EventSubscriptionBase
{
	typedef bool (T::*EventCallbackMethod)(EventArgs& args);

public:
	~EventSubscription_Method() = default;
	EventSubscription_Method(T& objectInstance, EventCallbackMethod method)
		: m_object(objectInstance)
		, m_method(method)
	{}

	virtual bool IsMethodSubscription() const override { return true; }
	virtual void* GetObjectPointer() const override { return reinterpret_cast<void*>(&m_object); }

	virtual bool Execute(EventArgs& args) override
	{
		bool consumedEvent = (m_object.*m_method)(args);
		return consumedEvent;
	}

public:
	T& m_object;
	EventCallbackMethod m_method = nullptr;
};

//! An alias for a list of #EventSubscription structures
typedef std::vector<EventSubscriptionBase*> SubscriptionList;

/*! \brief A structure for the configuration to be used for this EventSystem
* 
* Currently contains nothing but still must be passed in to the EventSystem constructor
* 
*/
struct EventSystemConfig
{
};

/*! \brief Handles event subscriptions and firing events
* 
* The EventSystem can be used to subscribe to events with specific names. If an event with that name is fired, the callback function provided is called.
* Any time a function is fired, the callback functions are called in the order they were added. If a callback function reports an event being consumed, no further callback functions are called for that event.
* This system is especially useful for engine code to communicate with game code. Engine code can fire events in certain situations and if a game code function is subscribed to that event, it gets notified. If no functions are subscribed, firing of that event is benign.
* The engine already provides a global EventSystem instance (g_eventSystem). However, game code must initialize this instance before it can be used.
* 
*/
class EventSystem
{
public:
	EventSystem(EventSystemConfig const& config);
	~EventSystem() = default;

	void																Startup();
	void																Shutdown();
	void																BeginFrame();
	void																EndFrame();

	void																SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr, std::string const& helpText = "");
	template<typename T>
	void SubscribeEventCallbackFunction(std::string const& eventName, bool (T::*methodPtr)(EventArgs&), T& objectInstance, std::string const& helpText = "")
	{
		EventSubscription_Method<T>* subscription = new EventSubscription_Method<T>(objectInstance, methodPtr);
		m_subscriptionListMutex.lock();
		m_subscriptionListByEventName[eventName].push_back(subscription);
		m_subscriptionListMutex.unlock();
		m_helpTexts[eventName] = helpText;
	}
	
	void																UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
	
	template<typename T>
	void UnsubscribeEventCallbackFunction(std::string const& eventName, bool (T::*methodPtr)(EventArgs&), T& objectInstance)
	{
		m_subscriptionListMutex.lock();

		auto subscriberListIter = m_subscriptionListByEventName.find(eventName);

		if (subscriberListIter == m_subscriptionListByEventName.end())
		{
			m_subscriptionListMutex.unlock();
			return;
		}

		SubscriptionList& subscriberList = subscriberListIter->second;
		for (int subscriberIndex = 0; subscriberIndex < static_cast<int>(subscriberList.size()); subscriberIndex++)
		{
			EventSubscription_Method<T>* subscription = dynamic_cast<EventSubscription_Method<T>*>(subscriberList[subscriberIndex]);
			if (!subscription)
			{
				continue;
			}
			if (subscription->GetObjectPointer() == &objectInstance && subscription->m_method == methodPtr)
			{
				subscriberList.erase(subscriberList.begin() + subscriberIndex);
				subscriberIndex--;
			}
		}

		if (m_subscriptionListByEventName[eventName].empty())
		{
			m_subscriptionListByEventName.erase(eventName);
			m_helpTexts.erase(eventName);
		}

		m_subscriptionListMutex.unlock();
	}

	template<typename T>
	void UnsubscribeAllEventCallbackFunctionsForObject(T& objectInstance)
	{
		m_subscriptionListMutex.lock();
		for (auto subscriptionListIter = m_subscriptionListByEventName.begin(); subscriptionListIter != m_subscriptionListByEventName.end();)
		{
			SubscriptionList& subscriberList = subscriptionListIter->second;
			std::string eventName = subscriptionListIter->first;
			for (int subscriberIndex = 0; subscriberIndex < static_cast<int>(subscriberList.size()); subscriberIndex++)
			{
				//EventSubscription_Method<T>* subscription = dynamic_cast<EventSubscription_Method<T>*>(subscriberList[subscriberIndex]);
				//if (!subscription)
				//{
				//	continue;
				//}

				if (subscriberList[subscriberIndex]->IsMethodSubscription() && subscriberList[subscriberIndex]->GetObjectPointer() == &objectInstance)
				{
					subscriberList.erase(subscriberList.begin() + subscriberIndex);
					subscriberIndex--;
				}
			}
			if (m_subscriptionListByEventName[eventName].empty())
			{
				subscriptionListIter = m_subscriptionListByEventName.erase(subscriptionListIter);
				m_helpTexts.erase(eventName);
			}
			else
			{
				++subscriptionListIter;
			}
		}

		m_subscriptionListMutex.unlock();
	}
	
	void																FireEvent(std::string const& eventName, EventArgs& args);
	void																FireEvent(std::string const& eventName);

	void																ListAllCommands() const;
	std::map<std::string, std::string, cmpCaseInsensitive>				GetAllCommandsList() const;

protected:
	//! The configuration used for this EventSystem
	EventSystemConfig													m_config;
	mutable std::mutex													m_subscriptionListMutex;
	//! A mapping of event names to a #SubscriptionList, uses the case-insensitive comparator for event names
	std::map<std::string, SubscriptionList, cmpCaseInsensitive>			m_subscriptionListByEventName;
	//! A mapping of event names to help texts, uses the case-insensitive comparator for event names
	std::map<std::string, std::string, cmpCaseInsensitive>				m_helpTexts;
};


void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr, std::string const& helpText = "");

template<typename T>
void SubscribeEventCallbackFunction(std::string const& eventName, bool (T::*methodPtr)(EventArgs&), T& objectInstance, std::string const& helpText = "")
{
	if (g_eventSystem)
	{
		g_eventSystem->SubscribeEventCallbackFunction(eventName, methodPtr, objectInstance, helpText);
	}
}

template<typename T>
void UnsubscribeEventCallbackFunction(std::string const& eventName, bool (T::* methodPtr)(EventArgs&), T& objectInstance)
{
	if (g_eventSystem)
	{
		g_eventSystem->UnsubscribeEventCallbackFunction(eventName, methodPtr, objectInstance);
	}
}

template<typename T>
void UnsubscribeAllEventCallbackFunctionsForObject(T& objectInstance)
{
	if (g_eventSystem)
	{
		g_eventSystem->UnsubscribeAllEventCallbackFunctionsForObject(objectInstance);
	}
}

void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
void FireEvent(std::string const& eventName, EventArgs& args);
void FireEvent(std::string const& eventStr);

class EventRecipient
{
public:
	virtual ~EventRecipient()
	{
		UnsubscribeAllEventCallbackFunctionsForObject(*this);
	}
};
