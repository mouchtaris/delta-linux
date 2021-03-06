// GenericReader.h
// GameMaker Project.
// A. Savidis, March 2007.
// September 2011, added generic reader decoder
// factory pattern via macros.
//

#ifndef	GENERICREADER_H
#define	GENERICREADER_H

#include <stdio.h>
#include "utypes.h"
#include "ustrings.h"
#include "uerrorclass.h"
#include "uptr.h"
#include "ubinaryio.h"
#include "ufiles.h"
#include "FileReadersDefs.h"
#include "GenericWriter.h"
#include <string>
#include <map>

///////////////////////////////////////////////////////////

#define	GENERIC_READER_COMMON_LOAD_STORE_METHODS_PUBLIC_DEF								\
	bool	LoadBin (const std::string& path);											\
	void	StoreBin (const std::string& path) const;									\
	void	StoreText (const std::string& path) const;

#define	GENERIC_READER_COMMON_LOAD_METHOD_PUBLIC_DEF									\
	bool	LoadBin (const std::string& path);

#define	GENERIC_READER_COMMON_STORE_METHODS_PUBLIC_DEF									\
	void	StoreBin (const std::string& path) const;									\
	void	StoreText (const std::string& path) const;

//**************************

#define	GENERIC_READER_COMMON_STORE_METHODS_IMPL_TEMPLATE(_prefix)						\
	void _prefix StoreText (const std::string& path) const {							\
		if (FILE* fp = fopen(path.c_str(), "wt")) {										\
			WriteText(fp);																\
			fclose(fp);																	\
		}																				\
	}																					\
	void _prefix StoreBin (const std::string& path) const {								\
		if (FILE* fp = ubinaryfileopen(path, "w")) {									\
			Write(utempobj(PortableBinFileWriter(fp)));									\
			fclose(fp);																	\
		}																				\
	}																					\

#define	GENERIC_READER_COMMON_STORE_METHODS_IMPL(_class)								\
	GENERIC_READER_COMMON_STORE_METHODS_IMPL_TEMPLATE(_class::)

// Requires the ERROR_HANDLER macro for posting primary / domino errors.
#define	GENERIC_READER_COMMON_LOAD_METHOD_IMPL(_class)									\
	bool _class::LoadBin (const std::string& path) {									\
		FILE* fp = ubinaryfileopen(path, "r");											\
		UCHECK_PRIMARY_ERROR(fp, uconstructstr("(failed to open '%s')", path.c_str()));	\
		UCHECK_DOMINO_ERROR(															\
			Read(utempobj(PortableBinFileReader(fp))),									\
			uconstructstr("(from '%s')", path.c_str())									\
		);																				\
		fclose(fp);																		\
		return true;																	\
		FAIL: if (fp) fclose(fp); return false;											\
	}

#define	GENERIC_READER_COMMON_STORE_METHODS_INLINE_IMPL									\
	GENERIC_READER_COMMON_STORE_METHODS_IMPL_TEMPLATE(UEMPTY)

#define	GENERIC_READER_COMMON_LOAD_METHOD_INLINE_IMPL(_format)							\
	bool LoadBin (const std::string& path) {											\
		UCHECK_ERROR_FORMAT_DEFINE(_format);											\
		FILE* fp = ubinaryfileopen(path, "r");											\
		UCHECK_PRIMARY_ERROR_REPORT(													\
			fp,																			\
			uconstructstr("(failed to open '%s')", path.c_str())						\
		);																				\
		UCHECK_DOMINO_ERROR_REPORT(														\
			Read(utempobj(PortableBinFileReader(fp))),									\
			uconstructstr("(from '%s')", path.c_str())									\
		);																				\
		fclose(fp);																		\
		return true;																	\
		FAIL: if (fp) fclose(fp); return false;											\
	}

#define	GENERIC_READER_COMMON_LOAD_STORE_METHODS_IMPL(_class)							\
	GENERIC_READER_COMMON_STORE_METHODS_IMPL(_class)									\
	GENERIC_READER_COMMON_LOAD_METHOD_IMPL(_class)

///////////////////////////////////////////////////////////

