// DeltaTableP.h
// Some privates definitions for DeltaTable.
// Delta Virtual Machine.
// ScriptFighter Project.
// A. Savidis, October 1999 (original version).
// Refectored November 2009, moved here.
//

#ifndef	DELTATABLEP_H
#define	DELTATABLEP_H

//-------------------------------------------------------------

#define	DELTA_TABLE_COMMON_HASH_SIZE		23
#define	DELTA_TABLE_NUMBER_HASH_SIZE		211
#define	DELTA_TABLE_STRING_HASH_SIZE		211
#define	DELTA_TABLE_TOTAL_HASHTABLES		8	
#define	TOTAL_GET_BUCKET_FUNCS				10

#define	GETBUCKET_ARGS				\
		DeltaTable*			p,		\
		const DeltaValue&	index,	\
		Element***	table

//-------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
