/**
 *	DebugInfo.h
 *
 *	Contains all object's info.
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	May 2010
 */

#ifndef DEBUG_INFORMATION_H
#define DEBUG_INFORMATION_H

#include <list>
#include <string>
#include "SlotDebugInfo.h"
#include "UndoRedoHelper.h"


namespace iviews {
	namespace dinfo {
		
		class DebugInfo {
		public:
			typedef SlotDebugInfo::SlotDebugInfoList	SlotDebugInfoList;

			DebugInfo (const DebugInfo & dInfo);
			DebugInfo (const std::string & absolutRef_);

			bool Undo (void);
			bool Redo (void);

			void AddSlot (const SlotDebugInfo & di);
			void AddSlot (
					const id_t				id,
					const bool				isKeyAggregate,
					const std::string&		keyString,
					const std::string&		keyTypeInfoString,
					const graphs::Vertex*	keyTargetVertex,
					 const bool				isContentAggergate,
					const std::string&		contentString,
					const graphs::Vertex*	contentTargetVertex
				);


			SlotDebugInfoList		&	GetAllSlots (void);
			const SlotDebugInfoList &	GetAllSlots (void) const;
			const std::string		&	GetAbsolutRef (void) const;

			void				RemoveVertexFromSlotsList (graphs::Vertex * vertex);
			
		private:
			///////////////////////////////////////////////////
			///---- UndoRedo data

			struct UndoRedoData {

				UndoRedoData(const id_t & slotId_, const bool isKey_ , graphs::Vertex * vertex_)
					: slotId(slotId_), isKey(isKey_), vertex(vertex_) {}
				
				UndoRedoData(const UndoRedoData & obj)
					: slotId(obj.slotId), vertex(obj.vertex), isKey(obj.isKey) {}
				
				bool operator== (const SlotDebugInfo & slot)
					{ return slotId == slot.Id(); }	
				
				bool isKey;
				id_t slotId;
				graphs::Vertex * vertex;
			};

			typedef std::list<UndoRedoData> UndoRedoDataList;

			///////////////////////////////////////////////////
			///---- UndoRedoHelperForDebugInfo

			class UndoRedoHelperForDebugInfo : public UndoRedoHelper<UndoRedoDataList> {
			public:
				UndoRedoHelperForDebugInfo (void);
				UndoRedoHelperForDebugInfo (const UndoRedoHelperForDebugInfo & o);
				
				void AddInUndoStack(
						id_t			 slotId,
						const bool		 isKey,
						graphs::Vertex * vertex
					);
			};

			///////////////////////////////////////////////////
			//	private members

			id_t						nextStringfieldId;
			std::string					absolutRef;
			SlotDebugInfoList			slots;
			UndoRedoHelperForDebugInfo	undoRedo;
		};
	}	//namespace dinfo
}	//namespace iviews
#endif	//DEBUG_INFORMATION_H