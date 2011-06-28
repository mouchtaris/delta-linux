/**
 *	StorageServiceManager.cpp
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
#include "StorageServiceManager.h"
#include "StorageService.h"

#include <boost/lambda/lambda.hpp>

namespace talkie
{
	//-------------------------------------------------------//
	//---- class StorageServiceManager ----------------------//

	void StorageServiceManager::Start(boost::asio::io_service& io_service)
	{
		//-- Construct timers
		wakeupTimer = new boost::asio::deadline_timer(
			io_service, boost::posix_time::seconds(0)
		);

		ResetWakeupTimer();
	}

	//-----------------------------------------------------------------------

	void StorageServiceManager::onWakeup(const boost::asio::error& error)
	{
		//-- Reset timer
		ResetWakeupTimer();

		//-- Remove finished services
		ServiceList::iterator it =
			std::remove_if(
				running.begin(), running.end(),
				boost::bind(&StorageService::IsDone, _1)
			);
		//std::for_each(it, running.end(), util::deleter<StorageService>());
		running.erase(it, running.end());
	}

	//-----------------------------------------------------------------------

	void StorageServiceManager::ResetWakeupTimer(void)
	{
		wakeupTimer->expires_at(
			wakeupTimer->expires_at() + boost::posix_time::seconds(WAKEUP_PERIOD)
		);
		wakeupTimer->async_wait(
			boost::bind(
				&StorageServiceManager::onWakeup,
				this, boost::asio::placeholders::error
			)
		);
	}

	//-----------------------------------------------------------------------
}
