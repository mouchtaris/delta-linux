/**
 *	SlotDebugInfo.h
 *
 *	Contains object's info.
 *	Specially info about the key and the corresponding content
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	May 2010
 */

#ifndef DEBUG_INFORMATION_CONTENTS_H
#define DEBUG_INFORMATION_CONTENTS_H


#include <string>
#include <utility>
#include <functional>
#include "Vertex.h"
#include "CommonTypes.h"


namespace iviews {
	namespace dinfo {

		enum KeyTypeInformation {
			OBJECT_DELEGATION_PSEUDO_KEY	= 0,	//delegation pseudo keys
			OBJECT_INHERITANCE_PSEUDO_KEY	= 1,	//inheritance pseudo keys
			OBJECT_OPERATOR_KEY				= 2,	//operator keys	
			OBJECT_ATTRIBUTE_KEY			= 3,	//attribute modified keys
			EXTERNID_USER_PSEUDO_KEY		= 4,	//externid user-keys
			OBJECT_PSEUDO_KEY				= 5,	//rest internal pseudo keys
			AGGREGATE_PSEUDO_KEY			= 6,	//rest internal pseudo keys
			EXTERNID_PSEUDO_KEY				= 7,	//rest internal pseudo keys
			NONE_PSEUDO_KEY					= 8		//without type key
		};

		class SlotDebugInfo {

		public:
			typedef std::list<SlotDebugInfo> SlotDebugInfoList;

			///------- functors
			
			struct SlotDebugInfoComparatorFunctor : public std::binary_function<
																const SlotDebugInfo *, 
																const SlotDebugInfo *, 
																bool
															>
			{
				bool operator() (const SlotDebugInfo * slot1, const SlotDebugInfo * slot2) const
					{	return slot1->Id() < slot2->Id();	}
			};
			

			///-------- Constructor(s)

			SlotDebugInfo ();
			SlotDebugInfo (
				const id_t				_id,
				const std::string &		_stringId,
				const bool				_isKeyAggregate,
				const std::string &		_keyString,
				const std::string &		_keyTypeInfoString,
				const graphs::Vertex *	_keyTargetVertex,
				const bool				_isContentAggergate,
				const std::string &		_contentString,
				const graphs::Vertex *	_contentTargetVertex
			);

			SlotDebugInfo (const SlotDebugInfo & sdi);
			

			///---------- Method(s)
			id_t				Id () const;
			void				SetId (const id_t & _id);
			
			const std::string &	GetIdRelevantToDebugInfo (void) const;
			void				SetIdRelevantToDebugInfo (const std::string & id);

			//****************************
			//key 
			bool				IsKeyAggregate (void) const;
			void				KeyAggregate (const bool b);
			
			//--------------------------------------------------------
			std::string			GetKeyString (void) const;
			void				SetKeyString (const std::string & keyStr);

			//--------------------------------------------------------
			KeyTypeInformation	GetKeyTypeInfo (void) const;
			void				SetKeyTypeInfo (const KeyTypeInformation keyTypeInfo);

			//--------------------------------------------------------

			std::string			GetStringKeyTypeInfo (void) const;
			void				SetStringKeyTypeInfo (const std::string & keyTypeInfo);

			//--------------------------------------------------------
			graphs::Vertex *	GetKeyTargetVertex (void) const;
			void				SetKeyTargetVertex (graphs::Vertex* v);


			//****************************
			//key 
			bool				IsContentAggregate (void) const;
			void				ContentAggregate (const bool b);

			//--------------------------------------------------------
			std::string			GetContentString (void) const;
			void				SetContentString (const std::string & conStr);

			//--------------------------------------------------------
			graphs::Vertex *	GetContentTargetVertex (void) const;
			void				SetContentTargetVertex (graphs::Vertex * v);

		private:
			//////////////////////////////////
			//	private class's members
			std::string			KeyTypeInformationToString (const KeyTypeInformation & info);
			KeyTypeInformation	StringToKeyTypeInformation (const std::string & keyTypeInfo);


			id_t				id;
			std::string			stringId;
			//key 
			bool				isKeyAggregate;		//is composite
			std::string			keyString;
			KeyTypeInformation	keyTypeInfo;
			std::string			keyTypeInfoString;
			graphs::Vertex *	keyTargetVertex;
			
			//content
			bool				isContentAggergate;	//is composite
			std::string			contentString;
			graphs::Vertex *	contentTargetVertex;
		};
	}	//namespace dinfo

}	//namespace iviews

#endif	//DEBUG_INFORMATION_CONTENTS_H