class GenericReader {
	public:
	template <typename T>
	static T*	ConstructTemplate (GenericReader& reader, T* newInst) {
					if (DPTR(newInst)->Read(reader))
						return newInst;
					else {
						DDELETE(newInst); 
						return (T*) 0;
					}
				}

	virtual bool eof (void) = 0;
	virtual bool read (std::string& at, bool quoted) = 0;
	virtual bool read (util_ui32* b) = 0;
	virtual bool read (util_i32* b) = 0;
	virtual bool read (util_ui16* b) = 0;
	virtual bool read (util_i16* b) = 0;
	virtual bool read (util_ui8* b) = 0;
	virtual bool read (float* f) = 0;
	virtual bool read (double* f) = 0;
	virtual bool read (bool* b) = 0;
	virtual bool read (void* b, util_ui32 n) = 0;
	virtual const std::string context (void) const = 0;

	GenericReader (void){}
	virtual ~GenericReader(){}
};

///////////////////////////////////////////////////////////

template <typename T> class uloaders_registry {
	public:
	typedef T*			(*Loader)(GenericReader& reader);
	typedef T			LoaderReturnType;

	protected:
	typedef std::map<std::string, Loader> Loaders;
	Loaders				loaders;
	uloaders_registry (void)
		{}

	public:
	void				Initialise (void) 
							{ DASSERT(loaders.empty()); }
	void				CleanUp (void)
							{ loaders.clear(); }

	void				Install (Loader loader, const std::string& classId) {
							DASSERT(loaders.find(classId) == loaders.end());
							loaders[classId] = loader;
						}

	void				WriteClassId (GenericWriter& writer, const std::string& classId)
							{ writer.write(classId); }

	T*					Load (GenericReader& reader, const std::string& classId) const {
							typename Loaders::const_iterator i = loaders.find(classId);
							DASSERT(i != loaders.end());
							return (*i->second)(reader);
						}

	T*					Load (GenericReader& reader) const {
							
							std::string	classId("<unknown class>");
							typename Loaders::const_iterator i = loaders.end();
							UCHECK_ERROR_FORMAT_DEFINE(
								uconstructstr("Loading %s: error in reading '%%s'!", classId.c_str())
							);

							UCHECK_PRIMARY_ERROR_REPORT(reader.read(classId, false), "class id");
							i = loaders.find(classId);
							UCHECK_PRIMARY_ERROR_REPORT(
								i != loaders.end(), 
								uconstructstr("invalid class id '%s'", classId.c_str())
							);

							return (*i->second)(reader);
							FAIL: return (T*) 0;
					}
};

///////////////////////////////////////////////////////////

#define	ULOADERS_REGISTRY_SINGLETON_DEF(_class, _type)					\
class _class : public uloaders_registry<_type> {						\
	DFRIENDDESTRUCTOR()													\
	private:															\
	USINGLETON_APISTYLE_DECLARE_PRIVATEINSTANCE(_class)					\
	public:																\
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS						\
	USINGLETON_APISTYLE_DECLARE_GETTER(_class)							\
};																		\
USINGLETON_INLINE_ACCESS_HELPER(_class)

///////////////////////////////////////////////////////////

#define	ULOADERS_REGISTRY_SINGLETON_IMPL(_class)						\
	USINGLETON_APISTYLE_DEFINE_PRIVATEINSTANCE(_class)					\
	USINGLETON_APISTYLE_IMPL_PUBLICSTDMETHODS(_class)					\
	USINGLETON_APISTYLE_IMPL_GETTER(_class)

///////////////////////////////////////////////////////////
// Requires usage of UCLASSID_STD_ABSTRACT_METHOD in superclass

#define	ULOADERS_DERIVED_CLASS_PUBLIC_COMMON_NO_WRITE(_class,_loaders,_load,_classid)	\
	UCLASSID_STD_DERIVED_METHODS(_classid)												\
	static void					InstallLoader (void) {									\
									_loaders##Get().Install(							\
										&_class::_load,									\
										_classid										\
									);													\
								}														\
	static _loaders::LoaderReturnType*													\
								_load (GenericReader& reader);

#define	ULOADERS_DERIVED_CLASS_PUBLIC_COMMON(_class,_loaders,_load,_classid)			\
	ULOADERS_DERIVED_CLASS_PUBLIC_COMMON_NO_WRITE(_class,_loaders,_load,_classid)		\
	virtual void				Write (GenericWriter& writer) const;

