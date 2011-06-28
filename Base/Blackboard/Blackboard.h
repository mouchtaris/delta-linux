/**
 *	Blackboard.h
 *
 *	-- Blackboard Design Pattern --
 *
 *	Blackboard class implements the blackboard pattern
 *	providing shared memory storage for message exchange
 *	among different architectural components that are not
 *	tightly coupled.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	March 2007
 */
#ifndef COMM_BLACKBOARD_H_
#define COMM_BLACKBOARD_H_

#include "Common.h"
#include "Singleton.h"
#include "Adaptors.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <utility>
#include <algorithm>

#include <list>
#include <map>

namespace comm
{
	//----------------------------
	//-- class Blackboard

	template <class MessageType>
	class Blackboard :
		public util::Singleton
	{
		///--- Singleton Pattern
		INLINE_SINGLETON(Blackboard);

	public:
		///--- type definitions
		typedef MessageType MessageType;
		typedef std::list<MessageType> MessageList;

		///--- inner classes
		struct Monitor {
			virtual void OnMessage(const MessageType& msg) =0;
		};

		///--- type definitions
		typedef boost::function<bool (const MessageType&)> Importer;

		///--- public API
		void RegisterImporter(const std::string& type, Importer importer) {
			importers.insert(std::make_pair(type, importer));
		}

		void Export(MessageList& results, const MessageType& msg)
		{
			//struct Exporter :
			//	public std::unary_function<Importer, void>
			//{
			//	Exporter(const MessageType& msg)
			//		: msg(msg) {}

			//	result_type operator ()(argument_type importer) {
			//		importer(msg);
			//	}

			//private:
			//	const MessageType& msg;
			//};

			//std::for_each(
			//	monitors.begin(),
			//	monitors.end(),
			//	boost::bind(&Monitor::OnMessage, _1, msg)
			//);

			//std::for_each(
			//	importers.lower_bound(msg.GetDst()),
			//	importers.upper_bound(msg.GetDst()),
			//	util::project2nd<ImportingMapping::value_type>(
			//		Exporter(msg)
			//	)
			//);
		}

	private:
		///--- constructors / destructor
		Blackboard(void) {}
		~Blackboard(void) {}

		///--- private API

		///--- private members
		typedef std::multimap<std::string, Importer> ImportingMapping;
		ImportingMapping importers;

		typedef std::list<Monitor*> MonitorList;
		MonitorList monitors;
	};

	template <class MessageType>
	DEFINE_SINGLETON_INSTANCE(Blackboard<MessageType>);
}

#endif	//COMM_BLACKBOARD_H_
