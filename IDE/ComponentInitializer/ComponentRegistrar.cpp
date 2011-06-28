/**
 *	ComponentRegistryUtils.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "ComponentRegistrar.h"
#include "ComponentRegistry.h"
#include "ComponentSignalRegistry.h"
#include "BitmapRegistry.h"

#include "Algorithms.h"

#include <boost/foreach.hpp>

namespace ide {

////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_AUTOINITIALIZER(className)							\
	void className::DelayedInit (const TupleType& tuple)				\
	{																	\
		if (s_list == 0)												\
			s_list = new ListType;										\
		s_list->push_back(className(tuple));							\
	}																	\
	void className::Commit (void)										\
	{																	\
		if (s_list)  {													\
			util::for_all(*s_list, std::mem_fun_ref(&className::init));	\
			Clear();													\
		}																\
	}																	\
	void className::Commit (const std::string& compId)					\
	{																	\
		if (s_list)  {													\
			ListType::iterator iter = s_list->begin();					\
			while (iter != s_list->end()) {								\
				if (iter->m_tuple.get<0>() != compId)					\
					++iter;												\
				else {													\
					iter->init();										\
					iter = s_list->erase(iter);							\
				}														\
			}															\
			if (s_list->empty())										\
				Clear();												\
		}																\
	}																	\
	void className::Clear (void)										\
	{																	\
		delete s_list;													\
		s_list = 0;														\
	}																	\
	className::ListType* className::s_list = 0

////////////////////////////////////////////////////////////////////////

IMPLEMENT_AUTOINITIALIZER(ComponentEntryCreator);

void ComponentEntryCreator::init (void)
{
	const ComponentEntry entry(
		m_tuple.get<0>(), m_tuple.get<1>(), m_tuple.get<2>(),
		m_tuple.get<3>(), m_tuple.get<4>()
	);
	ComponentRegistry::Instance().Register(entry);
}

//**********************************************************************
// Additional ComponentEntryCreator functions
//
bool ComponentEntryCreator::HasPendingComponents (void)
	{ return s_list && !s_list->empty(); }

void ComponentEntryCreator::GetPendingComponents (StdStringList& result)
{
	BOOST_FOREACH(ComponentEntryCreator& inst, *s_list)
		result.push_back(inst.m_tuple.get<0>());
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_AUTOINITIALIZER(ComponentMetadataSetter);

void ComponentMetadataSetter::init (void)
{
	ComponentRegistry::Instance().GetComponentEntry(m_tuple.get<0>()).
		SetMetadata(m_tuple.get<1>());
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_AUTOINITIALIZER(ComponentPostInitializer);

void ComponentPostInitializer::init (void)
{
	ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(m_tuple.get<0>());
	entry.Init();
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_AUTOINITIALIZER(ComponentEntryFunctionRegistrar);

void ComponentEntryFunctionRegistrar::init (void)
{
	const ComponentFuncEntry func(
		m_tuple.get<2>(), m_tuple.get<3>(), m_tuple.get<4>(), m_tuple.get<1>(),
		m_tuple.get<5>(), m_tuple.get<6>(), false
	);
	ComponentRegistry::Instance().GetComponentEntry(m_tuple.get<0>()).AddFunction(func);
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_AUTOINITIALIZER(ComponentEntrySignalRegistrar);

void ComponentEntrySignalRegistrar::init (void)
{
	ComponentSignalRegistry::Instance().AddSignal(m_tuple.get<1>(), m_tuple.get<2>(), m_tuple.get<3>());
	ComponentRegistry::Instance().GetComponentEntry(m_tuple.get<0>()).AddSignal(m_tuple.get<1>());
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_AUTOINITIALIZER(ComponentEntrySlotRegistrar);

void ComponentEntrySlotRegistrar::init (void)
{
	const ComponentFuncEntry func(
		m_tuple.get<3>(), m_tuple.get<4>(), m_tuple.get<5>(), m_tuple.get<1>(),
		m_tuple.get<6>(), m_tuple.get<7>(), true
	);
	ComponentEntry& entry = ComponentRegistry::Instance().GetComponentEntry(m_tuple.get<0>()).
		AddFunction(func);

	const ComponentSlot slot(m_tuple.get<0>(), m_tuple.get<1>());
	ComponentSignalRegistry::Instance().ConnectSlot(m_tuple.get<2>(), slot, !m_tuple.get<3>());
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_AUTOINITIALIZER(ComponentEntryUserCmdFunctionRegistrar);

void ComponentEntryUserCmdFunctionRegistrar::init (void)
{
	const ComponentFuncEntry func(
			m_tuple.get<2>(), m_tuple.get<3>(), m_tuple.get<4>(), m_tuple.get<1>(),
			m_tuple.get<5>(), m_tuple.get<7>(), false
	);
	ComponentRegistry::Instance().GetComponentEntry(m_tuple.get<0>()).AddFunction(func).
		AddUserCommand(
			m_tuple.get<8>().get<0>(),
			UserCommand(
				UserCommand::Callback("User", m_tuple.get<0>(), m_tuple.get<1>()),
				m_tuple.get<8>().get<1>() ? UserCommand::UCT_CHECK_ITEM : UserCommand::UCT_NORMAL,
				m_tuple.get<8>().get<2>(),
				m_tuple.get<6>(),
				true,	//automated entries are by default enabled
				m_tuple.get<8>().get<3>()
			)
		);
}

////////////////////////////////////////////////////////////////////////

IMPLEMENT_AUTOINITIALIZER(ComponentImageRegistrar);

void ComponentImageRegistrar::init (void)
{
	BitmapRegistry::Instance().Insert(m_tuple.get<1>(), m_tuple.get<2>());
}

////////////////////////////////////////////////////////////////////////

} // namespace ide
