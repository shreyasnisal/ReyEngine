#include "Engine/Core/EventSystem.hpp"

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


/*! \brief Constructor for the EventSystem
* 
* Sets the configuration of this EventSystem to the provided EventSystemConfig object
*/
EventSystem::EventSystem(EventSystemConfig const& config)
	: m_config(config)
{
}

/*! \brief Startup method for the EventSystem
* 
* Currently does nothing but should be called by when the App starts
* 
*/
void EventSystem::Startup()
{
}

/*! \brief Method that should be called by game code at the beginning of every frame
* 
* Currently does nothing but should still be called by the App at the beginning of each frame
* 
*/
void EventSystem::BeginFrame()
{
}

/*! \brief Method that should be called by game code at the end of every frame
*
* Currently does nothing but should still be called by the App at the end of each frame
*
*/
void EventSystem::EndFrame()
{
}

/*! \brief Shutdown method for the EventSystem
*
* Currently does nothing but should still be called by the App when shutting down all systems (normally before shutting down the application or resetting all systems)
*
*/
void EventSystem::Shutdown()
{
}

/*! \brief Adds a callback function to the list of functions that should be called when an event is fired
* 
* Although this method is publicly visible, it should not be used. Instead, the global SubscribeEventCallbackFunction should be used as a safer alternative to this method.
* 
* \param eventName The name of the event to which the provided callback function should be subscribed
* \param functionPtr An #EventCallbackFunction indicating the function to be notified when an event with the provided name is fired
* \param helpText (Optional) Additional information about the subscriptions that should be shown when registered commands are listed to the console
* \sa ListAllCommands
* \sa <a href="_event_system_8hpp.html#afbb35503f039bcd4460c38f411120e74">SubscribeEventCallbackFunction</a>
* 
*/
void EventSystem::SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr, std::string const& helpText)
{
	EventSubscription* subscription = new EventSubscription();
	subscription->m_callbackFunctionPtr = functionPtr;
	m_subscriptionListMutex.lock();
	m_subscriptionListByEventName[eventName].push_back(subscription);
	m_subscriptionListMutex.unlock();
	m_helpTexts[eventName] = helpText;
}

/*! \brief Removes a callback function from the list of functions that should be called when an event is fired
* 
* Although this method is publicly visible, it should not be used. Instead, the global UnsubscribeEventCallbackFunction should be used as a safer alternative to this method.
* \param eventName The name of the event to unsubscribe from
* \param functionPtr A pointer to an #EventCallbackFunction indicating which function to remove from the subscriptions to the provided event
* \sa <a href="_event_system_8hpp.html#a90a14d5968c238f2c34861d63f5256e5">UnsubscribeEventCallbackFunction</a>
* 
*/
void EventSystem::UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr)
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
		EventSubscription* subscription = dynamic_cast<EventSubscription*>(subscriberList[subscriberIndex]);
		if (!subscription)
		{
			continue;
		}
		if (subscription->m_callbackFunctionPtr == functionPtr)
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

/*! \brief Fires an event with the provided name, passing the provided arguments
*
* Although this method is publicly visible, it should not be used. Instead, the global function ::FireEvent(std::string const&, EventArgs&) should be used as a safer alternative to this method.
* \param eventName The name of the event to fire
* \param args An #EventArgs structure containing arguments to pass to all functions subscribed to this event
* \sa <a href="_event_system_8hpp.html#a73faa2f0ca85c201e50e09d41ae2c42b">FireEvent(std::string const&, EventArgs&)</a>
* 
*/
void EventSystem::FireEvent(std::string const& eventName, EventArgs& args)
{
	m_subscriptionListMutex.lock();

	auto subscriberListIter = m_subscriptionListByEventName.find(eventName);
	if (subscriberListIter == m_subscriptionListByEventName.end())
	{
		if (g_console)
		{
			g_console->AddLine(DevConsole::ERROR, Stringf("%s is not recognized as a command", eventName.c_str()));
		}
		else
		{
			DebuggerPrintf(Stringf("%s is not recognized as a command", eventName.c_str()).c_str());
		}
		m_subscriptionListMutex.unlock();
		return;
	}

	m_subscriptionListMutex.unlock();

	SubscriptionList subscriberList = subscriberListIter->second;
	for (int subscriberIndex = 0; subscriberIndex < static_cast<int>(subscriberList.size()); subscriberIndex++)
	{
		if (subscriberList[subscriberIndex]->Execute(args))
		{
			break;
		}
	}
}

/*! \brief Fires an event with the provided name with an empty #EventArgs structure
*
* Although this method is publicly visible, it should not be used. Instead, the global function ::FireEvent(std::string const&, EventArgs&) should be used as a safer alternative to this method.
* \param eventName The name of the event to fire
* \sa <a href="_event_system_8hpp.html#a69136df3624c27e1c5be55180a6d3c60">FireEvent(std::string const&)</a>FireEvent(std::string const&)
*
*/
void EventSystem::FireEvent(std::string const& eventName)
{
	EventArgs emptyArgs;
	FireEvent(eventName, emptyArgs);
}

