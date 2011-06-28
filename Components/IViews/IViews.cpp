/*
 * IViews.cpp
 * Component
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifdef _WIN32
	// Disable warning messages 4996. Security warning about localtime usage.
	#pragma warning( disable : 4996)
#endif


///C++ std includes
#include <assert.h>

#include "TypesConverter.h" 

#include "Call.h"
#include "IViews.h"
#include "Streams.h"
#include "ExtProperties.h"
#include "PropertyUtils.h"
#include "ComponentRegistry.h"
#include "ComponentFunctionCallerSafe.h"
#include "ComponentConfigurationDialog.h"

namespace ide {
	
wxFont					IViews::font;
iviews::LayoutParams	IViews::layoutParams;
iviews::RenderingParams	IViews::renderingParams;


///******************************************************************
/// component initialization
///

//-----------------------------------------------------------------------
COMPONENT_METADATA(
	IViews,
	_("IViews"),
	_(""),
	_T("Koutsopoulos Nikolaos <koutsop@ics.forth.gr>"),
	_T("alpha")
);
IMPLEMENT_IDE_COMPONENT(IViews);

//-----------------------------------------------------------------------

#define ADD_PROPERTY(ADD_TO, PROPERTY, ID, LABEL, VALUE, DESC)	\
	ADD_TO AddProperty(											\
		ID,														\
		new PROPERTY(											\
			_(LABEL),											\
			VALUE,												\
			_(DESC)												\
		)														\
	)

#define ADD_ENUM_STRIG_PROPERTY(ID, LABEL, VALUE, OPTION, DESC)	\
	table.AddProperty(											\
		ID,														\
		new EnumStringProperty(									\
			_(LABEL),											\
			VALUE,												\
			OPTION,												\
			_(DESC)												\
		)														\
	)

#define ADD_REAL_PROPERTY(ID, LABEL, VALUE, DESC)				\
	ADD_PROPERTY(table., RealProperty, ID, LABEL, VALUE, DESC)

#define ADD_FOONT_PROPERTY(ID, LABEL, VALUE, DESC)				\
	ADD_PROPERTY(table., FontProperty, ID, LABEL, VALUE, DESC)

#define ADD_BOOL_PROPERTY(ID, LABEL, VALUE, DESC)				\
	ADD_PROPERTY(table., BoolProperty, ID, LABEL, VALUE, DESC)

#define ADD_COLOUR_PROPERTY(ID, LABEL, VALUE, DESC)				\
	ADD_PROPERTY(table., ColorProperty, ID, LABEL, VALUE, DESC)

#define ADD_LAYOUT_BOOL_PROPERTY(ID, LABEL, VALUE, DESC)				\
	ADD_PROPERTY(layoutProp->, BoolProperty, ID, LABEL, VALUE, DESC)

#define ADD_LAYOUT_REAL_PROPERTY(ID, LABEL, VALUE, DESC)				\
	ADD_PROPERTY(layoutProp->, RealProperty, ID, LABEL, VALUE, DESC)

#define ADD_RENDERING_COLOUR_PROPERTY(ID, LABEL, VALUE, DESC)			\
	ADD_PROPERTY(renderingProp->, ColorProperty, ID, LABEL, VALUE, DESC)



#define RED()				wxColour(255, 0	 , 0)
#define CYAN()				wxColour(0	, 255, 255)
#define BLUE()				wxColour(0	, 0	 , 255)
#define BLACK()				wxColour(0	, 0	 , 0)
#define GREEN()				wxColour(0	, 100, 0)
#define YELLOW()			wxColour(255, 255, 0)
#define PURPLE()			wxColour(160, 32 , 240)
#define LIGHT_BLUE()		wxColour(0  , 100, 245)
#define SADLE_BROWN()		wxColour(139, 69 , 19)
#define LIGHT_SEA_GREEN()	wxColour(32 , 178, 170)


COMPONENT_SET_PROPERTIES_FUNCTION(IViews, table)
{
	using namespace conf;
	wxFont dFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL));
	ADD_FOONT_PROPERTY("font", "Font", dFont, "The iviews's main font");
	ADD_BOOL_PROPERTY("toDrawLayersRect", "Draw Layers' Rectangles", true, "");
	ADD_BOOL_PROPERTY("toDrawVertexCont", "Draw Vertices With Full Contents", true, "");

	////////////////////////////////////////////////////////////////////
	// Layers properties
	//
	StringVec vec;
	vec.push_back(_("Top")); vec.push_back(_("Bottom")); vec.push_back(_("Midle"));
	ADD_ENUM_STRIG_PROPERTY("layersProp", "Layers' alignment", vec, 2, "");

	////////////////////////////////////////////////////////////////////
	// Layout properties
	//
	AggregateProperty* layoutProp = new AggregateProperty(_("Layout Params"), _(""));
	ADD_LAYOUT_REAL_PROPERTY("vertexW"			, "Vertex Width", 256.0f, "Default width for all vertices");
	ADD_LAYOUT_REAL_PROPERTY("bookmarkW"		, "Bookmark Width", 128.0f, "Default width for the bookmarks");
	ADD_LAYOUT_REAL_PROPERTY("layersDist"		, "Layers Distance", 256.0f, "distance Between layers");
	ADD_LAYOUT_REAL_PROPERTY("absRefDistW"		, "Absolute Reference Distance Width", 8.0f, "Separation between absRefText and vertex Rectangle");
	ADD_LAYOUT_REAL_PROPERTY("absRefDistH"		, "Absolute Reference Distance Height", 8.0f, "Separation between absRefText and vertex Rectangle");
	ADD_LAYOUT_REAL_PROPERTY("expandButtonW"	, "Expand Button Width", 16.0f, "Default width for all expand buttons");
	ADD_LAYOUT_REAL_PROPERTY("verticesDist"		, "Vertices Distance", 64.0f, "Distance Between Vertices In Layer");
	ADD_LAYOUT_REAL_PROPERTY("distFromRectW"	, "Distance From Rectangle Width", 8.0f, "separation Between Text inside rectangle");
	ADD_LAYOUT_REAL_PROPERTY("distFromRectH"	, "Distance From Rectangle Height", 8.0f, "separation Between Text inside rectangle");
	ADD_LAYOUT_REAL_PROPERTY("innerEdgesDist"	, "Inner Edge Distance", 25.0f, "Distance between inner edges");
	ADD_LAYOUT_REAL_PROPERTY("distFromLayerW"	, "Distance From Layer Width", 32.0f, "separation Between Layer And Vertex");
	ADD_LAYOUT_REAL_PROPERTY("distFromLayerH"	, "Distance From Layer Height", 32.0f, "separation Between Layer And Vertex");
	ADD_LAYOUT_REAL_PROPERTY("distFromTopLeftCornerW", "Distance From Top Left Corner Width", 128.0f, "");
	ADD_LAYOUT_REAL_PROPERTY("distFromTopLeftCornerH", "Distance From Top Left Corner Height", 128.0f, "");
	table.AddProperty("layoutProp", layoutProp);

	////////////////////////////////////////////////////////////////////
	// Renderign properties
	//
	AggregateProperty* renderingProp = new AggregateProperty(_("Rendering Params"), _(""));
	ADD_RENDERING_COLOUR_PROPERTY("absRefColour"		, "Absolute Reference Colour"		, BLACK(), "");
	ADD_RENDERING_COLOUR_PROPERTY("defaultColour"		, "Default Colour"					, BLACK(), "");
	ADD_RENDERING_COLOUR_PROPERTY("delegationColour"	, "Delegation Colour"				, PURPLE(), "");
	ADD_RENDERING_COLOUR_PROPERTY("inheritanceColour"	, "Inheritance Colour"				, SADLE_BROWN(), "");
	ADD_RENDERING_COLOUR_PROPERTY("operatorColour"		, "Operator Colour"					, LIGHT_BLUE(), "");
	ADD_RENDERING_COLOUR_PROPERTY("attributeColour"		, "Attribute Colour"				, LIGHT_BLUE(), "");
	ADD_RENDERING_COLOUR_PROPERTY("externidColour"		, "Externid Colour"					, LIGHT_SEA_GREEN(), "");
	ADD_RENDERING_COLOUR_PROPERTY("internalColour"		, "Internal Colour"					, LIGHT_SEA_GREEN(), "");
	ADD_RENDERING_COLOUR_PROPERTY("backEdgeColour"		, "Back Edge Colour"				, BLUE(), "");
	ADD_RENDERING_COLOUR_PROPERTY("innerUpEdgeColour"	, "Inner Upward Edge Colour"		, YELLOW(), "");
	ADD_RENDERING_COLOUR_PROPERTY("innerDownEdgeColour"	, "Inner Downward Edge Colour"		, GREEN(), "");
	ADD_RENDERING_COLOUR_PROPERTY("forwardEdgeColour"	, "Forward Edge Colour"				, BLACK(), "");
	ADD_RENDERING_COLOUR_PROPERTY("highCurSlotColour"	, "Highlight Current Slot Colour"	, CYAN(), "");
	ADD_RENDERING_COLOUR_PROPERTY("highCurVertexColour"	, "Highlight Current Vertex Colour"	, wxColour(255, 165, 0), "");
	ADD_RENDERING_COLOUR_PROPERTY("selectionColour"		, "Text Selection Colour"			, CYAN(), "");
	ADD_RENDERING_COLOUR_PROPERTY("highRelativeSlotsColour"	, "Highlight Relative Slots Colour"	, BLUE(), "");
	ADD_RENDERING_COLOUR_PROPERTY("pathHighInEdgesColour"	, "Path Highlighting Incoming Edges Colour", CYAN(), "");
	ADD_RENDERING_COLOUR_PROPERTY("pathHighOutEdgesColour"	, "Path Highlighting Outgoing Edges Colour", YELLOW(), "");
	table.AddProperty("renderingProp", renderingProp);
}


///******************************************************************
/// public exported methods
///

//-----------------------------------------------------------------------

IViews::IViews(void)
{
}

//-----------------------------------------------------------------------

IViews::~IViews(void)
{
}

//-----------------------------------------------------------------------

EXPORTED_STATIC(IViews, void, Initialize, (void))
{		
	AppliedComponentProperties();
}

//-----------------------------------------------------------------------

EXPORTED_STATIC(IViews, void, CleanUp, (void))
{ }

//-----------------------------------------------------------------------

EXPORTED_CMD_STATIC(IViews, Configure, _("/{2}Configure/iviews..."), MT_MAIN, "")
	{ ComponentConfigurationDialog().Configure(s_classId); }


//-----------------------------------------------------------------------

EXPORTED_CMD_STATIC(IViews, GetExpressionFromUser, _("/{110}Debug/{110}i-views\tF4"), MT_MAIN, "")
{
	std::string selectedText=  "";
	if (HasOpenEditors())
		selectedText = GetFocusedEditorSelectedText();
	
	iviews::IviewsWindow * iviewsWindow = new iviews::IviewsWindow(
												wxGetActiveWindow(), 
												font, 
												layoutParams, 
												renderingParams
											);

	iviewsWindow->SetSelectedText(selectedText);
	iviewsWindow->Show(true);
}




///////////////////////////////////////////////////////
//	private method API implemantation

void IViews::ComponentAppliedChangedProperties (
		const conf::PropertyTable& old, 
		const conf::PropertyIdVec& changed
	) {
	AppliedComponentProperties();

	wxWindowList & children  = wxGetActiveWindow()->GetChildren();
	wxWindowList::iterator child;

	for (child = children.begin(); child != children.end(); ++child)
		if (dynamic_cast<iviews::IviewsWindow *>(*child))
			dynamic_cast<iviews::IviewsWindow *>(*child)->UpdateParams(font, layoutParams, renderingParams);
}



///////////////////////////////////////////////////////
//	private method API implemantation

//-----------------------------------------------------------------------

const conf::Property* IViews::GetProperty (const std::string& id)
	{ return ComponentRegistry::Instance().GetComponentEntry(s_classId).GetProperty(id); }	

//-----------------------------------------------------------------------
#define LOAD_PROPERTY(ID, ASSIGN_TO, TYPE)		\
	if (prop = GetProperty(ID))					\
		ASSIGN_TO = get_prop_value<TYPE>(prop);	\
	else										\
		assert(0)
//TODO check name
#define CALL_LAYOUT_PROPERTY(ID, ASSIGN_TO)											\
	if ( (prop = GetProperty("layoutProp"))	&&										\
		 (prop = dynamic_cast<const AggregateProperty *>(prop)->GetProperty(ID)) )	\
		layoutParams.ASSIGN_TO(get_prop_value<RealProperty>(prop));					\
	else																			\
		assert(0)
//TODO chaeck name
#define ASSIGN_LAYOUT_PROPERTY(ID, ASSIGN_TO)										\
	if ( (prop = GetProperty("layoutProp"))	&&										\
		 (prop = dynamic_cast<const AggregateProperty *>(prop)->GetProperty(ID)) )	\
		layoutParams.ASSIGN_TO = get_prop_value<RealProperty>(prop);				\
	else																			\
		assert(0)

#define LOAD_RENDERING_PROPERTY(ID, ASSIGN_TO)																\
	if ( (prop = GetProperty("renderingProp"))	&&															\
		 (prop = dynamic_cast<const AggregateProperty *>(prop)->GetProperty(ID)) )							\
		renderingParams.colours.ASSIGN_TO = iviews::wxColour2Colour(get_prop_value<ColorProperty>(prop));	\
	else																									\
		assert(0)

//ComponentAppliedChangedProperties
void IViews::AppliedComponentProperties(void) {
	using namespace conf;
	const Property* prop = (Property*)0;

	LOAD_PROPERTY("font", font, FontProperty);
	LOAD_PROPERTY("toDrawLayersRect"	, layoutParams.layerParams.drawLayersRect, BoolProperty);
	LOAD_PROPERTY("toDrawVertexCont"	, layoutParams.layerParams.drawVertexWithFullContents, BoolProperty);

	String alignment;
	LOAD_PROPERTY("layersProp", alignment, EnumStringProperty);

	if (alignment == _("Top"))
		layoutParams.layerParams.layersAlignment = iviews::LayerLayoutParams::TOP;
	else if (alignment == _("Bottom"))
		layoutParams.layerParams.layersAlignment = iviews::LayerLayoutParams::BOTTOM;
	else if (alignment == _("Midle"))
		layoutParams.layerParams.layersAlignment = iviews::LayerLayoutParams::MIDDLE;
	else 
		assert(0);
	
	ASSIGN_LAYOUT_PROPERTY("vertexW"				, vertexParams.width);
	ASSIGN_LAYOUT_PROPERTY("bookmarkW"				, vertexParams.bookmarkWidth);
	ASSIGN_LAYOUT_PROPERTY("layersDist"				, layerParams.layersDistance);
	ASSIGN_LAYOUT_PROPERTY("innerEdgesDist"			, layerParams.innerEdgesDistance);
	ASSIGN_LAYOUT_PROPERTY("expandButtonW"			, vertexParams.expandButtonWidth);
	ASSIGN_LAYOUT_PROPERTY("verticesDist"			, layerParams.verticesDistance);	
	ASSIGN_LAYOUT_PROPERTY("innerEdgesDist"			, layerParams.innerEdgesDistance);
	CALL_LAYOUT_PROPERTY("absRefDistW"				, vertexParams.absRefDistance.SetWidth);
	CALL_LAYOUT_PROPERTY("absRefDistH"				, vertexParams.absRefDistance.SetHeight);
	CALL_LAYOUT_PROPERTY("distFromRectW"			, vertexParams.distanceFromRect.SetWidth);
	CALL_LAYOUT_PROPERTY("distFromRectH"			, vertexParams.distanceFromRect.SetHeight);
	CALL_LAYOUT_PROPERTY("distFromLayerW"			, layerParams.distanceFromLayer.SetWidth);
	CALL_LAYOUT_PROPERTY("distFromLayerH"			, layerParams.distanceFromLayer.SetHeight);
	CALL_LAYOUT_PROPERTY("distFromTopLeftCornerW"	, layerParams.distanceFromTopLeftCorner.SetWidth);
	CALL_LAYOUT_PROPERTY("distFromTopLeftCornerH"	, layerParams.distanceFromTopLeftCorner.SetHeight);

	LOAD_RENDERING_PROPERTY("absRefColour"			, absRefColour);
	LOAD_RENDERING_PROPERTY("defaultColour"			, defaultColour);
	LOAD_RENDERING_PROPERTY("delegationColour"		, delegationColour);
	LOAD_RENDERING_PROPERTY("inheritanceColour"		, inheritanceColour);
	LOAD_RENDERING_PROPERTY("operatorColour"		, operatorColour);
	LOAD_RENDERING_PROPERTY("attributeColour"		, attributeColour);
	LOAD_RENDERING_PROPERTY("externidColour"		, externidColour);
	LOAD_RENDERING_PROPERTY("internalColour"		, internalColour);
	LOAD_RENDERING_PROPERTY("backEdgeColour"		, backEdgeColour);
	LOAD_RENDERING_PROPERTY("innerUpEdgeColour"		, innerUpwardEdgeColour);
	LOAD_RENDERING_PROPERTY("innerDownEdgeColour"	, innerDownwardEdgeColour);
	LOAD_RENDERING_PROPERTY("forwardEdgeColour"		, forwardEdgeColour);
	LOAD_RENDERING_PROPERTY("highCurSlotColour"		, highlightCurrentSlotColour);
	LOAD_RENDERING_PROPERTY("highCurVertexColour"	, highlightCurrentVertexColour);
	LOAD_RENDERING_PROPERTY("highRelativeSlotsColour", highlightRelativeSlotsColour);
	LOAD_RENDERING_PROPERTY("selectionColour"		, selectionColour);
	LOAD_RENDERING_PROPERTY("pathHighInEdgesColour"	, pathHighlightingIncomingEdgesColour);
	LOAD_RENDERING_PROPERTY("pathHighOutEdgesColour", pathHighlightingOutgoingEdgesColour);
}

//-----------------------------------------------------------------------

bool IViews::HasOpenEditors (void) {
	if ( !ComponentRegistry::Instance().GetFocusedInstance("EditorManager") )
		return false;

	HandleVec vec = Call<HandleVec (void)>(s_classId, "EditorManager", "GetEditors")();
	return !vec.size() ? false : true;
}

//-----------------------------------------------------------------------

const std::string IViews::GetFocusedEditorURI(void) {
	const Handle& han = Call<const Handle& (void)>(s_classId, "EditorManager", "GetFocusedEditor")();
	return util::str2std(Call<const String& (void)>(s_classId, han.Resolve(), "GetURI")());
}

//-----------------------------------------------------------------------

const std::string IViews::GetFocusedEditorSelectedText (void) {
	const Handle& han = Call<const Handle& (void)>(s_classId, "EditorManager", "GetFocusedEditor")();
	return util::str2std(Call<const String (void)>(s_classId, han.Resolve(), "GetSelectedText")());
}

} // namespace ide