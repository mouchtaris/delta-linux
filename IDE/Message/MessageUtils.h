/**
 *	MessageUtils.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_MESSAGEUTILS_H
#define IDE_MESSAGEUTILS_H

#include "Common.h"
#include "IDECommon.h"
#include "Message.h"

namespace ide {

////////////////////////////////////////////////////////////////////////

class Component;

////////////////////////////////////////////////////////////////////////

void _IDE_API MessageSetDefaultEndpoints (Message& msg, const Message& received, Component* comp);
void _IDE_API MessageSetDefaultEndpoints (Message& msg, const Message& received);

////////////////////////////////////////////////////////////////////////

template<class TResultContainerType, class TMessageContainerType>
void MessageExtractSingleValue (TResultContainerType& retVal, const TMessageContainerType& msgs)
{
	typename TResultContainerType::value_type currRetVal;
	typename TMessageContainerType::const_iterator iter = msgs.begin();
	for (; iter != msgs.end(); ++iter) {
		comm::decoder(iter->GetData()) >> currRetVal;
		retVal.push_back(currRetVal);
	}
}

template<class TResultContainerType, class TMessageContainerType>
inline TResultContainerType MessageExtractSingleValue (const TMessageContainerType& msgs)
{
	TResultContainerType retVal;
	MessageExtractSingleValue(retVal, msgs);
	return retVal;
}

//**********************************************************************

template<class T>
void MessageExtractSingleValue (T& retVal, const Message& result)
{
	comm::decoder(result.GetData()) >> retVal;
}

template<class T>
inline T MessageExtractSingleValue (const Message& result)
{
	T retVal;
	MessageExtractSingleValue(retVal, result);
	return retVal;
}

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_MESSAGEUTILS_H
