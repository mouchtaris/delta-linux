#ifndef	RENDERING_INFO_ALGORITHMS_MACROS_H
#define	RENDERING_INFO_ALGORITHMS_MACROS_H

#include "SlotRenderingInfo.h"
#include "EdgeRenderingInfo.h"
#include "LayerRenderingInfo.h"
#include "VertexRenderingInfo.h"



namespace iviews {

//-----------------------------------------------------------------------

#define LAYER_VEC LayerRenderingInfo::LayerRenderingInfoPtrVec

#define FOR_ALL_LAYERS(VAR, LAYERS_VEC)													\
	if (LAYER_VEC * _layers_vec_ = &(LAYERS_VEC))										\
		for (LAYER_VEC::iterator VAR = _layers_vec_->begin();							\
			 VAR != _layers_vec_->end();												\
			++VAR)	

#define FOR_ALL_LAYERS_CONST(VAR, LAYERS_VEC)											\
	if (LAYER_VEC * _layers_vec_ = const_cast<LAYER_VEC * >(&(LAYERS_VEC)))				\
		for (LAYER_VEC::const_iterator VAR = _layers_vec_->begin();						\
			 VAR != _layers_vec_->end();												\
			++VAR)	

//-----------------------------------------------------------------------

#define VERTEX_LIST VertexRenderingInfo::VertexRenderingInfoPtrList

#define FOR_ALL_VERTICES(VAR, VERTICES_LIST)											\
	if (VERTEX_LIST * _vertices_list_ = &(VERTICES_LIST))								\
		for(VERTEX_LIST::iterator VAR = _vertices_list_->begin();						\
			VAR != _vertices_list_->end();												\
			++VAR)

#define FOR_ALL_VERTICES_CONST(VAR, VERTICES_LIST)										\
	if (VERTEX_LIST * _vertices_list_ = const_cast<VERTEX_LIST * >(&(VERTICES_LIST)))	\
		for(VERTEX_LIST::const_iterator VAR = _vertices_list_->begin();					\
			VAR != _vertices_list_->end();												\
			++VAR)


//-----------------------------------------------------------------------

#define EDGE_LIST EdgeRenderingInfo::EdgeRenderingInfoPtrList

#define FOR_ALL_EDGES(VAR, EDGES_LIST)													\
	if (EDGE_LIST * _edge_list_ = &(EDGES_LIST))										\
		for (EDGE_LIST::iterator VAR = _edge_list_->begin();							\
			 VAR != _edge_list_->end();													\
			 ++VAR) 

#define FOR_ALL_EDGES_CONST(VAR, EDGES_LIST)											\
	if (EDGE_LIST * _edge_list_ = const_cast<EDGE_LIST * >(&(EDGES_LIST)))				\
		for (EDGE_LIST::const_iterator VAR = _edge_list_->begin();						\
			 VAR != _edge_list_->end();													\
			 ++VAR) 



//-----------------------------------------------------------------------

#define SLOT_LIST SlotRenderingInfo::SlotRenderingInfoPtrList

#define FOR_ALL_SLOTS(VAR, SLOTS_LIST)													\
	if (SLOT_LIST * _slot_list_ = &(SLOTS_LIST))										\
		for (SLOT_LIST::iterator VAR = _slot_list_->begin();							\
			 VAR != _slot_list_->end();													\
			 ++VAR) 

#define FOR_ALL_SLOTS_CONST(VAR, SLOTS_LIST)											\
	if (SLOT_LIST * _slot_list_ = const_cast<SLOT_LIST * >(&(SLOTS_LIST)))				\
		for (SLOT_LIST::const_iterator VAR = _slot_list_->begin();						\
			 VAR != _slot_list_->end();													\
			 ++VAR) 

}	//namespace iviews 


#endif	//RENDERING_INFO_ALGORITHMS_MACROS_H