/*
 * IViews.h 
 *
 *	Interactive Object Graphs for Debuggers 
 *	with Improved Visualization, Inspection 
 *	and Configuration Features
 *
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#ifndef I_VIEWS_H
#define I_VIEWS_H


///C++ STD includes
#include <list>


/// wx includes 
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/Dialog.h>
#include <wx/textctrl.h>

#include "LayoutParams.h"
#include "IviewsWindow.h"
#include "RenderingParams.h"


#include "Common.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "ComponentBuilder.h"
#include "UndefineAllWin32Shit.h"
#include "DeltaVMFuncReturnTypes.h"


namespace ide {

class IViews : public Component	{
private:		
	///--- declear component
	DECLARE_IDE_COMPONENT(Component);

public:
	IViews(void);
	~IViews(void);

	
	DECLARE_EXPORTED_STATIC(void, Initialize, (void));
	DECLARE_EXPORTED_STATIC(void, CleanUp, (void));
	
	DECLARE_EXPORTED_STATIC(void, Configure, (void));
	DECLARE_EXPORTED_STATIC(void, GetExpressionFromUser, (void));

protected:
	virtual void					 ComponentAppliedChangedProperties (
										const conf::PropertyTable& old, 
										const conf::PropertyIdVec& changed
									);


private:
	static void						AppliedComponentProperties(void);
	static const conf::Property*	GetProperty (const std::string& id);

	static bool				 		HasOpenEditors (void);
	static const std::string 		GetFocusedEditorURI (void);
	static const std::string 		GetFocusedEditorSelectedText (void);


	static wxFont					font;
	static iviews::LayoutParams		layoutParams;
	static iviews::RenderingParams	renderingParams;

};



}	//namespace ide


#endif	//I_VIEWS_H