/*! \brief Lists all registered commands to the console
* 
* Iterates through the list of registered commands (using m_subscriptionListByEventName) and lists them on the console by adding a line for each command. If a help text for the command is provided, the help text is also appended to the line to be displayed on the console.
* Excludes commands used internally (WM_CHAR, WM_KEYDOWN, WM_KEYUP) from the list.
* 
*/
void EventSystem::ListAllCommands() const
{
	m_subscriptionListMutex.lock();
	if (g_console)
	{
		g_console->AddLine("For more information on commands, type `<command> help`", false);
	}
	for (auto command = m_subscriptionListByEventName.begin(); command != m_subscriptionListByEventName.end(); ++command)
	{
		if (!strcmp(command->first.c_str(), "WM_CHAR") ||
			!strcmp(command->first.c_str(), "WM_KEYDOWN") ||
			!strcmp(command->first.c_str(), "WM_KEYUP") || 
			!strcmp(command->first.c_str(), "WM_MOUSEWHEEL"))
		{
			continue;
		}
		std::string devConsoleLine = "";
		devConsoleLine += Stringf("%-20s", command->first.c_str());
		auto helpTextsIter = m_helpTexts.find(command->first);
		if (helpTextsIter == m_helpTexts.end() || !strcmp(helpTextsIter->second.c_str(), ""))
		{
			devConsoleLine += "No command information available";
		}
		else
		{
			devConsoleLine += helpTextsIter->second;
		}
		if (g_console)
		{
			g_console->AddLine(Rgba8::GREEN, devConsoleLine, false);
		}
	}
	m_subscriptionListMutex.unlock();
}

std::map<std::string, std::string, cmpCaseInsensitive> EventSystem::GetAllCommandsList() const
{
	return m_helpTexts;
}

/*! A global function to subscribe to an event
* 
* In general, only this function should be used and not the member function (EventSystem#SubscribeEventCallbackFunction). This global function ensures that an attempt to subscribe to an event does not cause crashes if the global EventSystem (g_eventSystem) has not been initialized.
* \param eventName The name of the event to subscribe to
* \param functionPtr A pointer to an #EventCallbackFunction indicating the function that should be called when an event with the provided name is fired
* \param helpText (Optional) A help text that should be shown when listing this event in the console. Note that any helpText provided for an existing event overwrites the existing helpText for that event
* \sa EventSystem#SubscribeEventCallbackFunction
* 
*/
void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr, std::string const& helpText)
{
	if (g_eventSystem)
	{
		g_eventSystem->SubscribeEventCallbackFunction(eventName, functionPtr, helpText);
	}
}

/*! A global function to unsubscribe from an event
* 
* In general, only this function should be used and not the member function (EventSystem#UnsubscribeEventCallbackFunction). This global function ensures that an attempt to unsubscribe to an event does not cause crashes if the global EventSystem (g_eventSystem) has not been initialized.
* \param eventName The name of the event to unsubscribe from
* \param functionPtr A pointer to an #EventCallbackFunction indicating which function to remove from the subscriptions to the provided event
* \sa EventSystem#UnsubsrcibeEventCallbackFunction
* 
*/
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr)
{
	if (g_eventSystem)
	{
		g_eventSystem->UnsubscribeEventCallbackFunction(eventName, functionPtr);
	}
}

/*! \brief A global function to fire an event
* 
* In general, only this function should be used and not the member function (EventSystem#FireEvent(std::string const&, EventArgs&)). This global function ensures that an attempt to fire an event does not cause crashes if the global EventSystem (g_eventSystem) has not been initialized.
* \param eventName The name of the event to fire
* \param args An #EventArgs structure containing arguments to pass to all functions subscribed to this event
* \sa EventSystem#FireEvent(std::string const&, EventArgs&)
* 
*/
void FireEvent(std::string const& eventName, EventArgs& args)
{
	if (g_eventSystem)
	{
		g_eventSystem->FireEvent(eventName, args);
	}
}

/*! \brief A global function to fire an event
* 
* In general, only this function should be used and not the member function (EventSystem#FireEvent(std::string const&). This global functions ensures that an attempt to fire an event does not cause crashes if the global EventSystem (g_eventSystem) has not been initialized.
* \param eventName The name of the event to fire
* \sa EventSystem#FireEvent(std::string const&)
* 
*/
void FireEvent(std::string const& eventStr)
{
	//if (g_eventSystem)
	//{
	//	g_eventSystem->FireEvent(eventName);
	//}

	if (!g_eventSystem)
	{
		return;
	}

	Strings commandNameAndArgs;
	int numArgs = SplitStringOnDelimiter(commandNameAndArgs, eventStr, ' ');
	std::string commandName = commandNameAndArgs[0];
	EventArgs eventArgs;
	for (int argIndex = 1; argIndex < numArgs; argIndex++)
	{
		Strings keyValuePair;
		SplitStringOnDelimiter(keyValuePair, commandNameAndArgs[argIndex], '=');

		if (keyValuePair.size() == 1)
		{
			keyValuePair.push_back("true");
		}

		eventArgs.SetValue(keyValuePair[0], keyValuePair[1]);
	}
	g_eventSystem->FireEvent(commandName, eventArgs);
}
