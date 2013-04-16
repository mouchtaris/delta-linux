/**
 *	LanguageModule.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 *	i-sense hooks, Anthony Savidis, August, 2010
 *	Goto Definition additions, A. Savidis, March 2011.
 */

#include "LanguageModule.h"

#include "LibraryAPIDecls.h"
#include "StringUtils.h"
#include "Streams.h"
#include "Namespace.h"

#include "DeltaParser/DeltaParser.h"
#include "DeltaParser/DeltaASTVisualizer.h"
#include "DeltaParser/DeltaASTPrinter.h"
#include "EditorUtils/EditorStreams.h"
#include "DeltaScintillaStyler/DeltaScintillaStyler.h"
#include "DeltaScintillaStyler/DeltaASTSymbolFinder.h"
#include "EditorMain/EditorWindow.h"

#include "DeltaStdLibFuncNames.h"
#include "LibraryNamespace.h"
#include "DeltaObject.h"
#include "DeltaTableSerialiser.h"
#include "AutoCompletion.h"
#include "GotoDefinition.h"
#include "ShMemLib.h"
#include <ctype.h>
#include <assert.h>
#include <boost/foreach.hpp>
#include <wx/accel.h>

#ifndef NO_VLD
#include <vld.h>
#endif

#define	DELTA_AUTOCOMPLETION

using namespace editor;

namespace delta {


ucallbackwithclosure<LanguageModule::OnAfterLanguageModuleCreation>		LanguageModule::onAfterCreation;
ucallbackwithclosure<LanguageModule::OnBeforeLanguageModuleDestruction>	LanguageModule::onBeforeDestruction;

////////////////////////////////////////////////////////////////////////
// LanguageModule class
//
IntList LanguageModule::hotspots;

LanguageModule::LanguageModule (void) :
	m_contentVisualization(0), 
	m_showParseIndicator(false), 
	isContentSetup(false) {

	m_parseIndicatorTimer.SetOwner(this, &LanguageModule::onParseIndicatorTimerExpired);

	// i-sense calls.
	NotifyAfterCreation(this);
	AddCallbacks();

	DeltaAutoCompletion::HandleLanguageModuleInstantiation(this);
}

//**********************************************************************

LanguageModule::~LanguageModule (void)
{
	NotifyBeforeDestruction(this);
	this->DestroyContentVisualization();

	DeltaAutoCompletion::HandleLanguageModuleDestruction(this);
	DeltaGotoDefinition::HandleLanguageModuleDestruction(this);
}

//**********************************************************************

static const std::string PureIdent (const char* s) {
	assert(s && *s && isalpha(*s)); 
	std::string id;
	do id += *s++; while (isalnum(*s) || *s == '_');
	return id;
}

static const std::string ExtractIdent (const char* s) {
	assert(s); 
	while (isspace(*s))
		++s;
	return PureIdent(s);
}

//**********************************************************************

void LanguageModule::Initialize (void) {
	Namespace::Initialize();
	const char** functions = DeltaStdLib_FuncNames();
	assert(functions);
	while(*functions)
	{
		if (**functions == *DELTA_LIBRARYNAMESPACE_COMMENT)
			;
		else if (!strcmp(*functions, DELTA_LIBRARYNAMESPACE_ENTER)) {
			if (*(functions + 1))	//not last entry
				Namespace::Enter(*++functions);
		}
		else if (!strcmp(*functions, DELTA_LIBRARYNAMESPACE_EXIT))
			Namespace::Exit();
		else if (!strcmp(*functions, DELTA_LIBRARYNAMESPACE_TYPE))
			++functions;
		else if (!strcmp(*functions, DELTA_LIBRARYNAMESPACE_CONST)) {
			if (*(functions + 1))	//not last entry
				Namespace::CurrentNamespace()->AddFunction(ExtractIdent(*++functions), true);
		}
		else
			Namespace::CurrentNamespace()->AddFunction(PureIdent(*functions), true);
		if (*functions)
			++functions;
	}

	hotspots.push_back(StdLibfuncASTNode::Type);
	hotspots.push_back(LibfuncASTNode::Type);

	ASTreeCtrl::AddCommand(0, _T("Copy node"));
	ASTreeCtrl::AddCommand(1, _T("Cut node"));
	ASTreeCtrl::AddCommand(2, _T("Delete node"));

	DeltaAutoCompletion::Initialise();
	DeltaGotoDefinition::Initialise();
}

//**********************************************************************

void LanguageModule::CleanUp (void) {
	DeltaAutoCompletion::CleanUp();
	DeltaGotoDefinition::CleanUp();

	ASTreeCtrl::ClearCommands();
	Namespace::CleanUp();
	hotspots.clear();
}

//**********************************************************************

void LanguageModule::SetEditor (editor::EditorWindow* editor) {
	LanguageModuleIface::SetEditor(editor);
	editor->AddContextMenuAction(
		"Go To Definition",
		(void(*)(void*, int, int)) &DeltaGotoDefinition::GotoDefinition,
		&DeltaGotoDefinition::GotoDefinitionPrecond,
		this
	);
	editor->AddContextMenuAction(
		"Backward <-",
		&DeltaGotoDefinition::GoBackward,
		&DeltaGotoDefinition::GoBackwardPrecond,
		this
	);
	editor->AddContextMenuAction(
		"Forward  ->",
		&DeltaGotoDefinition::GoForward,
		&DeltaGotoDefinition::GoForwardPrecond,
		this
	);
}

bool  LanguageModule::GotoGlobalDefinition (const std::string& id) 
	{ return DeltaGotoDefinition::GotoGlobalFunction(this, id); }

//**********************************************************************

void LanguageModule::ContentSetup (void)
{
	EditorWindow* editor = this->GetEditor();
	
	typedef StmtsASTNode* StmtsASTNode_ptr_t;

	SetIsContentSetup();	// i-sense call.
	this->parseSlice(Slice(Range(0, editor->GetLength()), StmtsASTNode_ptr_t(0)));
													// MSVC compiler cannot implicitly convert 
													// an int to a NULL pointer through many 
													// layers of template instatiations
	ResetIsContentSetup();

	if (m_contentVisualization)
		m_contentVisualization->ExpandRoot();
}

void LanguageModule::ContentCleared (void)
{
	EditorWindow* editor = this->GetEditor();
	ScintillaStyler::ClearIndicators(editor, m_progDesc, 0, editor->GetLength());
	m_progDesc.ClearAll();
}

//**********************************************************************

void LanguageModule::ContentAdded (
		uint			atPos,
		uint			length,
		uint			firstLine,
		uint			lastLine
	)
{
	this->updatePositions(atPos, (int) length);
	Slice slice = m_progDesc.GetAffectedSliceAfterAdd(Range(atPos, atPos + length));
	this->expandAffectedSliceToIncludeBlockComments(slice);
	this->parseSlice(slice);
	DeltaGotoDefinition::HandleContentEdited(this);
}

