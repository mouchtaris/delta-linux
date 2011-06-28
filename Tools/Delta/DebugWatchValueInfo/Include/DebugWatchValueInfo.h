// DebugWatchValueInfo.h
// Class to hold data regarding watch values needed by the debugger UI.
// ScriptFighter Project.
// Debug expession evaluator.
// A. Savidis, August 2008.
//

#ifndef	DEBUGWATCHVALUEINFO_H
#define	DEBUGWATCHVALUEINFO_H

#include "utypes.h"
#include "DDebug.h"

#include <string>
#include <list>

//////////////////////////////////////////////////////////////////

class DebugWatchValueInfo {

	//////////////////////////////////////////////////////////////////
	public:
	class Composite {
		
		//////////////////////////////////////////////////////////////////
		public:
		class Content {

			//////////////////////////////////////////////////////////////////
			public:
			class FieldKey {

				private:
				std::string	displayedKey;
				std::string	contentReference;	// Empty is none.
				bool		isSet;

				//*****************************

				public:
				const std::string	GetDisplayedText (void) const		{  DASSERT(isSet); return displayedKey;					}
				const std::string	GetContentReference (void) const	{  DASSERT(isSet); return contentReference;				}
				bool				HasContentReference (void) const	{  DASSERT(isSet); return !contentReference.empty();	}

				void	operator=(const FieldKey& fk)
							{ new (this) FieldKey(fk); }

				void	Set (const std::string& dk, const std::string& cr)
							{ new (this) FieldKey(dk, cr); }

				FieldKey (void) : isSet(false){}

				FieldKey (const std::string& dk, const std::string& cr) :
					displayedKey(dk),
					contentReference(cr),
					isSet(true){}

				FieldKey (const FieldKey& fk) :
					displayedKey(fk.displayedKey),
					contentReference(fk.contentReference),
					isSet(fk.isSet){}
				~FieldKey(){}
			};

			//////////////////////////////////////////////////////////////////

			private:
			std::string				subIndex;
			std::string				displayDesc;	// Empty is none.
			bool					keyVisible;
			std::string				keyType;		// Empty is none.
			std::list<FieldKey>		fieldKeys;
			bool					isSet;

			//*****************************

			public:
			const std::string			GetDisplayDesc (void) const	{ DASSERT(isSet); return displayDesc;			}
			bool						HasDisplayDesc (void) const	{ DASSERT(isSet); return !displayDesc.empty();	}
			bool						IsKeyVisible (void) const	{ DASSERT(isSet); return keyVisible;			}
			const std::string			GetKeyType (void) const		{ DASSERT(isSet); return keyType;				}
			bool						HasKeyType (void) const		{ DASSERT(isSet); return !keyType.empty();		}

			const std::list<FieldKey>&	GetFieldKeys (void) const	{ DASSERT(isSet); return fieldKeys;				}
			const std::string			GetSubIndex (void) const	{ DASSERT(isSet); return subIndex;				}

			void	operator=(const Content& c) 
						{ new (this) Content(c); }
			void	Set (const std::string& si, const std::string& dd, bool kv, const FieldKey& key, const std::string& kt = "")
						{ new (this) Content(si, dd, kv, key, kt); }
			void	Set (const std::string& si, const std::string& dd, bool kv, const std::list<FieldKey>& keys, const std::string& kt = "")
						{ new (this) Content(si, dd, kv, keys, kt); }

			Content (void): isSet(false){}

			Content (const std::string& si, const std::string& dd, bool kv, const FieldKey& key, const std::string& kt = "") :
				subIndex(si),
				displayDesc(dd),
				keyVisible(kv),
				keyType(kt),
				isSet(true){ fieldKeys.push_back(key); }

			Content (const std::string& si, const std::string& dd, bool kv, const std::list<FieldKey>& keys, const std::string& kt = "") :
				subIndex(si),
				displayDesc(dd),
				keyVisible(kv),
				keyType(kt),
				fieldKeys(keys),
				isSet(true){}

			Content (const Content& c) :
				subIndex(c.subIndex),
				displayDesc(c.displayDesc),
				keyVisible(c.keyVisible),
				keyType(c.keyType),
				fieldKeys(c.fieldKeys),
				isSet(c.isSet){}
			~Content(){}
		};
		//////////////////////////////////////////////////////////////////

		private:
		std::string			overview;
		std::string			absref;
		std::list<Content>	contents;
		bool				isSet;

		//*****************************

		public:
		const std::string&			GetOverview (void) const			{ DASSERT(isSet); return overview; }
		const std::string&			GetAbsoluteReference (void) const	{ DASSERT(isSet); return absref; }
		const std::list<Content>&	GetContents (void) const			{ DASSERT(isSet); return contents; }

		void	operator=(const Composite& c)
					{ new (this) Composite(c); }
		void	Set (const std::string& ov, const std::string& ar, const Content& c)
					{ new (this) Composite(ov, ar, c); }
		void	Set (const std::string& ov, const std::string& ar, const std::list<Content>& cs)
					{ new (this) Composite(ov, ar, cs); }

		Composite (void): isSet(false) {}

		Composite (const std::string& ov, const std::string& ar, const Content& c) :
			overview(ov),
			absref(ar),
			isSet(true){ contents.push_back(c); }

		Composite (const std::string& ov,  const std::string& ar, const std::list<Content>& cs) :
			overview(ov),
			absref(ar),
			contents(cs),
			isSet(true){}

		Composite (const Composite& c) :
			overview(c.overview),
			absref(c.absref),
			contents(c.contents),
			isSet(c.isSet){}
		~Composite(){}
	};
	//////////////////////////////////////////////////////////////////

	typedef Composite::Content	Content;
	typedef Content::FieldKey	FieldKey;

	enum Type { SimpleType = 0, CompositeType = 1, Undef = 2 };
	typedef std::string Simple;

	private:
	Type		type;
	Simple		simple;
	Composite	composite;

	public:
	Type						GetType (void) const		
									{ return type; }
	const Simple&				GetSimple (void) const		
									{ DASSERT(type == SimpleType);  return simple; }
	const Composite&			GetComposite (void) const
									{ DASSERT(type == CompositeType); return composite; }

	void						Set(const Simple& _simple)	
									{ type = SimpleType; simple = _simple; }
	void						Set (const Composite& _composite) 
									{ type = CompositeType; composite = _composite; }

	//*****************************
	// Token helpers for value construction (documenting better the code).

	static bool					T_VISIBLE (void)					{ return true; }
	static bool					T_HIDDEN (void)						{ return false; }
	static const std::string	T_NODESC (void)						{ return ""; }
	static const std::string	T_DESC (const std::string& desc)	{ return desc; }
	static const std::string	T_NOKEYREF (void)					{ return ""; }
	static const std::string	T_KEYREF (const std::string& ref)	{ return ref; }

	DebugWatchValueInfo (void) : type(Undef){}
	~DebugWatchValueInfo(){}
};

#define WATCHVALUE_VISIBLE	DebugWatchValueInfo::T_VISIBLE
#define WATCHVALUE_HIDDEN	DebugWatchValueInfo::T_HIDDEN
#define WATCHVALUE_NODESC	DebugWatchValueInfo::T_NODESC
#define WATCHVALUE_DESC		DebugWatchValueInfo::T_DESC
#define WATCHVALUE_NOKEYREF DebugWatchValueInfo::T_NOKEYREF
#define WATCHVALUE_KEYREF	DebugWatchValueInfo::T_KEYREF

//////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
