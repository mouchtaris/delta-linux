#ifndef VERTEX_TO_DEBUG_INFO_MAPPING_H
#define VERTEX_TO_DEBUG_INFO_MAPPING_H


#include <map>
#include "Vertex.h"
#include "DebugInfo.h"
#include "Functional.h"
#include "UndoRedoHelper.h"

namespace iviews {

	namespace graphs {
		
		class VertexToDebugInfoMapping : public std::map <	
													Vertex *, 
													dinfo::DebugInfo, 
													utility::ComparatorFunctor<Vertex>
												>
		{
		public:
			bool Undo (void);
			bool Redo (void);

			void NewUndoAction (void);
			void RemoveVertexFromSlots (const Vertex & v);

			virtual void erase (iterator position);
		private:
			typedef dinfo::DebugInfo				DebugInfo;
			typedef std::pair<Vertex *, DebugInfo>	VertexToDebugInfo;
			typedef std::list<VertexToDebugInfo>	VertexToDebugInfoList;


			class UndoRedoHelperForVertexToDInfo : public UndoRedoHelper<VertexToDebugInfoList> {
			public:
				UndoRedoHelperForVertexToDInfo ();
				void AddInUndoStack (Vertex *, DebugInfo &);				
			};

			UndoRedoHelperForVertexToDInfo undoRedo;
		};
	}	//graphs
}// iviews


#endif	//VERTEX_TO_DEBUG_INFO_MAPPING_H