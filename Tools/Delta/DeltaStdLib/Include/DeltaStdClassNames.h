// DeltaStdClassNames.h
// Class names for extern id objects of the std library,
// collected alltogether.
// ScriptFighter Project.
// A. Savidis, September 2008.
//

#ifndef	DELTASTDCLASSNAMES_H
#define	DELTASTDCLASSNAMES_H

#define	DELTA_STDLIB_NAMESPACE					"std"
#define	DELTA_STDLIB_NAMESPACE_PREFIX			"std::"
#define	DELTA_STDLIB_LIBS_NAMESPACE				"libs"
#define	DELTA_STDLIB_LIBS_NAMESPACE_PREFIX		"libs::"
#define	DELTA_STDLIB_NAMESPACE_ENTER			"@enter", "std",
#define	DELTA_STDLIB_NAMESPACE_EXIT				"@exit",
#define	DELTA_STDLIB_NAMESPACE_TYPE(t)			"@type", #t,
#define	DELTA_STDLIB_NAMESPACE_CONST(id, val)	"@const", #id " " "\"" val "\"",

#define	LIST_TYPE_STR							DELTA_STDLIB_NAMESPACE_PREFIX "list"
#define	VECTOR_TYPE_STR							DELTA_STDLIB_NAMESPACE_PREFIX "vector"
#define	LISTITER_TYPE_STR						DELTA_STDLIB_NAMESPACE_PREFIX "listiter"
#define	VECTORITER_TYPE_STR						DELTA_STDLIB_NAMESPACE_PREFIX "vectoriter"
#define	TABLEITER_TYPE_STR						DELTA_STDLIB_NAMESPACE_PREFIX "tableiter"
#define	FILE_TYPE_STR							DELTA_STDLIB_NAMESPACE_PREFIX "file"
#define	READER_TYPE_STR							DELTA_STDLIB_NAMESPACE_PREFIX "reader"
#define	WRITER_TYPE_STR							DELTA_STDLIB_NAMESPACE_PREFIX "writer"
#define	OUTPUTBUFFER_TYPE_STR					DELTA_STDLIB_NAMESPACE_PREFIX "outputbuffer"
#define	INPUTBUFFER_TYPE_STR					DELTA_STDLIB_NAMESPACE_PREFIX "inputbuffer"
#define	SOCKET_TYPE_STR							DELTA_STDLIB_NAMESPACE_PREFIX "socket"
#define DLL_TYPE_STR							DELTA_STDLIB_NAMESPACE_PREFIX "dll"

#define	DELTA_STD_ALGORITHMS_SUPERCLASS			"superclass"
#define	DELTA_STD_ALGORITHMS_ITERATOR			"iterator"
#define	DELTA_STD_ALGORITHMS_CONTAINER			"container"

#define	DELTA_SOCKET_PACKET_ORIGINAL			"packet_original"
#define	DELTA_SOCKET_PACKET_ADD_SIZE			"packet_add_size"

#endif	// Do not add stuff beyond this point.