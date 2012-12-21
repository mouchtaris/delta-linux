// TreeAttribute.h
// Attribute support for TreeNodes.
// ScriptFighter Project.
// Y.Lilis, August 2011.
//

#ifndef TREEATTRIBUTE_H
#define TREEATTRIBUTE_H

#include "DeltaSyntaxTreeDefs.h"
#include "DDebug.h"
#include <list>
#include <string>
#include <new>
#include <assert.h>

/////////////////////////////////////////////////////////

enum TreeAttributeType {
	AttributeTypeInt		= 0,
	AttributeTypeUInt		= 1,
	AttributeTypeDouble		= 2,
	AttributeTypeBool		= 3,
	AttributeTypeString		= 4,
	AttributeTypeStringList	= 5,
	AttributeTypePointer	= 6
};

/////////////////////////////////////////////////////////

class SYNTAXTREELIB_CLASS TreeAttribute {
private:
	typedef std::list<std::string> StringList;

	typedef void* (*CopierFunc)(void*);
	typedef void  (*CleanerFunc)(void*);
	struct PointerValue {
		void*		data;
		CopierFunc	copier;
		CleanerFunc cleaner;
		PointerValue (void* data, CopierFunc copier = (CopierFunc) 0, CleanerFunc cleaner = (CleanerFunc) 0) :
			data(data), copier(copier), cleaner(cleaner) {}
	};

	union Content {
		util_i32		intVal;
		util_ui32		uintVal;
		double			doubleVal;
		bool			boolVal;
		std::string*	stringVal;
		StringList*		stringListVal;
		PointerValue*	pointerVal;
	};

	TreeAttributeType	type;
	Content				content;

	static StringList* CopyStringList(const StringList& list)
		{ return DNEWCLASS(StringList, (list.begin(), list.end())); }
public:
	
	void Clear (void);

	TreeAttributeType GetType (void) const { return type; }

	//*********************

	util_i32	GetInt (void) const;
	void		SetInt (util_i32 val);

	//*********************

	util_ui32	GetUInt (void) const;
	void		SetUInt (util_ui32 val);

	//*********************

	double	GetDouble (void) const;
	void	SetDouble (double val);

	//*********************

	bool GetBool (void) const;
	void SetBool (bool val);

	//*********************

	const std::string&	GetString (void) const;
	void				SetString (const std::string& str);
	
	//*********************
			
	StringList&			GetStringList (void);
	const StringList&	GetStringList (void) const;
	void				SetStringList (const StringList& list);

	//*********************

	void*		GetPointer (void);
	const void*	GetPointer (void) const;
	void		SetPointer (void* val, CopierFunc copier = (CopierFunc) 0, CleanerFunc cleaner = (CleanerFunc) 0);

	//*********************

	bool IsInt			(void) const { return type == AttributeTypeInt;			}
	bool IsUInt			(void) const { return type == AttributeTypeUInt;		}
	bool IsDouble		(void) const { return type == AttributeTypeDouble;		}
	bool IsBool			(void) const { return type == AttributeTypeBool;		}
	bool IsString		(void) const { return type == AttributeTypeString;		}
	bool IsStringList	(void) const { return type == AttributeTypeStringList;	}
	bool IsPointer		(void) const { return type == AttributeTypePointer;		}

	//*********************

	void operator =	(const TreeAttribute& attr) { Clear(); new (this) TreeAttribute(attr);	}

	bool operator ==(const TreeAttribute& attr);
	bool operator !=(const TreeAttribute& attr) { return !operator==(attr);	}

	//*********************

	const std::string toString(void) const;
	
	//*********************

	TreeAttribute (util_i32 val);
	TreeAttribute (util_ui32 val);
	TreeAttribute (double val);
	TreeAttribute (bool val);
	TreeAttribute (const char *val);
	TreeAttribute (const std::string& val);
	TreeAttribute (StringList* val);	//Takes ownership
	TreeAttribute (const StringList& val);
	TreeAttribute (void* val, CopierFunc copier = (CopierFunc) 0, CleanerFunc cleaner = (CleanerFunc) 0);
	TreeAttribute (const TreeAttribute& expr);	//Deep copy

	~TreeAttribute() { Clear(); }
};

/////////////////////////////////////////////////////////

#endif