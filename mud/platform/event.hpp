#ifndef EVENT_HPP
#define EVENT_HPP

#include <set>

namespace mud
{
	template<typename ... TParams>
	class IEventListenerCallback
	{
	public:

		virtual void operator() (TParams ... args) = 0;
	};

	
	template<typename ... TParams>
	class EventListenerCallbackFree : public IEventListenerCallback<TParams ...>
	{
	public:

		using TFunc = void(*)(TParams ...);

		EventListenerCallbackFree(TFunc callback)
			: m_callback(callback)
		{ }

		virtual void operator() (TParams ... args) override
		{
			(*m_callback)(args...);
		}

	private:

		TFunc m_callback;
	};

	template <typename TListener, typename ... TParams>
	class EventListenerCallback : public IEventListenerCallback<TParams ...>
	{
	public:

		using TFunc = void(TListener:: *)(TParams ...);

		EventListenerCallback(TListener * listener, TFunc callback)
			: m_listener(listener), m_callback(callback)
		{ }

		virtual void operator() (TParams ... args) override
		{
			(m_listener->*m_callback)(args...);
		}

	private:

		TListener * m_listener;
		TFunc m_callback;
	};

	template <typename ... TParams>
	class Event
	{
	public:

		using TIListenerCallback = IEventListenerCallback<TParams ...>;

		template <typename TListener>
		using TListenerCallback = EventListenerCallback<TListener, TParams ...>;

		using TListenerCallbackFree = EventListenerCallbackFree<TParams ...>;

		void addListener(TIListenerCallback * listenerCallback)
		{
			m_listenerCallbacks.insert(listenerCallback);
		}

		template <typename TListener>
		void addListener(TListenerCallback<TListener> * listenerCallback)
		{
			TIListenerCallback * callbackInterface = reinterpret_cast<TIListenerCallback *>(listenerCallback);
			addListener(callbackInterface);
		}

		void removeListener(TIListenerCallback * listenerCallback)
		{
			m_listenerCallbacks.erase(listenerCallback);
		}

		template <typename TListener>
		void removeListener(TListenerCallback<TListener> * listenerCallback)
		{
			TIListenerCallback * callbackInterface = reinterpret_cast<TIListenerCallback *>(listenerCallback);
			removeListener(callbackInterface);
		}

		void broadcast(TParams ... args)
		{
			for (auto listenerCallback : m_listenerCallbacks)
				(*listenerCallback)(args...);
		}

	private:

		std::set<TIListenerCallback *> m_listenerCallbacks;
	};
}

#endif