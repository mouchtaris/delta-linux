/**
 *	XMLConfigurationLoader.cpp
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	July 2010
 */

#include <assert.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>
#include "XMLConfigurationLoader.h"

namespace iviews {

//-----------------------------------------------------------------------

bool XMLConfigurationLoader::operator()(const wxString & path, 
										LayoutParams & lparams, 
										RenderingParams & rParams)
{
	wxXmlDocument doc;
	if(!doc.Load(path, wxT("UTF-8"), wxXMLDOC_KEEP_WHITESPACE_NODES)) {
		wxMessageBox(_("While opening xml file: '") + path + _("'"), _("Error"), wxICON_ERROR);
		return false;
	}

	// start processing the XML file
	if (doc.GetRoot()->GetName() != _("configuration_params"))
		return false;

	wxXmlNode *child = doc.GetRoot()->GetChildren();

	while (child) {
		if (child->GetName() == wxT("Layout")) 
			GetLayoutParams(child, lparams);
		else if (child->GetName() == wxT("Rendering")) 
			GetRenderingParams(child, rParams);
		child = child->GetNext();
	}
	return true;
}

//-----------------------------------------------------------------------

bool XMLConfigurationLoader::ParseBoolean(const wxString & value, const wxString & type) {
	bool tmp = false;
	if (type == wxT("boolean"))
		tmp = (value == wxT("true") ? true : false);
	else 
		assert(0);
	return tmp;
}

//-----------------------------------------------------------------------

double XMLConfigurationLoader::ParseDouble(const wxString & value, const wxString & type) {
	double tmp = -1;
	if (type == wxT("double"))
		value.ToDouble(&tmp);
	else 
		assert(0);
	return tmp;
}

//-----------------------------------------------------------------------

long XMLConfigurationLoader::ParseLong(const wxString & value, const wxString & type) {
	long tmp = -1;
	if (type == wxT("long")) 
		value.ToLong(&tmp);
	else 
		assert(0);
	return tmp;
}

//-----------------------------------------------------------------------

std::string XMLConfigurationLoader::ParseString(const wxString & value, const wxString & type) 
	{ return ""; }

//-----------------------------------------------------------------------

#define SET_COLOUR(COLOUR)				\
		token	= tkz.GetNextToken();	\
		token.ToLong(&val);				\
		colour.Set##COLOUR(val)

geo::Colour	XMLConfigurationLoader::ParseColour(const wxString & value, const wxString & type) {
	geo::Colour colour;

	if (type == wxT("colour")) {
		wxStringTokenizer tkz(value, wxT(" "));
		
		if(tkz.CountTokens() == 3 || tkz.CountTokens() == 4) {
			long val = 0;
			wxString token;

			SET_COLOUR(Red);
			SET_COLOUR(Green);
			SET_COLOUR(Blue);
			if (tkz.HasMoreTokens()) {	SET_COLOUR(Alpha);	}
		}
		else
			assert(0);
	}
	else 
		assert(0);
	return colour;
}

//-----------------------------------------------------------------------

bool XMLConfigurationLoader::GetLayoutParams(wxXmlNode *child, LayoutParams & lparams){
	wxXmlProperty * property = child->GetProperties();
	
	if (property == static_cast<wxXmlProperty *>(0))
		return false;

	wxString value	= property->GetNext()->GetValue();
	wxString type	= property->GetNext()->GetNext()->GetValue();

	if (property->GetValue() == _("drawVertexWithFullContents"))
		lparams.layerParams.drawVertexWithFullContents = ParseBoolean(value, type);

	else if (property->GetValue() == _("verticesDistance"))
		lparams.layerParams.verticesDistance = ParseDouble(value, type);

	else if (property->GetValue() == _("layersDistance"))
		lparams.layerParams.layersDistance = ParseDouble(value, type);
	
	else if (property->GetValue() == _("layersAlignment")) {
	
	switch(ParseLong(value, type)){
			case 0:
				lparams.layerParams.layersAlignment = LayerLayoutParams::TOP;
				break;
			case 1:
				lparams.layerParams.layersAlignment = LayerLayoutParams::BOTTOM;
				break;
			case 2:
				lparams.layerParams.layersAlignment = LayerLayoutParams::MIDDLE;
				break;
			default:	assert(0);
		};
	}

	else if (property->GetValue() == _("distanceFromLayerWidth"))
		lparams.layerParams.distanceFromLayer.SetWidth(ParseDouble(value, type));
	
	else if (property->GetValue() == _("distanceFromLayerHeight"))
		lparams.layerParams.distanceFromLayer.SetHeight(ParseDouble(value, type));
	
	else if (property->GetValue() == _("distanceFromTopLeftCornerWidth"))
		lparams.layerParams.distanceFromTopLeftCorner.SetWidth((ParseDouble(value, type)));
	
	else if (property->GetValue() == _("distanceFromTopLeftCornerHeight"))
		lparams.layerParams.distanceFromTopLeftCorner.SetHeight((ParseDouble(value, type)));
	
	else if (property->GetValue() == _("vertexWidth"))
		lparams.vertexParams.width = ParseDouble(value, type);

	else if (property->GetValue() == _("bookmarkWidth"))
		lparams.vertexParams.bookmarkWidth = ParseDouble(value, type);
	
	else if (property->GetValue() == _("expandButtonWidth"))
		lparams.vertexParams.expandButtonWidth = ParseDouble(value, type);
	
	else if (property->GetValue() == _("absRefDistanceWidth"))
		lparams.vertexParams.absRefDistance.SetWidth((ParseDouble(value, type)));
	
	else if (property->GetValue() == _("absRefDistanceHeight"))
		lparams.vertexParams.absRefDistance.SetHeight((ParseDouble(value, type)));
	
	else if (property->GetValue() == _("distanceFromRectWidth"))
		lparams.vertexParams.distanceFromRect.SetWidth((ParseDouble(value, type)));
	
	else if (property->GetValue() == _("distanceFromRectHeight"))
		lparams.vertexParams.distanceFromRect.SetHeight((ParseDouble(value, type)));

	else {
		assert(0);
		return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------

bool XMLConfigurationLoader::GetRenderingParams(wxXmlNode *child, RenderingParams & rParams) {
	wxXmlProperty * property = child->GetProperties();
	
	if (property == static_cast<wxXmlProperty *>(0))
		return false;

	wxString value	= property->GetNext()->GetValue();
	wxString type	= property->GetNext()->GetNext()->GetValue();

	if (property->GetValue() == _("absRefColour"))
		rParams.colours.absRefColour = ParseColour(value, type);

	else if (property->GetValue() == _("defaultColour"))
		rParams.colours.defaultColour = ParseColour(value, type);

	else if (property->GetValue() == _("delegationColour"))
		rParams.colours.delegationColour = ParseColour(value, type);

	else if (property->GetValue() == _("inheritanceColour"))
		rParams.colours.inheritanceColour = ParseColour(value, type);

	else if (property->GetValue() == _("operatorColour"))
		rParams.colours.operatorColour = ParseColour(value, type);
	
	else if (property->GetValue() == _("attributeColour"))
		rParams.colours.attributeColour = ParseColour(value, type);
	
	else if (property->GetValue() == _("externidColour"))
		rParams.colours.externidColour = ParseColour(value, type);
	
	else if (property->GetValue() == _("internalColour"))
		rParams.colours.internalColour = ParseColour(value, type);

	else if (property->GetValue() == _("backEdgeColour"))
		rParams.colours.backEdgeColour = ParseColour(value, type);

	else if (property->GetValue() == _("innerUpwardEdgeColour"))
		rParams.colours.innerUpwardEdgeColour = ParseColour(value, type);

	else if (property->GetValue() == _("innerDownwardEdgeColour"))
		rParams.colours.innerDownwardEdgeColour = ParseColour(value, type);

	else if (property->GetValue() == _("forwardEdgeColour"))
		rParams.colours.forwardEdgeColour = ParseColour(value, type);

	else if (property->GetValue() == _("pathHighlightingIncomingEdgesColour"))
		rParams.colours.pathHighlightingIncomingEdgesColour = ParseColour(value, type);

	else if (property->GetValue() == _("pathHighlightingOutgoingEdgesColour"))
		rParams.colours.pathHighlightingOutgoingEdgesColour = ParseColour(value, type);

	else if (property->GetValue() == _("highlightCurrentSlotColour"))
		rParams.colours.highlightCurrentSlotColour = ParseColour(value, type);

	else if (property->GetValue() == _("highlightcurrentVertexColour"))
		rParams.colours.highlightCurrentVertexColour = ParseColour(value, type);

	else if (property->GetValue() == _("highlightRelativeSlotsColour"))
		rParams.colours.highlightRelativeSlotsColour = ParseColour(value, type);

	else if (property->GetValue() == _("selectionColour"))
		rParams.colours.selectionColour = ParseColour(value, type);

	else {
		assert(0);
		return false;
	}
	
	return true;
}



}	//namespace iviews
