#ifndef __DELTA_COMPILER_INVOKER__H__
#define __DELTA_COMPILER_INVOKER__H__

#include <ostream>
//
#define DSC_EXT		".dsc"
#define DBC_EXT		".dbc"
#define IDC_EXT		".idc"
#define TXT_EXT		".txt"
#define DSC(FILE)	(FILE + DSC_EXT)
#define DBC(FILE)	(FILE + DBC_EXT)
#define IDC(FILE)	(FILE + IDC_EXT)
#define TXT(FILE)	(FILE + TXT_EXT)

struct DeltaCompilerInvokerData;
class DeltaCompilerInvoker {
public:
	typedef	void	(*OnErrorCallback) (char const* message);
	bool			NeedHelp (void) const;
	std::ostream&	WriteHelpTo (std::ostream&) const;
	
	void			CompileAll (void) const;

					// --bytecode-path=path
					// --debug={anything}
					// --functions=functions_file_path
					// --parent={anything}
					// --output-directory=dirpath
					// --help={anything}
					// --symbolic-name={anything}
					// --icode
					// --text
					//
					DeltaCompilerInvoker (
							int const			argc,
							char* const			argv[],
							OnErrorCallback);
					~DeltaCompilerInvoker (void);
private:
	friend struct DeltaCompilerInvokerData;
	DeltaCompilerInvokerData* const data;
};


#ifndef DELTA_COMPILER_INVOKER__MAX_FUNCTION_DESCRIPTION_LENGTH
#define DELTA_COMPILER_INVOKER__MAX_FUNCTION_DESCRIPTION_LENGTH	4096
#endif

#endif//__DELTA_COMPILER_INVOKER__H__
