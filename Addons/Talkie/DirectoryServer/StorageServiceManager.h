/**
 *	StorageServiceManager.h
 *
 *	-- Talkie Directory Server --
 *
 *	Storage Service Manager of the Directory is 
 *	a manager class that controls storage service
 *	requests and their lifetime.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	February 2007
 */
#ifndef TALKIE_STORAGE_SERVICE_MANAGER_H_
#define TALKIE_STORAGE_SERVICE_MANAGER_H_

#pragma warning(disable: 4267)

#include "Common.h"

#include <boost/asio.hpp>

#define WAKEUP_PERIOD 2

namespace talkie
{
	class StorageService;

	//----------------------------
	//-- class StorageServiceManager

	class StorageServiceManager
	{
	public:
		///--- Singleton Pattern
		static StorageServiceManager& Instance(void) {
			static StorageServiceManager singleton;		///< Mayer's Singleton
			return singleton;
		}

		///--- public API
		void Start(boost::asio::io_service& io_service);

		void InsertService(StorageService* service)		{ running.push_back(service); }
		void RemoveService(StorageService* service)		{ running.erase(std::remove(running.begin(), running.end(), service)); }

	private:
		///--- constructors / destructor
		StorageServiceManager(void) : wakeupTimer(0) {}
		~StorageServiceManager(void) { if (wakeupTimer) delete wakeupTimer; }
		DISABLE_COPY(StorageServiceManager);

		///--- private API
		void onWakeup(const boost::asio::error& error);
		void ResetWakeupTimer(void);

		///--- private members
		typedef std::list<StorageService*> ServiceList;
		ServiceList running;

		boost::asio::deadline_timer* wakeupTimer;
	};
}

#endif	//TALKIE_STORAGE_SERVICE_MANAGER_H_
