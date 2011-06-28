
#ifndef COMMONDEFS_H
#define COMMONDEFS_H

#ifdef WXMAKINGDLL_ADDITIONALWIDGETS
    #define WXDLLIMPEXP_ADDITIONALWIDGETS                  WXEXPORT
    #define WXDLLIMPEXP_DATA_ADDITIONALWIDGETS(type)       WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_ADDITIONALWIDGETS                  WXIMPORT
    #define WXDLLIMPEXP_DATA_ADDITIONALWIDGETS(type)       WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_ADDITIONALWIDGETS
    #define WXDLLIMPEXP_DATA_ADDITIONALWIDGETS(type)	    type
#endif

#endif // COMMONDEFS_H