///////////////////////////////////////////////////////////
// Common serialiable API with a registry of loader static constructors.
// Requires subclasses to implement: Read(), Write() and ToString().

#define ULOADERS_SERIALISABLE_SUPER_PUBLIC_DEFS(_super_class)						\
	UCLASSID_STD_ABSTRACT_METHOD													\
	virtual const std::string	ToString (void) const = 0;							\
	virtual void				Write (GenericWriter& writer) const {				\
									_super_class##LoadersGet().WriteClassId(		\
										writer,										\
										GetClassId()								\
									);												\
								}													\
	void						WriteText (FILE* fp) const							\
									{ fprintf(fp, "%s\n", ToString().c_str()); }	\
	static _super_class*		Load (GenericReader& reader);						\
	virtual _super_class*		Clone (void) const = 0;								\
	virtual ~_super_class(){}

#define	ULOADERS_SERIALISABLE_REGISTRY_DEFS(_super_class)							\
	class _super_class;																\
	ULOADERS_REGISTRY_SINGLETON_DEF(												\
		_super_class##Loaders,														\
		_super_class																\
	)

#define	ULOADERS_SERIALISABLE_REGISTRY_IMPL(_super_class)							\
	ULOADERS_REGISTRY_SINGLETON_IMPL(_super_class##Loaders)

#define	ULOADERS_SERIALISABLE_SUPER_LOAD_IMPL(_super_class)							\
	_super_class* _super_class::Load (GenericReader& reader)						\
		{ return _super_class##Loaders##Get().Load(reader); }

#define	ULOADERS_SERIALISABLE_DERIVED_PUBLIC_DEFS(_class,_classid,_loaders)			\
	typedef _loaders::LoaderReturnType SuperType;									\
	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(_class)									\
	UCLONE_VIRTUAL_VIA_COPY_CONSTRUCTOR(_class, Clone)								\
	ULOADERS_DERIVED_CLASS_PUBLIC_COMMON(											\
		_class, _loaders, Load, _classid											\
	)																				\
	virtual bool				Read (GenericReader& reader);						\
	virtual const std::string	ToString (void) const;

#define	ULOADERS_SERIALISABLE_DERIVED_LOAD_IMPL(_class)								\
	_class::SuperType* _class::Load (GenericReader& reader) {						\
		_class* inst = DNEW(_class);												\
		if (DPTR(inst)->Read(reader))												\
			return inst;															\
		else 																		\
			{ DDELETE(inst); return (_class*) 0; }									\
	}

///////////////////////////////////////////////////////////

class userialisable {
	public:
	UCLASSID_STD_ABSTRACT_METHOD
	virtual userialisable*			Clone (void) const = 0;
	virtual const std::string		ToString (void) const = 0;		
	virtual bool					Read (GenericReader& reader) = 0;
	virtual void					Write (GenericWriter& writer) const
										{ writer.write(GetClassId()); }
	void							WriteText (FILE* fp) const
										{ fprintf(fp, "%s\n", ToString().c_str()); }	
	virtual ~userialisable(){}
};

///////////////////////////////////////////////////////////

template <typename T> class userialisable_factory {

	public:
	typedef T*			(*FactoryFunc)(void);

	protected:
	typedef std::map<std::string, FactoryFunc> Factories;
	Factories			factories;

	public:
	void				Initialise (void)
							{ DASSERT(factories.empty()); }
	void				CleanUp (void)
							{ factories.clear(); }
	void				Install (
							const std::string&	classId, 
							FactoryFunc			f
						) {
							DASSERT(factories.find(classId) == factories.end());
							factories[classId] = f;
						}

	T*					New (GenericReader& reader) {

							std::string	classId("<unknown class>");
							UCHECK_ERROR_FORMAT_DEFINE(
								uconstructstr("Loading %s: error in reading '%%s'!", classId.c_str())
							);

							T* elem = (T*) 0;
							typename Factories::const_iterator i;

							UCHECK_PRIMARY_ERROR_REPORT(
								reader.read(classId, false), 
								"class id"
							);
							i = factories.find(classId);
							UCHECK_PRIMARY_ERROR_REPORT(
								i != factories.end(), 
								"unknown class id"
							);

							elem = (*i->second)();
							UCHECK_DOMINO_ERROR_REPORT(
								DPTR(elem)->Read(reader), 
								"serialized state"
							);

							return elem;
							FAIL: udeleteunlessnull(elem); return (T*) 0;
						}

	T*					New (const std::string& classId) const {
							typename Factories::const_iterator i = factories.find(classId);
							DASSERT(i != factories.end());
							return (*i->second)();
						}
	userialisable_factory (void){}
	virtual ~userialisable_factory(){}
};

///////////////////////////////////////////////////////////

#define	USERIALISABLE_FACTORY_SINGLETON_DEF(_class,_base)						\
class _class : public userialisable_factory<_base> {							\
	DFRIENDDESTRUCTOR()															\
	protected:																	\
	_class (void){}																\
	virtual ~_class(){}															\
	USINGLETON_APISTYLE_DECLARE_PRIVATEINSTANCE(_class)							\
	public:																		\
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS								\
	USINGLETON_APISTYLE_DECLARE_GETTER(_class)									\
};

// requires copy constructor
#define USERIALISABLE_DERIVED_PUBLIC_COMMON(_factory,_super,_class,_classid)	\
	protected:																	\
	static _super*	New (void)													\
						{ return DNEW(_class); }								\
	public:																		\
	UCLASSID_STD_DERIVED_METHODS(_classid)										\
	UCLONE_VIRTUAL_VIA_COPY_CONSTRUCTOR(_class, Clone)							\
	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(_class)								\
	static void		Install (void) 												\
						{ _factory##Get().Install(_classid, &New); }			\
	virtual bool	Read (GenericReader& reader);								\
	virtual void	Write (GenericWriter& writer) const;						\
	virtual const std::string ToString (void) const;

///////////////////////////////////////////////////////////
// Do not export as having only inline members.

class TextFileReader : public GenericReader {
	private:
	FILE* fp;

	template <class T> bool readint (T* b) {
		util_ui32 val; 
		if (fscanf(fp, "%u", &val) != 1) 
			return false; 
		*b = (T) val;				
		return true;
	}

	public:
	bool eof (void) { return feof(fp); }
	bool read (std::string& at, bool quoted)
			{ return quoted ? ureadquotedstring(fp, at) : ureadstring(fp, at); }
	bool read (util_ui32* b) 
			{ return fscanf(fp, "%u", b) == 1; }
	bool read (util_i32* b)
			{ return fscanf(fp, "%d", b) == 1; }
	bool read (util_ui16* b) { return readint(b); }
	bool read (util_i16* b){ return readint(b); }
	bool read (util_ui8* b) { return readint(b); }
	bool read (float* b) { return fscanf(fp, "%f", b) == 1; }

	bool read (double* b) { 
		float f; 
		bool r = (fscanf(fp, "%f", &f) == 1);
		*b = f;
		return r;
	}

	bool read (bool* b) 
		{ return ureadbool(fp, b); }

	bool read (void* b, util_ui32 n) {
			while (n--) {
				util_ui32 byte;
				if (fscanf(fp, "%u", &byte) != 1)
					return false;
				*((util_ui8*) b) = (util_ui8) byte;
				PINC(b, 1);
			}
			return true;
	}

	const std::string context (void) const
		{ return uconstructstr("[TEXT FILE(0x%x)]", (util_ui32) fp); }

	TextFileReader (FILE* _fp) : fp(_fp){}
	TextFileReader (const TextFileReader& r) : fp(r.fp){}
	~TextFileReader(){}
};

///////////////////////////////////////////////////////////
// Do not export as having only inline members.

class BinFileReader : public GenericReader {
	private:
	FILE* fp;
	template <class T> bool read_template (T* b)
		{ DASSERT(fp); return fread(b, sizeof(*b), 1, fp) == 1; }

	public:
	bool eof (void) { DASSERT(fp); return feof(fp); }
	bool read (std::string& at, bool quoted)
			{ DASSERT(fp); return uloadstrex(fp, at); }

	bool read (util_ui32* b)	{ return read_template(b); }
	bool read (util_i32* b)		{ return read_template(b); }
	bool read (util_ui16* b)	{ return read_template(b); }
	bool read (util_i16* b)		{ return read_template(b); }
	bool read (util_ui8* b)		{ return read_template(b); }
	bool read (float* b)		{ return read_template(b); }
	bool read (double* b)		{ return read_template(b); }
	bool read (bool* b)			{ return read_template(b); }

	bool read (void* b, util_ui32 n) 
		{ DASSERT(fp); return fread(b, sizeof(util_ui8), n, fp) == n; }

	const std::string context (void) const
		{ DASSERT(fp); return uconstructstr("[BIN FILE(0x%x)]", (util_ui32) fp); }

	BinFileReader (FILE* _fp) : fp(_fp){}
	BinFileReader (const BinFileReader& r) : fp(r.fp){}
	~BinFileReader(){}
};

///////////////////////////////////////////////////////////
// Do not export as having only inline members.

class PortableBinFileReader : public GenericReader {

	private:
	FILE* fp;
	bool read_float (float* b) { 
		double v; 
		if (!ubinaryio::ReadDouble(fp, &v)) 
			return false;
		else
			{ *b = (float) v; return true; }
	}

	public:
	bool eof (void) 
			{ DASSERT(fp); return feof(fp); }

	bool read (std::string& at, bool quoted)
			{ DASSERT(fp); return ubinaryio::ReadString(fp, at); }

	bool read (util_ui32* b)	{ return ubinaryio::ReadInteger(fp, b); }
	bool read (util_i32* b)		{ return ubinaryio::ReadInteger(fp, b); }
	bool read (util_ui16* b)	{ return ubinaryio::ReadInteger(fp, b); }
	bool read (util_i16* b)		{ return ubinaryio::ReadInteger(fp, b); }
	bool read (util_ui8* b)		{ return ubinaryio::ReadByte(fp, b); }
	bool read (float* b)		{ return read_float(b); }
	bool read (double* b)		{ return ubinaryio::ReadDouble(fp, b); }
	bool read (bool* b)			{ return ubinaryio::ReadBool(fp, b); }

	bool read (void* b, util_ui32 n) 
		{ DASSERT(fp); return ubinaryio::ReadBuffer(fp, b, n); }

	const std::string context (void) const
		{ DASSERT(fp); return uconstructstr("[PORTABLE BIN FILE(0x%x)]", (util_ui32) fp); }

	PortableBinFileReader (FILE* _fp) : fp(_fp){}
	PortableBinFileReader (const PortableBinFileReader& r) : fp(r.fp){}
	~PortableBinFileReader(){}
};

///////////////////////////////////////////////////////////
// Do not export as having only inline members.

class PortableBufferReader : public GenericReader {

	private:
	ubinaryio::InputBuffer* input;

	bool read_float (float* b) { 
		double v; 
		if (!input->ReadDouble(&v)) 
			return false;
		else
			{ *b = (float) v; return true; }
	}

	public:
	bool eof (void) 
		{ return input->IsEof(); }

	bool read (std::string& at, bool quoted)
			{ return input->ReadString(at); }

	bool read (util_ui32* b)	{ return input->ReadInteger(b); }
	bool read (util_i32* b)		{ return input->ReadInteger(b); }
	bool read (util_ui16* b)	{ return input->ReadInteger(b); }
	bool read (util_i16* b)		{ return input->ReadInteger(b); }
	bool read (util_ui8* b)		{ return input->ReadByte(b); }
	bool read (float* b)		{ return read_float(b); }
	bool read (double* b)		{ return input->ReadDouble(b); }
	bool read (bool* b)			{ return input->ReadBool(b); }

	bool read (void* b, util_ui32 n) 
		{ return input->ReadBuffer(b, n); }

	const std::string context (void) const{
		return	uconstructstr(
					"[input buffer: size %d, remaining %d]", 
					input->GetSize(),
					input->GetRemainingBytes()
				);
	}

	PortableBufferReader (ubinaryio::InputBuffer* b) : input(b){}
	PortableBufferReader (const PortableBufferReader& r) : input(r.input){}
	~PortableBufferReader(){}
};

///////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.