	// Added for i-sense (AS).
void LanguageModule::CursorRepositioned (uint atPos) 
	{ m_progDesc.UpdadeFocusNode(atPos); }

const std::string LanguageModule::GetCursorFocusInformation (void) const
	{ return m_progDesc.GetFocusNodeInformation(); }

//**********************************************************************

void LanguageModule::ContentDeleted (
		uint			atPos,
		uint			length,
		uint			firstLine,
		uint			lastLine
	)
{
	this->updatePositions(atPos, -((int) length));
	Slice slice = m_progDesc.GetAffectedSliceAfterRemove(Range(atPos, atPos + length));
	this->expandAffectedSliceToIncludeBlockComments(slice);
	this->parseSlice(slice);
	DeltaGotoDefinition::HandleContentEdited(this);
}

//**********************************************************************

void LanguageModule::ContentOffsetChanged (
		uint			atPos,
		int				offset
	)
{
	m_progDesc.UpdateParseErrorPositions(atPos, offset);
	m_progDesc.UpdateSemanticErrorPositions(atPos, offset);
}

void LanguageModule::MouseHoverStarted  (uint atPos) {
	if (DeltaASTNode* node = m_progDesc.GetNode(atPos))	// i-sense call
		NotifyMouseHoverStarted(node);
}

void LanguageModule::MouseHoverEnded  (void) 
	{ NotifyMouseHoverEnded(); }	// i-sense call

//**********************************************************************

void LanguageModule::CharacterAdded (int keyCode)
{
	NotifyCharacterAdded(keyCode);	// i-sense call

	EditorWindow* editor = this->GetEditor();

	switch (keyCode) {

#if	0	// Disable basic auto completion for objects.
		case '.':
			this->completeSymbol(SYMBFL_KEY | SYMBFL_ID, editor->GetCurrentPos());
			break;
		case '[':
			this->completeSymbol(SYMBFL_KEY | SYMBFL_VAR, editor->GetCurrentPos());
			break;
		case '@':
			this->completeSymbol(SYMBFL_ATTR, editor->GetCurrentPos());
			break;
#endif
		case '{': //FALLTHROUGH
		case '}': //FALLTHROUGH
		case ']': //FALLTHROUGH
		case '\n':
			this->setIdentLevel(keyCode);
			break;

		default: {

#ifdef	DELTA_AUTOCOMPLETION
			DeltaAutoCompletion::Handle(this, GetEditor(), m_progDesc, keyCode);
#else
			if (!editor->AutoCompActive() && DeltaASTSymbolFinder::IsIdChar(keyCode)) {
				int endPos		= editor->GetCurrentPos();
				int startPos	= editor->WordStartPosition(endPos, true);

				if (endPos - startPos > 2)
					this->completeSymbol(SYMBFL_FUNC | SYMBFL_VAR, endPos, startPos);
			}
#endif
		}	// default
	}	// switch
}

//**********************************************************************
// i-sense additions to support quick (semantic) info tooltip

bool LanguageModule::GetInfoForPosition (
		uint			pos,
		uint*			startPos,
		uint*			endPos,
		int*			type,
		String*			msg
	) {
	EditorWindow* editor = this->GetEditor();

	if (editor->GetStyleAt(pos) & wxSCI_INDIC0_MASK) {	// Marked as error text, but was parsed.

		ErrorRange range = m_progDesc.GetParseError(pos);
		if (range.left == range.right && range.errorMsg.empty())
			range = m_progDesc.GetSemanticError(pos);
		this->setInfoIndicator(m_infoAtRange, range, wxSCI_INDIC2_MASK);

		*startPos = range.left;
		*endPos = range.right;
		*type = -1;
		*msg = range.errorMsg;
	}
	else {
		const DeltaASTNode::Range& range = m_progDesc.GetNotParsed(pos);
		if (range) {	// Error but not yet parsed source text (normally follows an erroneous code fragment).
			this->setInfoIndicator(m_infoAtRange, range, wxSCI_INDIC2_MASK);
			*startPos = range.left;
			*endPos = range.right;
			*type = -2;
			*msg = _("Cannot recognize symbols due to parse errors");
		}
		else if (DeltaASTNode* node = m_progDesc.GetNode(pos)) { // Syntactically correct text, thus normally parsed text.

			const DeltaASTNode::Range range = node->GetRange();
			this->setInfoIndicator(m_infoAtRange, range, wxSCI_INDIC2_MASK);

			*startPos = range.left;
			*endPos = range.right;
			*type = node->GetType();

			DeltaASTPrinter printer;
			node->AcceptNode(&printer);
			*msg = _("Syntax information:\n") + printer.GetDescription();
		}
		else	//no position information
			return false;
	}
	return true;
}

//**********************************************************************

void LanguageModule::CancelDisplayedInfo (void)
	{ this->clearInfoIndicator(m_infoAtRange, wxSCI_INDIC2_MASK); }

//**********************************************************************
// Setting library functions.

void LanguageModule::SetByteCodeLoadingPath (const std::string& path) {

	EditorWindow* editor = this->GetEditor();
	DeltaAutoCompletion::SetByteCodeLoadingPath(this, editor, m_progDesc.GetAST(), path);

	ScintillaStyler::ClearSemanticErrorIndicators(editor, m_progDesc, 0, editor->GetLength());
	Namespace::CloseAll();
	checkNamespaces(m_progDesc.GetAST());
	ScintillaStyler::StyleProgram(editor, m_progDesc, 0, editor->GetLength());
	ScintillaStyler::SetIndicators(editor, m_progDesc, 0, editor->GetLength());
}

void LanguageModule::SetExtraLibraryDefinitions (const StringList& keywords) {

	if (!libfuncs.empty()) {

		StringList::iterator i = libfuncs.begin();
		StringList::iterator end = libfuncs.end();

		while(i != end) {
			std::string s = util::strip<std::string>(util::str2std(*i), UTIL_WHITESPACE);
			if (s[0] == *DELTA_LIBRARYNAMESPACE_COMMENT)
				;
			else if (s == DELTA_LIBRARYNAMESPACE_ENTER) {
				if (&*i != &libfuncs.back()) {
					s = util::strip<std::string>(util::str2std(*++i), UTIL_WHITESPACE);
					Namespace::Enter(s, true);
				}
			}
			else if (s == DELTA_LIBRARYNAMESPACE_EXIT)
				Namespace::Exit(true);
			else if (s == DELTA_LIBRARYNAMESPACE_TYPE)
				++i;
			else if (s == DELTA_LIBRARYNAMESPACE_CONST) {
				if (&*i != &libfuncs.back()) {
					s = util::strip<std::string>(util::str2std(*++i), UTIL_WHITESPACE);
					Namespace::CurrentNamespace()->RemoveFunction(ExtractIdent(s.c_str()));
				}
			}
			else
				Namespace::CurrentNamespace()->RemoveFunction(PureIdent(s.c_str()));
			if (i != end)
				++i;
		}
	}

	libfuncs = keywords;
	StringList::const_iterator i = libfuncs.begin();
	StringList::const_iterator end = libfuncs.end();

	while(i != end) {
		std::string s = util::strip<std::string>(util::str2std(*i), UTIL_WHITESPACE);
		if (s[0] == *DELTA_LIBRARYNAMESPACE_COMMENT)
			;
		else if (s == DELTA_LIBRARYNAMESPACE_ENTER) {
			if (&*i != &libfuncs.back()) {
				s = util::strip<std::string>(util::str2std(*++i), UTIL_WHITESPACE);
				Namespace::Enter(s);
			}
		}
		else if (util::str2std(*i) == DELTA_LIBRARYNAMESPACE_EXIT)
			Namespace::Exit();
		else if (s == DELTA_LIBRARYNAMESPACE_TYPE)
			++i;
		else if (s == DELTA_LIBRARYNAMESPACE_CONST) {
			if (&*i != &libfuncs.back()) {
				s = util::strip<std::string>(util::str2std(*++i), UTIL_WHITESPACE);
				Namespace::CurrentNamespace()->AddFunction(ExtractIdent(s.c_str()), false);
			}
		}
		else
			Namespace::CurrentNamespace()->AddFunction(PureIdent(s.c_str()), false);
		if (i != end)
			++i;
	}

	std::list<std::string> funcs;
	for (i = libfuncs.begin(); i != end; ++i)
		funcs.push_back(util::str2std(*i));
	DeltaAutoCompletion::SetLibraryFunctions(funcs, this);

	EditorWindow* editor = this->GetEditor();
	ScintillaStyler::ClearSemanticErrorIndicators(editor, m_progDesc, 0, editor->GetLength());
	Namespace::CloseAll();
	checkNamespaces(m_progDesc.GetAST());

	ScintillaStyler::StyleProgram(editor, m_progDesc, 0, editor->GetLength());
	ScintillaStyler::SetIndicators(editor, m_progDesc, 0, editor->GetLength());
}

//**********************************************************************

const IntList LanguageModule::GetHotspots (void) const
	{ return hotspots; }

const SerializedEditorAST LanguageModule::GetAST (void) const {
	StmtsASTNode *ast = const_cast<ProgramDescription&>(m_progDesc).GetAST();
	if (ast) {
		DeltaTable* table = ast->ToTable();
		unsigned size;
		void *data = DeltaTableSerialiser().Serialise(table, &size);
		DeltaObject::NativeCodeHelpers::Delete(table);
		SerializedEditorAST buffer(data, size);
		DeltaTableSerialiser::DestroyBuffer(data);
		return buffer;
	}
	else
		return SerializedEditorAST();
}

//**********************************************************************

void LanguageModule::SharedWriteAST (void) const {
	if (StmtsASTNode *ast = const_cast<ProgramDescription&>(m_progDesc).GetAST())
		DeltaSharedMemory::Write("ast", ast->ToTable());
}

//**********************************************************************

wxWindow* LanguageModule::VisualizeContent (wxWindow* parent)
{
	this->DestroyContentVisualization();

	m_contentVisualization = new ASTreeCtrl(parent, &m_progDesc,
		boost::bind(&LanguageModule::onVisualizerCommand, this, _1, _2));
	m_contentVisualization->ExpandRoot();

	return m_contentVisualization;
}

//**********************************************************************

void LanguageModule::DestroyContentVisualization (void)
{
	if (m_contentVisualization)
		m_contentVisualization->Destroy();
	m_contentVisualization = 0;
}

//**********************************************************************

void LanguageModule::GetAccelerators (wxAcceleratorEntry** entry, int* size)
{
	static wxAcceleratorEntry entries[] = {
		wxAcceleratorEntry(wxACCEL_CTRL | wxACCEL_ALT, int('P'), 0),
		wxAcceleratorEntry(wxACCEL_CTRL | wxACCEL_ALT, int('G'), 1),
		wxAcceleratorEntry(wxACCEL_CTRL, WXK_SPACE, 2),
		wxAcceleratorEntry(wxACCEL_CTRL, int('K'), 3),
		wxAcceleratorEntry(wxACCEL_CTRL | wxACCEL_SHIFT, int('K'), 4)
	};
	*entry = entries;
	*size = SIZEOF_ARRAY(entries);
}

//**********************************************************************

void LanguageModule::DispatchAcceleratorCommand (int command)
{
	DBGOUT << "--> Accelerator command: " << command << DBGENDL;
	EditorWindow* editor = this->GetEditor();

	switch (command) {

		case 0: m_showParseIndicator = !m_showParseIndicator; return;

		case 1: {
			DeltaASTNode* node = m_progDesc.GetNodeAbove(editor->GetCurrentPos());
			if (m_contentVisualization)
				m_contentVisualization->Focus(node);
			else
				this->onVisualizerCommand(node, -1);
			break;
		}

		case 2: 
			if (!DeltaAutoCompletion::Handle(this, GetEditor(), m_progDesc))
				this->completeSymbol(editor->GetCurrentPos());
			break;

		case 3: this->commentSelection(); return;
		case 4: this->uncommentSelection(); return;

		default: return;
	}
}

//**********************************************************************

void LanguageModule::onVisualizerCommand (DeltaASTNode* node, int command)
{
	DBGOUT << "--> OnCommand" << DBGENDL;
	const Range& range = node->GetRange();

	switch (command) {
		case -1: this->GetEditor()->SetSelection(range.left, range.right); break;
		case -2: break;
		case -3: break;

		case 0:	this->GetEditor()->Copy();	break;
		case 1: this->GetEditor()->Cut();	break;
		case 2: this->GetEditor()->Clear(); break;
		default:
			break;
	}
}

//**********************************************************************

void LanguageModule::onParseIndicatorTimerExpired (void)
	{ this->clearInfoIndicator(m_parseInfoAtRange, wxSCI_INDIC1_MASK); }

//**********************************************************************

void LanguageModule::setInfoIndicator (Range& savedRange, const Range& range, int mask)
{
	this->clearInfoIndicator(savedRange, mask);

	if (range) {
		this->GetEditor()->AddIndicator((int) range.left, (int) range.right, mask);
		savedRange = range;
	}
}

//**********************************************************************

void LanguageModule::clearInfoIndicator (Range& range, int mask)
{
	if (range) {
		this->GetEditor()->RemoveIndicator((int) range.left, (int) range.right, mask);
		range = Range();
	}		
}

//**********************************************************************

void LanguageModule::updatePositions (uint atPos, int offset)
{
	m_progDesc.UpdateAllPositions(atPos, offset);

	m_infoAtRange.Update(atPos, offset);
	m_parseInfoAtRange.Update(atPos, offset);
}

//**********************************************************************

void LanguageModule::parseSlice (const Slice& slice)
{
	DBGOUT << "==== PARSING: " << slice.first << DBGENDL;

	if (!slice.first)
		return;

	EditorWindow* editor = this->GetEditor();

	ScintillaStyler::ClearIndicators(editor, m_progDesc, slice.first.left, slice.first.right);
	m_progDesc.Remove(slice);

	EditorInputStream input(editor, slice.first.left, slice.first.right);
	DeltaParser(m_progDiff, slice.first.left).ParseStream(input);

	bool hasParseErrors = !m_progDiff.GetParseErrors().Empty();
	m_progDesc.Merge(slice, m_progDiff);

	if (IsContentSetup())	// Initial loading of the entire file.
		NotifyFileParsed(m_progDesc.GetAST());	// i-sense call
	
	if (!hasParseErrors) {
		Namespace::CloseAll();
		DeltaAutoCompletion::HandleSourceCodeUpdate(this, editor, m_progDesc.GetAST());	// i-sense call
		checkNamespaces(m_progDesc.GetAST());
	}

	ScintillaStyler::StyleProgram(editor, m_progDesc, slice.first.left, editor->GetLength());
	ScintillaStyler::SetIndicators(editor, m_progDesc, slice.first.left, slice.first.right);

	if (m_showParseIndicator) {
		this->setInfoIndicator(m_parseInfoAtRange, slice.first, wxSCI_INDIC1_MASK);
		m_parseIndicatorTimer.Start(2000, wxTIMER_ONE_SHOT);
	}
}

//**********************************************************************

void LanguageModule::expandAffectedSliceToIncludeBlockComments (Slice& slice) {
	EditorWindow* editor = this->GetEditor();
	int start = editor->FindText(slice.first.left, slice.first.right, _T("/*"));
	if (start >= slice.first.left && start < slice.first.right) {
		int end = editor->FindText(start + 2, editor->GetLength(), _T("*/"));
		if (end == -1)
			slice.first.right = editor->GetLength();
		else if (end > slice.first.right)
			slice.first.right = end;
	}
}

//**********************************************************************

bool LanguageModule::afterOpeningBrace (char brace, int* position) const
{
	EditorWindow* editor = this->GetEditor();
	int pos = editor->GetCurrentPos() - 2;	//skip possibly newly added char
	if (pos < 0)
		pos = 0;
	while(pos && editor->GetCharAt(pos) != brace)
		--pos;
	if (!pos)
		return false;

	int matchingBrace = editor->BraceMatch(pos);
	bool ret = matchingBrace == -1 || matchingBrace > editor->GetCurrentPos();
	if (ret && position)
		*position = pos;
	return ret;
}

//**********************************************************************

bool LanguageModule::shouldIncreaseIndentAfterNewLine (int line) const
{
	static const String openingBraces = _T("{([");
	const String str = this->GetEditor()->GetLine(line).Trim();
	return	std::find(openingBraces.begin(), openingBraces.end(), str[str.size() - 1]) != 
				openingBraces.end();
}

//**********************************************************************

int LanguageModule::getPreviousOpeningBracketIndentation (char brace) const
{
	int pos;
	if (afterOpeningBrace(brace, &pos)) {
		EditorWindow* editor = this->GetEditor();
		return editor->GetLineIndentation(editor->LineFromPosition(pos));
	}
	else
		return -1;
}

//**********************************************************************

int LanguageModule::getMatchingOpeningBracketIndentation (int pos) const
{
	EditorWindow* editor = this->GetEditor();
	int matchingBrace = editor->BraceMatch(pos);
	return	matchingBrace == -1 ? -1 :
		editor->GetLineIndentation(editor->LineFromPosition(matchingBrace));
}

//**********************************************************************

void LanguageModule::setIdentLevel (int c)
{
	EditorWindow* editor = this->GetEditor();

	int currLine = editor->LineFromPosition(editor->GetCurrentPos());
	int foldState = editor->GetFoldLevel(currLine);
	if (foldState & wxSCI_FOLDLEVELHEADERFLAG)
		return;

	int tabWidth = editor->GetTabWidth();
	int prevFoldState = editor->GetFoldLevel(currLine - 1);
	int level = editor->GetLineIndentation(currLine - 1);

	if (prevFoldState & wxSCI_FOLDLEVELHEADERFLAG && c == '\n')
		level += tabWidth;
	else if (c == '{') {
		int squareBracePos, curlyBracePos;
		bool afterSquareBrace = afterOpeningBrace('[', &squareBracePos);
		bool afterCurlyBrace = afterOpeningBrace('{', &curlyBracePos);

		if (afterSquareBrace && (!afterCurlyBrace || squareBracePos > curlyBracePos) ||	//Table construction
			afterCurlyBrace && editor->LineFromPosition(curlyBracePos) == currLine
		)
			return;
		int previous = getPreviousOpeningBracketIndentation('{');
		if (previous != -1)
			level = previous + tabWidth;
	}
	else if (c == '}' || c == ')' || c ==']')
	{
		int match = getMatchingOpeningBracketIndentation(editor->GetCurrentPos() - 1);
		if (match != -1)
			level = match;
		else
			level -= tabWidth;
	}
	else if (c == '\n' && shouldIncreaseIndentAfterNewLine(currLine - 1))
		level += tabWidth;

	editor->SetLineIndentation(currLine, level);

	if (c == '\n') {
		int identPos = editor->GetLineIndentPosition(currLine);
		editor->GotoPos(identPos);
		static const std::string braces(")]}");
		if (std::find(braces.begin(), braces.end(), editor->GetCharAt(identPos)) != braces.end())
		{
			int match = getMatchingOpeningBracketIndentation(identPos);
			if (match != -1)
				level = match;
			else
				level -= tabWidth;
			editor->SetLineIndentation(currLine, level);
			editor->GotoPos(editor->GetLineIndentPosition(currLine));
		}
	}
}

//**********************************************************************

void LanguageModule::completeSymbol (long flags, int endPos, int startPos)
{
	EditorWindow* editor = this->GetEditor();
	int atPos = editor->GetCurrentPos();
	String hint;

	if (startPos != -1)
		hint = editor->GetTextRange(startPos, endPos);

	DBGOUT << "--> AUTOCOMPLETE: \"" << hint << "\"" << DBGENDL;

	if (DeltaASTNode* node = m_progDesc.GetNodeAbove(atPos)) {
		DeltaASTSymbolFinder finder(editor, hint, flags);
		node->AcceptBack(&finder);
		const String& completionList = finder.GetCompletionList();
		if (!completionList.empty())
			this->GetEditor()->AutoCompShow((int) hint.size(), completionList);
	}
}

//**********************************************************************

void LanguageModule::completeSymbol (int endPos)
{
	EditorWindow* editor = this->GetEditor();
	long flags = 0;

	int startPos = -1;
	int c = editor->GetCharAt(editor->PositionBefore(endPos));
	if (DeltaASTSymbolFinder::IsIdChar(c)) {
		startPos = editor->WordStartPosition(endPos, true);
		c = editor->GetCharAt(editor->PositionBefore(startPos));
	}
	
	switch (c) {
#if 0
		case '[':	flags = SYMBFL_VAR | SYMBFL_KEY;	break;
		case '.':	flags = SYMBFL_KEY | SYMBFL_ID;		break;
		case '@':	flags = SYMBFL_ATTR;				break;
#endif
		case '"':
			flags = SYMBFL_KEY;
			startPos = startPos != -1 ? editor->PositionBefore(startPos) :
				editor->PositionBefore(endPos);
			break;
		default:	flags = SYMBFL_VAR | SYMBFL_FUNC;	break;
	}

	this->completeSymbol(flags, endPos, startPos);
}

//**********************************************************************

void LanguageModule::deleteText (int startPos, int endPos)
{
	int pos1, pos2;
	EditorWindow* editor = this->GetEditor();
	editor->GetSelection(&pos1, &pos2);
	editor->SetSelection(startPos, endPos);
	editor->ReplaceSelection(String());

	//check if previous selection need to be adjusted due to deletion
	if (startPos < pos1)
		pos1 -= std::min(endPos, pos1) - startPos;
	if (startPos < pos2)
		pos2 -= std::min(endPos, pos2) - startPos;
	editor->SetSelection(pos1, pos2);
}

//**********************************************************************

void LanguageModule::commentLine (int line, int startPos)
{
	EditorWindow* editor = this->GetEditor();
	if (!editor->GetLine(line).Trim().empty())	//don't comment empty lines
	{
		const std::string whitespaces(" \t");
		int pos = editor->PositionFromLine(line);
		if (startPos == -1)
			while (whitespaces.find(editor->GetCharAt(pos)) != std::string::npos)
				++pos;
		else
			pos += startPos;
		editor->InsertText(pos, _T("//"));
	}
}

//**********************************************************************

void LanguageModule::uncommentLine (int line)
{
	EditorWindow* editor = this->GetEditor();
	int pos = editor->PositionFromLine(line);
	const std::string whitespaces(" \t");
	while (whitespaces.find(editor->GetCharAt(pos)) != std::string::npos)
		++pos;
	if (editor->GetTextRange(pos, pos + 2) == _T("//"))
		deleteText(pos, pos + 2);
}

//**********************************************************************

void LanguageModule::commentSelection (void)
{
	EditorWindow* editor = this->GetEditor();
	editor->Freeze();
	editor->BeginUndoAction();

	int startPos, endPos;
	editor->GetSelection(&startPos, &endPos);
	if (startPos == endPos)	//no selection
		commentLine(editor->GetCurrentLine());
	else
	{
		bool singleEolChar = editor->GetEOLMode() != 0;	//SC_EOL_CRLF
		bool selectionContainsNextLine = false;
		if (editor->PositionFromLine(editor->LineFromPosition(endPos)) == endPos)
			selectionContainsNextLine = true;

		int startLine = editor->LineFromPosition(startPos);
		int endLine = editor->LineFromPosition(endPos);
		if (selectionContainsNextLine)
			--endLine;
		int startLinePos = editor->PositionFromLine(startLine);
		int endLinePos = editor->PositionFromLine(endLine) + editor->LineLength(endLine) -
			(selectionContainsNextLine ? 0 : (singleEolChar ? 1 : 2));
		if (startLinePos == startPos && endLinePos == endPos)	//full row selection so use '//' comments
		{
			int min = -1;
			//try to determine the min column for placing the '//' comments
			for(int i = startLine; i <= endLine; ++i)
			{
				if (!editor->GetLine(i).Trim().empty())	//don't check empty lines
				{
					int pos = editor->PositionFromLine(i);
					int counter = 0;
					const std::string whitespaces(" \t");
					while (whitespaces.find(editor->GetCharAt(pos + counter)) != std::string::npos)
						++counter;
					if (counter < min || min == -1)
						min = counter;
				}
			}

			for(int i = startLine; i <= endLine; ++i)
				commentLine(i, min);
			editor->SetSelection(editor->PositionFromLine(startLine), editor->PositionFromLine(endLine + 1));
		}
		else	//partial row selection so use '/**/' commens
		{
			editor->InsertText(endPos, _T("*/"));	//first insert ending comment so that endPos is unchanged
			editor->InsertText(startPos, _T("/*"));
			editor->SetSelection(startPos, endPos + 4);
		}
	}

	editor->EndUndoAction();
	editor->Thaw();
}

//**********************************************************************

void LanguageModule::uncommentSelection (void)
{
	EditorWindow* editor = this->GetEditor();
	editor->Freeze();
	editor->BeginUndoAction();

	int startPos, endPos;
	editor->GetSelection(&startPos, &endPos);
	if (startPos == endPos)	//no selection
		uncommentLine(editor->GetCurrentLine());
	else if (
		editor->GetTextRange(startPos, startPos + 2) == _T("/*") &&
		editor->GetTextRange(endPos - 2, endPos) == _T("*/")
	)
	{
		deleteText(endPos - 2, endPos);	//first delete ending comment so that endPos is unchanged
		deleteText(startPos, startPos + 2);
	}
	else
	{
		int startLine = editor->LineFromPosition(startPos);
		int endLine = editor->LineFromPosition(endPos - 1);
		for(int i = startLine; i <= endLine; ++i)
			uncommentLine(i);
	}

	editor->EndUndoAction();
	editor->Thaw();
}

//**********************************************************************

static DeltaASTNode *createLibfuncNode(const Range& range, bool std)
{
	if (std)
		return new StdLibfuncASTNode(range);
	else
		return new LibfuncASTNode(range);
}

void LanguageModule::patchBinaryQualifiedId (BinaryOpASTNode* node, bool std)
{
	assert(!strcmp(node->GetValue(), DELTA_LIBRARYNAMESPACE_SEPARATOR));
	while (node->GetRightChild()->GetType() == BinaryOpASTNode::Type)
		node = static_cast<BinaryOpASTNode*>(node->GetRightChild());
	DeltaASTNode *leaf = node->GetRightChild();
	assert(
		leaf->GetType() == VariableASTNode::Type	||
		leaf->GetType() == LibfuncASTNode::Type		||
		leaf->GetType() == StdLibfuncASTNode::Type
	);
	if (leaf->GetType() == VariableASTNode::Type)
		m_progDesc.ReplaceNode(leaf, createLibfuncNode(leaf->GetRange(), std));
}

bool LanguageModule::checkQualifiedFunction (DeltaASTNode* node, bool *std, bool reportError)
{
	const Range& range = node->GetRange();
	const String path = GetEditor()->GetTextRange(range.left, range.right);

	if (path.find_first_of(_T("~!")) != String::npos)	//TODO: how to resolve generated namespaces?
		return false;

	std::string error;
	StdStringList pathList = Namespace::CreateNamespacePathList(util::str2std(path));
	if (pathList.size() == 2 && DeltaAutoCompletion::HasByteCodeLibraryFunction(this, pathList.front(), pathList.back()))
		return true;
	else
	if (Namespace::HasFunction(range.left, pathList, std, &error))
		return true;
	else {
		if (reportError && !error.empty())
			m_progDesc.AddSemanticError(range, util::std2str(error));
		return false;
	}
}

void LanguageModule::checkNamespaces (DeltaASTNode* node)
{
	DeltaASTNodeList children;
	if (node && node->GetChildren(children))
		BOOST_FOREACH(DeltaASTNode *child, children)
		{
			switch(child->GetType()) {

				case UsingASTNode::Type: {
					UsingASTNode* node = static_cast<UsingASTNode*>(child);
					if (node->GetChild<0>() && !node->GetChild<1>()) {	// it is 'using <namespace>' form
						const Range& range	= node->GetChild<0>()->GetRange();
						const String path	= GetEditor()->GetTextRange(range.left, range.right);
						if (path.find_first_of(_T("~!")) != String::npos)	//TODO: how to resolve generated namespaces?
							break;

						std::string error;
						if (!Namespace::Open(node->GetRange().left, Namespace::CreateNamespacePathList(util::str2std(path)), &error))
							m_progDesc.AddSemanticError(range, util::std2str(error));
					}
					break;
				}

				case UnaryKwdASTNode::Type: checkNamespaces(child); break;

				case BinaryOpASTNode::Type:
					if (!strcmp(static_cast<BinaryOpASTNode*>(child)->GetValue(), DELTA_LIBRARYNAMESPACE_SEPARATOR))
					{
						bool std = false;
						if (checkQualifiedFunction(child, &std))
							patchBinaryQualifiedId(static_cast<BinaryOpASTNode*>(child), std);
					}
					else
						checkNamespaces(child);
					break;

				case UnaryOpASTNode::Type:
					if (!strcmp(static_cast<UnaryOpASTNode*>(child)->GetValue(), DELTA_LIBRARYNAMESPACE_SEPARATOR))
					{
						UnaryOpASTNode* op = static_cast<UnaryOpASTNode*>(child);
						bool std = false;
						switch(op->GetChild()->GetType()) {
							case BinaryOpASTNode::Type:
								if (checkQualifiedFunction(op, &std))
									patchBinaryQualifiedId(static_cast<BinaryOpASTNode*>(op->GetChild()), std);
								break;
							case VariableASTNode::Type:		//FALLTHROUGH
							case UnaryOpASTNode::Type:
								if (checkQualifiedFunction(op, &std, false))
									m_progDesc.ReplaceNode(op->GetChild(), createLibfuncNode(op->GetChild()->GetRange(), std));
								break;
							case LibfuncASTNode::Type:		//FALLTHROUGH
							case StdLibfuncASTNode::Type:
								if (!checkQualifiedFunction(op, (bool *) 0, false))
									m_progDesc.ReplaceNode(op->GetChild(), new VariableASTNode(op->GetChild()->GetRange()));
								break;
							default:
								assert(false);
						}
					}
					else
						checkNamespaces(child);
					break;

				case VariableASTNode::Type:
					{
						bool std = false;
						if (checkQualifiedFunction(child, &std, false))
							m_progDesc.ReplaceNode(child, createLibfuncNode(child->GetRange(), std));
					}
					break;

				case LibfuncASTNode::Type:		//FALLTHROUGH
				case StdLibfuncASTNode::Type:
					if (!checkQualifiedFunction(child, (bool *) 0, false))
						m_progDesc.ReplaceNode(child, new VariableASTNode(child->GetRange()));
					break;
				default:	checkNamespaces(child); break;
			}
		}
}

////////////////////////////////////////////////////////////////////////

} // namespace delta

////////////////////////////////////////////////////////////////////////
// Exported Factory method
//
UTIL_C_EXPORT void* CreateLanguageModule (void) { return new delta::LanguageModule; }

////////////////////////////////////////////////////////////////////////
// DLL entry point
//
BOOL APIENTRY DllMain (HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			delta::LanguageModule::Initialize();
			return true;
		case DLL_PROCESS_DETACH:
			delta::LanguageModule::CleanUp();
			return true;
	}
	return true;
}
