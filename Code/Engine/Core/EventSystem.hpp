#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"

//#include <algorithm>
#include <mutex>
//#include <cctype>
#include <string>

//! \file EventSystem.hpp

//! An alias for NamedStrings
typedef NamedStrings EventArgs;

//! An alias for functions that can be used as callback functions for event subscriptions
typedef bool (*EventCallbackFunction)(EventArgs& args);

/*! \brief A structure for EventSubscriptions
* 
* Currently contains only a pointer to the #EventCallbackFunction, but should later contain a pointer to the object on which a member function should be called.
* No support for member functions right now, only supports global functions or static member functions
* 
*/
struct EventSubscription
{
public:
	EventCallbackFunction		m_callbackFunctionPtr;
};

//! An alias for a list of #EventSubscription structures
typedef std::vector<EventSubscription> SubscriptionList;

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
	void																UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
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
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
void FireEvent(std::string const& eventName, EventArgs& args);
void FireEvent(std::string const& eventStr);
