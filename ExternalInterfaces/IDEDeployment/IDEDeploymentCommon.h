/**
 *	IDEDeploymentCommon.h
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	October 2010
 */
#ifndef IDE_DEPLOYMENT_COMMON_H_
#define IDE_DEPLOYMENT_COMMON_H_

#ifdef IDEDEPLOYMENT_EXPORTS
#	define _DEPLOY_API		__declspec(dllexport)
#else
#	define _DEPLOY_API		__declspec(dllimport)
#endif

#endif	//IDE_DEPLOYMENT_COMMON_H_
