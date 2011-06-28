/////////////////////////////////////////////////////////////////////////////
// Interactive web document generation from content defintions.
// Diploma thesis supervisor:	Anthony Savidis
// Student:						Konstantinos Mousikos
// Delivery date:				03/11/2009
// Refactoring, extension (syntax highlighting), 
// and maintence:				Anthony Savidis	
//

// FIXME: add style for indexing terms inside the href to make it apply.

using std;
const nl 					= "\n";
const cpp					= "K:\\bin\\cpp.exe";
const DELTA					= "delta";
const CPP					= "cpp";
const DOC					= "doc";
const TAB 					= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
const INDEXINGTERMS_DOCID	= "i_indexingterms";
const INDEXINGTERMS_SUFFIX	= "IndexingTerm";
const MENU_BUTTONS_IE7_SIZE	= 34;
const MENU_BUTTONS_IE8_SIZE	= 32;
const ANCHOR_TYPE_DOCUMENT	= 1;
const JQUERY_URL			= "http://ajax.googleapis.com/ajax/libs/jquery/1.2.6/jquery.min.js";
const VIDEO_WIDTH			= 425;
const VIDEO_HEIGHT			= 344;

// Global variables.
style			= nil,
toc				= nil,
html			= nil,
css				= nil,
data			= nil,
stylesheet		= nil,
sectionCounter	= nil,
config 			= nil,
indexingTerms	= nil,
document		= nil,
write			= nil,
mainmenu		= nil;

function Styles 
		{ return style; }

/////////////////////////////////////////////////////////////
// A heuristic to create a large FSM-based lexical analyser
// for keyword tokens.

function KwdLexicalAnalyser (kwds) {

	const START = 0;
	
	local la = [

		@final		: [],	// [state]->kwd
		@tran		: [],	// [state][c]->state
		@num_states	: 0,	// how many states we have

		method produce_fsm (kwds) {	// FSM production
			tabdisableoverloading(@tran[START] = []);
			foreach (kwd, kwds) {
				for (local state = START, local n = strlen(kwd), local i = 0; n; --n, ++i) {
					local t = @tran[state];
					assert t;				
					if (not (local target = t[local c = strchar(kwd, i)])) {
						t[c] = state = ++@num_states;						// New edge.
						if (not @tran[state])								// New state.
							tabdisableoverloading(@tran[state] = []);	
					} 
					else
						state = target;										// Follow edge to existing stage.
				}
				assert @tran[state];
				@final[state] = kwd;
			}
		},
		
		method @operator()(text) {	// Lexical analysis.
			local tokens	= list_new();
			local raw_text	= "";
			for (local state = START, local n = strlen(text), local i = 0, local start = 0; i < n; ++i) {
				assert @tran[state];
				if (local j = @tran[state][local c = strchar(text, i)]) 	// Transition exists.
					state = j;												// Take the transition.
				else														// No transition.
				if (local kwd = @final[state]) {							// Is it a final state?
					if (raw_text != "")										// Add remaining raw text.
						tokens.push_back(raw_text), raw_text = "";
					assert state != START; 
					start = i--, tokens.push_back(kwd), state = START; 		// Add kwd, restart the fsm.
				}
				else														// Hang state.
				if (state == START)											// Failed from start state.
					raw_text += c, ++start;									// Add char as raw text and proceed.
				else {														// Need to restart from next char.
					raw_text += strchar(text, start);						// The first char is added as raw text only.
					i = start++;											// Input starts from the next char.
					state = START;											// Restart fsm.
				}
			}
			
			if (raw_text != "")
				tokens.push_back(raw_text);									// Add remaining unrecognized text.
			if (state != START)	
				tokens.push_back(((local kwd = @final[state]) ? kwd : strslice(text, start, 0)));			
			return tokens;
		}
	];
	
	la.produce_fsm(kwds);
	return la;
};

/////////////////////////////////////////////////////////////

function LexicalAnalyser {
	
	const IDENT			= #d_ident;
	const ATTRIBUTE		= #d_attr;
	const KWD			= #d_kwd;
	const TO_STRING		= #d_tostring;
	const NUMBER		= #d_number;
	const STRING_CONST	= #d_stringconst;
	const STRINGIFY		= #d_stringify;
	const OTHER			= #d_other;
	const WHITE_SPACE	= #d_ws;
	const FUNC_NAME		= #d_funcname;
	const FORMALS		= #d_formals;
	const LIBFUNC		= #d_libfunc;
	const COMMENT		= #d_comment;
	const BINARY_OP		= #d_binaryop;
	const OBJECT_OP		= #d_objectop;
	const OVERLOADED_OP	= #d_overloadedop;
	const PUNCTUATION	= #d_punctuation;
	
	const TAB_SPACES	= 4;
	
	if (not static la)
		
		la = [
			@class 		: #LexicalAnalyser,
			@pos		: 0,
			@size		: 0,
			@tokens		: list_new(),
			@text		: "",
			@raw_text	: "",
			@ignore 	: [ { OTHER, WHITE_SPACE : true } ],
			
			//************************************
			// Scanner utilities.

			method set_input (text) {
				@text 				= text;
				@raw_text			= "";
				@pos 				= 0;
				@size 				= strlen(text);
				@line_chars			= 0;
				@extra_spaces		= 0;
				@check_func_name 	=
				@check_func_args 	= false;
				@tokens.clear();
			},

			method input {
				assert @pos < @size; 
				if ((local c = strchar(@text, @pos++)) == "\n")
					@line_chars = @extra_spaces = 0;
				else
					++@line_chars;
				return c;
			},
						
			method retract			
				{ assert @pos; --@pos; if (@line_chars) --@line_chars; },
			
			method eof				
				{ return @pos >= @size; },

			method add_token_unprocessed (t, tag)
				{ @tokens.push_back(taggedvalue(t, tag)); },

			method add_token (t) {
				@add_any_raw_text();
				if (not @syntax_handling(t))
					@tokens.push_back(t);
			}, 
			
			@operators : [
				{ BINARY_OP		:	list_new(
										">", ">=", "<", "<=", "==", "!=", 
										"+", "+=", "-", "-=", "*",	"*=", 
										"/", "/=", "%", "%=", "=" 
									) },
								
				{ OBJECT_OP		:	list_new(".", "..", "...", "[", "[[", "]", "]]") },
				
				{ PUNCTUATION	:	list_new("::", "{", "}", ";", ",", ":", "(", ")") },
				
				{ OVERLOADED_OP	:	list_new(
										">_", ">=_", "<_", "<=_", "==_", "!=_",
										"+_", "-_", "*_", "/_", "%_", "=()"
									) }
			],
					
			method get_operators {
				if (isundefined(static l))
					for (	local t = tabindices(@operators), local n = tablength(t), local i = 0, l = list_new(); 
							n; 
							++i, --n	)
							foreach (local token, @operators[ t[i] ])
								l.push_back(taggedvalue(token, t[i]));
				return l;
			},
						
			@kwd_analyser : KwdLexicalAnalyser(@self.get_operators()),
			
			method add_any_raw_text	{
				if (@raw_text != "") {
					local t = @raw_text, @raw_text = ""; 
					foreach (t, @kwd_analyser(t)) {
						assert t != "";
						@add_token(taggedvalue(t,  ((local tag = getvaluetag(t)) == "" ? OTHER : tag)));
					}
				}
			},
						
			function is_delta_kwd (s) {
				if (isundefined(static keywords))
					keywords = [
						{ 	"if", 		"else", 		"while", 		"for", 
							"foreach", 	"break", 		"continue", 	"function",
							"method", 	"using", 		"try", 			"trap", 
							"throw", 	"static", 		"local", 		"const",
							"lambda", 	"@lambda", 		"@set", 		"@get", 
							"self", 	"@self", 		"@operator", 	"return", 
							"assert",	"onevent", 		"arguments",	"nil",
							"and", 		"not", 			"or", 			"true", 
							"false" 	: true 
						}
					];
				return keywords[s];
			},

			function is_cpp_kwd (s) {
				if (isundefined(static keywords))
					keywords = [
						{ 	"auto",   		"const",     	"double",  		"float",  			"int",      
							"break",  		"continue",  	"else",    		"for",    			"long",     
							"case",   		"default",  	"enum",    		"goto",   			"register", 
							"char",   		"do",       	"extern",  		"if",     			"return", 
							"asm",         	"dynamic_cast", "namespace",  	"reinterpret_cast", "try",
							"bool",        	"explicit",     "new",        	"static_cast",      "typeid",
							"catch",       	"false",        "operator",   	"template",         "typename",
							"class",       	"friend",       "private",    	"this",             "using",
							"const_cast",  	"inline",       "public",     	"throw",            "virtual",
							"short",   		"struct",		"unsigned", 	"static",  		 	"union",   
							"while",		"signed",  		"switch",   	"void",
							"sizeof",  		"typedef",  	"volatile" 		: true 
						}
					];
				return keywords[s];
			},

			method set_kwds (lang) 
				{ @is_kwd = (lang == CPP? is_cpp_kwd : (lang == DELTA? is_delta_kwd : (function{return nil;}) ) ); },

			function is_libfunc (s) { 
				return 	s == "std"  or
						s == "libs" or
						libfuncget("std::" + s)  or
						libfuncget("std::libs::" + s);
			},
			
			/////////////////////////////////////////////////////////////
			// Syntax analysis (restricted) heuristics.
			
			method syntax_handling (t) {
				if ((local tag = getvaluetag(t)) == KWD and t == "function" or t == "method")
					@check_func_name = true;
				else
				if (t == "lambda")
					{ @check_func_name = false; @start_fargs(); }
				else
				if (tag != WHITE_SPACE)
					if (@check_func_name)
						{ @handle_func_name(tag, t); return true; }
					else 
					if (@check_func_args)
						{ @handle_fargs(tag, t); return true; }
				return false;	// Request add token.
			},
			
			method handle_func_name (tag, t) {	// Function name.
				@check_func_name = false;
				@start_fargs(); 
				if (tag == IDENT)
					@add_token_unprocessed(t, FUNC_NAME); 
				else
					@handle_fargs(tag, t);
			},
			
			method fargs_next_state (tag, t, f) {
				@fargs_state = f; 
				++@fargs_n;
				@add_token_unprocessed(t, tag);
			},
			
			method fargs_wait_lp (tag, t) {
				if (t == "(")
					{ @fargs_state = @fargs_wait_id; @fargs_n = 1; }
				else
					@check_func_args = false;
				@add_token_unprocessed(t, tag);	
			},

			method fargs_wait_id (tag, t) {
				if (tag == IDENT)
					@fargs_next_state(tag, t, @fargs_wait_comma_or_rpar);
				else
					@fargs_wait_rpar_or_triple_dot(tag, t);
			},
			
			method fargs_wait_rpar_only (tag, t) {
				if (not @fargs_rpar_handler(tag, t)) {
					@check_func_args = false;
					@add_token_unprocessed(t, tag);
				}
			},
			
			method fargs_rpar_handler (tag, t) {
				if (strchar(t, 0) == ")") {
					@fargs_recognised();
					if (strlen(t) > 1 and (t = strslice(t, 1, 0)) != "")	// was only rpar
						@add_token_unprocessed(t, tag);
					return true;
				}
				else
					return false;
			},
			
			method fargs_triple_dot_handler (tag, t) {
				if ((local i = strsub(t, "...")) == 0) {
					@add_token_unprocessed("...", OTHER);
					++@fargs_n;
					@fargs_state = @fargs_wait_rpar_only;
					if (strlen(t) > 3 and (t = strslice(t, 3, 0)) != "")	// was only ...
						@add_token(t, OTHER);	// we actually split in two tokens.
					return true;	
				}
				else
					return false;
			},
			
			method fargs_wait_rpar_or_triple_dot (tag, t) {
				if (not (@fargs_rpar_handler(tag, t) or @fargs_triple_dot_handler(tag, t))) {
					@check_func_args = false;
					@add_token_unprocessed(t, tag);
				}
			},
			
			method fargs_wait_comma_or_rpar (tag, t) {
				if (t == ",")
					@fargs_next_state(tag, t, @fargs_wait_id);
				else
					@fargs_wait_rpar_or_triple_dot(tag, t);
			},

			method fargs_recognised {
				for (local args = ")"; @fargs_n; @tokens.pop_back()) {
					if (getvaluetag(local t = @tokens.back()) != WHITE_SPACE)
						--@fargs_n;
					args = t + args;
				}
				@check_func_args = false;
				@add_token_unprocessed(args, FORMALS);
			},

			method start_fargs 
				{ @check_func_args = true; @fargs_state = @fargs_wait_lp; },

			method handle_fargs(tag, t) // Formal arguments.
				{ @fargs_state(tag, t); },
	
			/////////////////////////////////////////////////////////////
			// Lexical analysis.

			method try_scan_ident_p {
				if (@eof()) 
					return nil;
				else if (strisalpha(local c  = @input()))
					return @scan_ident_p(c);
				else
					{ @retract(); return nil; }
			},
			
			method try_scan_dotted_ident_p {
				local str = "";
				while (true)
					if (@eof()) 
						return str;
					else if ((local c = @input()) == "." and local id = @try_scan_ident_p(c))
						str += "." + id;
					else
						{ @retract(); return str; }
			},
			
			method scan_stringify_ident {
				if (local id = @try_scan_ident_p()) {
					id += @try_scan_dotted_ident_p();
					@add_token(taggedvalue("#" + id, STRINGIFY));
				}
				else
					@add_token(taggedvalue("#", OTHER));
			},
			
			method scan_ident_p (c) {
				assert strisalpha(c);
				local id = c;
				while (not @eof())
					if (strisalnum(c = @input()) or c == "_")
						id += c;
					else 
						{ @retract(); break; }					
				return id;
			},
			
			method scan_ident_kwd (c) {
				local id = @scan_ident_p(c);
				@add_token(taggedvalue(id, (@is_kwd(id) ? KWD : (is_libfunc(id) ? LIBFUNC : IDENT))));
			},
			
			//************************************

			method try_scan_fractional_p {
				if (@eof())
					return nil;
				if ((local c = @input()) == ".")
					if (strisdigit(c = @input()))
						return "." + @scan_decimal(c);
					else
						@retract();
				@retract(); 
				return nil;
			},
			
			method scan_decimal_p (c) {
				for (local dec = c; not @eof() and strisdigit(c = @input()); dec += c)
					;
				@retract();
				return dec;
			},
			
			method scan_number (c) {
				local dec = @scan_decimal_p(c);
				if (local fract = @try_scan_fractional_p())
					dec += fract;
				@add_token(taggedvalue(dec, NUMBER)); 
			},
			
			//************************************

			method scan_attr_kwd_tostring {
				if (local id = @try_scan_ident_p()) 
					if (@is_kwd(id = "@" + id))
						@add_token(taggedvalue(id, KWD));
					else
						@add_token(taggedvalue(id, ATTRIBUTE));
				else
					@add_token(taggedvalue("@", TO_STRING));
			},
			
			//************************************
			
			method scan_quoted_string {
				++@extra_spaces;			// Was \" in the source
				local b_slashes = 0, local str = "\"";
				while (not @eof()) {
					local c = @input();
					if (c == "\\") {		// Was \\ in the source
						++@extra_spaces;
						assert b_slashes == 0 or b_slashes == 1;
						if (b_slashes == 1) 
							{ str += "\\\\";  b_slashes = 0; }
						else 
							b_slashes = 1;
					}
					else
					if (c == "\"") {
						++@extra_spaces;	// Was \" in the source
						if (b_slashes == 0)
							{ str += "\""; @add_token(taggedvalue(str, STRING_CONST)); return; }
						else
							{ str += "\\\""; b_slashes = 0; }
					}
					else {
						if (b_slashes == 1)
							{ str += "\\"; b_slashes = 0; }
						str += c;
					}
				}
				@add_token(taggedvalue(str, OTHER));	// Unrecognized text.
			},

			//************************************
			
			method extend_tab_spaces {
				if (not static t)
					t = [ "    ", "   ", "  ", " " ];
				local n = (--@line_chars + @extra_spaces) % TAB_SPACES;	// Allign to tab stops.
				local s = t[n];
				@line_chars += (TAB_SPACES - n) + @extra_spaces;
				while (@extra_spaces)
					{ s += " "; --@extra_spaces; }
				return s;
			},

			method scan_white_space(c) {
				if (c == "\t")
					c = @extend_tab_spaces();
				for (local ws = c; not @eof(); ws += c)
					if (not strisspace(c = @input()))
						{ @retract(); break; }
					else if (c == "\t")
						c = @extend_tab_spaces();	// Conversion of tabs to spaces directly.
				@add_token(taggedvalue(ws, WHITE_SPACE));
			},

			//************************************
			
			method scan_till_eol {
				local t = ""; 
				while (not @eof())
					if ((local c = @input()) != "\n") 
						t += c;
					else 
						{ @retract(); break; }
				return t;
			},
			
			method try_scan_c_comment {
				local t = "/*";
				while (not @eof()) {
					if ((local c = @input()) == "*")
						if (@eof())
							break;
						else if ((c = @input()) == "/")
							{ @add_token(taggedvalue(t + "*/", COMMENT)); return; }
						else
							{ t += "*"; @retract(); }
					else
						t += c;
				}	
				@raw_text += t;
			},
			
			method scan_comment {
				if (not @eof()) {
					if ((local c = @input()) == "/") {
						c = "//" + @scan_till_eol();
						@add_token(taggedvalue(c, COMMENT));
					}
					else if (c == "*")
						@try_scan_c_comment();
					else
						@raw_text += "/" + c;							
				}
			},
			
			//************************************

			method unparse (tokens) {
				local str = "";
				foreach (local token, tokens)
					str += taggedvalue(token, "");
				return str;
			},
			
			//************************************

			method @operator()(text, lang) {
				@set_input(text);
				@set_kwds(lang);
				while (not @eof()) {
					local pos = @pos;
					if (strisalpha(local c = @input()))
						@scan_ident_kwd(c);
					else if (strisdigit(c))
						@scan_number(c);
					else if (c == "@")
						@scan_attr_kwd_tostring();
					else if (c == "#")
						@scan_stringify_ident();
					else if (c == "\"")
						@scan_quoted_string();
					else if (c == "/")
						@scan_comment();
					else if (strisspace(c))
						@scan_white_space(c);
					else
						@raw_text += c;
					assert pos != @pos;	// Can't stay at the same point (check for infinite loop).
				}
				@add_any_raw_text();
				return @tokens;
			}		
		];
		
	return la;
}

/////////////////////////////////////////////////////////////

function ExtendSpecialHTMLCharacters (text) {		
	if (not static map) 
		map = [
			{ "&" 	: "&amp;" 		},
			{ "|" 	: "&brvbar;" 	},
			{ ">" 	: "&gt;"		},
			{ "<" 	: "&lt;"		},
			{ "\"" 	: "&quot;" 		},
			{ ""	: "&apos;"		}
		];
	for (local result = "", local n = strlen(text), local i = 0; n; ++i, --n)
		if (local c2 = map[local c1 = strchar(text, i)])
			result += c2;
		else
			result += c1;
	return result;
}

/////////////////////////////////////////////////////////////

function SyntaxHighlighter {
	if (isundefined(static sh))
		sh = [		
			method @operator()(text, lang) {
				code = "";
				text = config.code_prefix + text + config.code_suffix;
				foreach (local t, (local la = LexicalAnalyser())(text, lang)) {
					local content = ExtendSpecialHTMLCharacters(t);
					local style = getvaluetag(t);
					assert style != "";
					if (la.ignore[style])
						code += content;
					else
						code += "<span class = \"" + style + "\">" + content + "</span>";
				}
				return code;
			}
		];
	return sh;
}

/////////////////////////////////////////////////////////////
// The 'content' arg concerns the 'TextContent' grammar rule 
// and can be of type String or Object.
//
function ExtractTextContent (content) {
	
	function Extract (content, val) {
		if ((local ts = typeof(content)) == "String") {						// The actual text content.
			assert not val.text; 											// Can be set only once.								
			if (val.is_code)												// If code style.
				val.text = SyntaxHighlighter()(content, val.lang);		// Perform syntax highlighting (extends HTML inside too).
			else
				val.text = ExtendSpecialHTMLCharacters(content);			// Extend special HTML characters
		}
		else
		if (ts != "Table") 
			throw "Unexpected text content type " + ts;
		else
		if ((local n = tablength(local ti = tabindices(content))) != 1)	// Styled text is singleton.
			throw "Unexpected number of elements " + n + " in styled text content";
		else
		if (typeof(ti[0]) == "Number") {								// Means separator [ "<directive>" ].
			assert ti[0] == 0;
			if (val.styles)
				throw "Can't combine styles and directives together";
			assert not val.separator;
			val.separator = content[0];									// Store the separator.		
		}
		else {
			assert typeof(ti[0]) == "String";
			if (not val.styles)											// First style added?
				val.styles = list_new();
			val.styles.push_back(ti[0]);								// Accumulate style.
			if (ti[0] == config.delta_style_name)						// If it is code we mark it.
				{ val.is_code = true; val.lang = DELTA; }
			else
			if (ti[0] == config.cpp_style_name)
				{ val.is_code = true; val.lang = CPP; }
			else
			if (ti[0] == config.code_doc_style_name)
				{ val.is_code = true; val.lang = DOC; }
			try  Extract(content[ ti[0] ], val);						// Recursively extract content.
			trap e { throw e; }
		}
	}
	
	try  	{ Extract(content, local val = []); return val; }
	trap e 	{ print(e); return nil; }
}

/////////////////////////////////////////////////////////////

function File {	//Holds the html elements that constitute the page.
	return [
		@class 	: #File,
		@l 		: list_new(),
		method @operator()(s) 
				{ @l.push_back(s); },
		method flush (path) {
			local fp = fileopen(path, "wt");
			foreach (local x, @l)
				fp.write(x);
			fp.close();
		}
	];
}

/////////////////////////////////////////////////////////////

function TableOfContents {		// Holds the listings of the Table of Contents (and their indentation)
	
	function TOCNode (title, indent, document) {
		return [
			{ #class 	: #TOCNode 	},
			{ #title	: title		},
			{ #indent	: indent	},
			{ #document : document	}
		];
	}

	return [
		{ #class	: #TableOfContents	},
		{ #l 		: list_new() 		},
		{ #indent 	: 0 		 		},

		method @operator() (title, document) 
			{ @l.push_back( TOCNode(title,@indent,document) ); },
		
		method incr	
			{ ++@indent; },
		
		method decr 
			{ --@indent; },
		
		method section (section, document) {
			local size = tablength(section);
			if (section.index)
				size -= 1;
			if (section.itemref)
				size -= 1;
			if (section.class)
				size -= 1;
			if (section.title) {
				@l.push_back(TOCNode(section.title, @indent, document.itemref) );
				size -= 2;
				@incr();
			}
			else
				size -= 1;
				
			for (local i = 0; i < size; ++i)
				if(section[i].type == "section")
					@section(section[i], document);
			if (section.title)
				@decr();
		},

		method calculate (documents) {
			for (local size = tablength(documents), local i = 0; i < size; ++i){
				local len = tablength(documents[i]) - 3;
				@l.push_back(TOCNode(documents[i].title,@indent,documents[i].itemref));
				@incr();
				if (documents[i].class)  
					len -= 1;
				if (documents[i].index)	
					len -= 1;
				for (local j = 0; j < len; ++j)
					if (documents[i][j].type == "section")
						@section(documents[i][j], documents[i]);
				@decr();
			}
		},

		method write { 
			local counter = 0;
			html("<div class=\"toc\" id=\"tableofcontents\">\n");
			
			while (@l.total() > 1) {
				
				local current 	= @l.pop_front();
				local next 		= @l.front();
				local dif 		= current.indent - next.indent;
				
				html("<div class=\"indent" + current.indent + "\" id=\"section" + counter + "\">\n\t");
				
				if (next.indent > current.indent)
					html(
						"<a href=\"javascript:expand(" + counter + ")\"
						onmouseover=\"status='Click to expand/collapse nested items';return true\"
						onmouseout=\"status='';return true\"><img alt=\"image\" name=\"widget" + counter + "\" src=\"images/plus.gif\" height=\"12\" width=\"12\" border=\"0\" >
						</a>&nbsp;
						<a href=\"#anchor" + counter + "\" onclick =\"javascript:set_visible_document('"+current.document+"')\">"+current.title+"</a><br>\n"
					);
				else {
					html(
						"<a href=\"javascript:void(0)\"
						onmouseover=\"status='No further items';return true\"
						onmouseout=\"status='';return true\"><img alt=\"image\" src=\"images/end.gif\" height=\"12\" width=\"12\" border=\"0\" >
						</a>&nbsp;
						<a href=\"#anchor" + counter + "\" onclick =\"javascript:set_visible_document('" + current.document + "')\">" + current.title + "</a><br>\n"
					);
					while (dif-- >= 0)
						html("</div>\n");
				}
				++counter;
			}
			
			local current = @l.pop_front();
			html(
				"<div class=\"indent" + current.indent + "\" id=\"section" + counter + "\">\n\t
				<a href=\"javascript:void(0)\"
				onmouseover=\"status='No further items';return true\"
				onmouseout=\"status='';return true\"><img alt=\"image\" src=\"images/end.gif\" height=\"12\" width=\"12\" border=\"0\" >
				</a>&nbsp;
				<a href=\"#anchor" + counter + "\" onclick =\"javascript:set_visible_document('" + current.document + "')\">" + current.title + "</a><br>\n</div>\n"
			);
			
			for (local i = current.indent; i; --i)
				html("</div>\n");
			html("</div><!--tableofcontents-->\n");
		},

		method print (documents) 
			{ @calculate(documents); @write(); }
	];
}

/////////////////////////////////////////////////////////////
			
function Writer {	// Prints basic elements of the html document
	return [
		method initialize (script, style) {		// Adds the head tag of the html document. MUST be called before adding anything else to the Webpage.
			html(
				"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"
				\"http://www.w3.org/TR/html4/strict.dtd\">
				<html>\n<head>\n<title>Delta Programming Language</title>\n
				<link rel='stylesheet' href='" + style + "' type='text/css'>
				<script type=\"text/javascript\" src=\"" + JQUERY_URL + "\">
				</script>\n<script src='" + script + "'></script>\n"
				"</head>\n<body onload=\"javascript:load()\">\n"
			);
		},

		method finalize (output, menu) {		// Closes the main tags of the document and writes the result to file.
			if(menu.direction == "vertical")
				html("<div id=\"verticalMenu\"></div>\n");
			html("</body>\n</html>");
			html.flush(output);
		},

		method header (text, depth) 
			{ html("<h" + depth + ">" + text + "</h" + depth+ ">\n"); },

		method class (text, depth, class) 
			{ html("<h" + depth + "><span class='" + class + "'>" + text + "</span></h" + depth + ">\n"); },

		method anchor (text, type) {
			html("<a name=\"anchor" + sectionCounter + "\">");
			++sectionCounter;
			if(type == ANCHOR_TYPE_DOCUMENT)
				html(
					"<div class=\"documenttitle\"><h1><center>" + 
					text 										+ 
					"</center></h1></div>\n"
				);
			else
				self.header(text,type);
			html("</a>\n");
		},

		method logo (contentref, description, logo) {
			html(
				"<a href=\""				+
				contentref					+
				"\"><img alt=\""			+
				description					+
				"\" src=\""					+
				logo.path					+
				"\" border=\"0\" height=\""	+
				logo.height					+
				"\" width=\""				+
				logo.width					+
				"\"/></a>\n"
			);
		},

		method titlebar (maintitle, logo) {
			html("<div id='deltabar' class='titlebar'>\n\t<div class='logo'>\n\t");
			if (logo)
				self.logo("javascript:set_visible_document('homepage')", "Delta logo", logo);
			html(
				"\t</div>
				<!--logo-->\n
				<!--[if lt IE 8]><div class='titles'><![endif]-->"
			);
			if (maintitle) {
				if (maintitle.primary)
					if (maintitle.primaryClass)
						self.class(maintitle.primary,1,maintitle.primaryClass);
					else
						self.header(maintitle.primary,1);
				if (maintitle.secondary)
					if(maintitle.secondaryClass)
						self.class(maintitle.secondary,2,maintitle.secondaryClass);
					else
						self.header(maintitle.secondary,2);
			}
			html("<!--[if lt IE 8]>
				</div>
				<![endif]-->\n</div><!--deltabar-->\n<p></p>\n");
		},

		method searchbar {
			html(
				"<div id='searchbar' class='search'>
				<form method=get action=\"javascript:void(0)\" onsubmit=\"search(this); return false;\">
				<tr><td>
				<input type=text name=srchval value="">
				<input type=submit value=\"Search index\">
				</td></tr>
				</form>
				</div><!--searchbar-->\n"
			);
		}
	];
}

/////////////////////////////////////////////////////////////

function Menu {
	
	// Names of the following methods match content 
	// item tags, so they should not be renamed!

	return [
				
		method item (item) {
			if (item.url)
				html("<li><a href=\"" + item.url + "\">");
			else 
			if (item.documentref)
				html("<li><a href=\"javascript:set_visible_document('" + item.documentref + "')\">");
			else 
			if (item.sectionref)
				html("<li><a href=\"javascript:restorePage('#" + item.sectionref + "')\">");
			else
				html("<li><a href=\"#\">");
		},
			
		method simpleitem (item) {
			self.item(item);
			html(item.title + "</a></li>\n");
		},
			
		method imageitem (item) {
			self.item(item);
			html("<img src=\"" + item.path + "\" border=0 height=14 width=14></a></li>\n");
		},
			
		method header (menu){
			self.item(menu);
			html(menu.title + "</a>\n\t<ul>\n\t");
		},
			
		method footer (menu)
			{ html("\t</ul>\n</li>\n"); },
			
		method submenu (menu) {
			self.header(menu);
			for (local n = tablength(local menuitems = menu[0]), local i = 0; i < n; ++i)
				self[menuitems[i].type](menuitems[i]);
			self.footer(menu);
		},
			
		method print (menu, style) {
			
			for(local size = tablength(menu), local i = 0; i < size; ++i)
				self[(menu[i]).type](menu[i]);

			const NAVBUTTON_PREFIX 	= "<span class=\"navbutton\"><li><a href=\"javascript:";
			const NAVBUTTON_SUFFIX	= "<img src=\"";
			
			const BACK_ACTION		= "undo()\">" 							+ NAVBUTTON_SUFFIX;
			const HOME_ACTION		= "set_visible_document('homepage')\">" + NAVBUTTON_SUFFIX;
			const FORWARD_ACTION	= "redo()\">"							+ NAVBUTTON_SUFFIX;
			const PRINT_ACTION		= "printDocument()\">"					+ NAVBUTTON_SUFFIX;
			
			local back 				= NAVBUTTON_PREFIX + BACK_ACTION 	+ style.backButton;
			local home 				= NAVBUTTON_PREFIX + HOME_ACTION 	+ style.homeButton;
			local forward 			= NAVBUTTON_PREFIX + FORWARD_ACTION + style.forwButton;
			local printer 			= NAVBUTTON_PREFIX + PRINT_ACTION 	+ style.printButton;
						
			local IE7 	= "\" border=0 height=" + MENU_BUTTONS_IE7_SIZE + " width=" + MENU_BUTTONS_IE7_SIZE + "></a></li></span>\n";
			local IE8 	= "\" border=0 height=" + MENU_BUTTONS_IE8_SIZE + " width=" + MENU_BUTTONS_IE8_SIZE + "></a></li></span>\n";
							
			// Adds the navigation buttons to the menu, for Internet Explorer 6 and 7
			html(
				"<!--[if lt IE 8]>\n" 	+ 
				back 					+ 
				IE7 					+ 
				home 					+ 
				IE7 					+ 
				forward 				+ 
				IE7 					+ 
				printer 				+ 
				IE7 					+ 
				"<![endif]-->"
			);
						
			// Adds the navigation buttons to the menu, for Internet Explorer 8
			html(
				"<!--[if IE 8]>\n"		+
				back					+
				IE8						+
				home					+
				IE8						+
				forward					+
				IE8						+
				printer					+
				IE8						+
				"<![endif]-->"
			);
							
			// Adds the navigation buttons to the menu, for browsers other than IE
			html(
				"<comment>\n"			+
				back					+
				IE8						+
				home					+
				IE8						+
				forward					+
				IE8						+
				printer					+
				IE8						+
				"</comment>"
			);				
		}
	];
}

separators = [
	@paragraph 	: "<p></p>\n",
	@linebreak 	: "<br>",
	@section	: "<br>--------------------------------------------------------------------------------------<br>\n",
	@tab		: TAB
];

/////////////////////////////////////////////////////////////

function Map {
	return [
		@indices : [],
		method key_exists (key) 
			{ return self.indices[key]; },
			
		method value_exists (key, value) {
			assert self.key_exists(key);
			for (local n = tablength(local t = self.indices[key]), local i = 0; i < n; ++i)
				if (t[i] == value)	
					return true;
			return false;
		},
			
		method add (key, value, title, is_document) {
			local t = [ {#value, #title, #is_document : value, title, is_document } ];
			if (self.key_exists(key)) {
				if (not self.value_exists(key,value))
					self.indices[key][tablength(self.indices[key])] = t;
			}
			else
				self.indices[key] = [{ 0 : t }];
		},
									
		method sort_indices {
			for (local n = tablength(local t = tabindices(self.indices)), local i = 0; i < n; ++i)
				for (local j = n - 1; j > i; --j)
					if (strlower(t[j]) < strlower(t[j-1])) {
						local tmp	= t[j];
						t[j] 		= t[j-1];
						t[j-1] 		= tmp;
					}
			return t;
		},
			
		method print {

			local ti = self.sort_indices(self.indices);
			html("<div class=\"document\" id=\"" + INDEXINGTERMS_DOCID + "\">\n");

			for (local size = tablength(ti), local i = 0; i < size; ++i) {
				
				local item 			= ti[i];
				local item_lowcase 	= strlower(item);
				
				html(
					"<div class=\"index_terms\" id=\"" 			+ 
					item_lowcase + INDEXINGTERMS_SUFFIX			+
					"\"><a href=\"javascript:getReferences('"	+
					item_lowcase + INDEXINGTERMS_SUFFIX			+
					"')\">"										+
					item										+
					"</a>\n"
				);
				
				for (local values = tablength(self.indices[item]), local j = 0; j < values; ++j) {
					
					local ref 	= self.indices[item][j];
					local vis  	= (ref.is_document ? "set_visible_document('" : "restorePage('#");
						
					html(
						"\t<div class=\"index_locations\" id=\"" 					+ 
						ref.title													+
						"\"><a href=\"javascript:"									+
						vis															+
						ref.value													+	
						"')\">"														+
						ref.title + (ref.is_document ? " document" : " section")	+
						"</a></div>\n"
					);
				}
				html("</div><br>\n");
			}
			html("</div><!--" + INDEXINGTERMS_DOCID + "-->\n");
		}
	];
}
				
/////////////////////////////////////////////////////////////

function DocumentItem {  	// Provides methods to print SectionItems and SimpleItems
	return [
		method section (section, document) {
			local size = tablength(section);
			if (section.index){
				size -= 1;	// Add its indexing term.
				for (local terms = tablength(section.index), local j = 0; j < terms; ++j)
					indexingTerms.add(
						section.index[j], 
						section.itemref, 
						section.title,
						false
					);
			}
			if (section.class) 
				size -= 1;
			html("<div class=\"section\">");
			if (section.itemref) {
				html("<a name=\""+section.itemref+"\">");
				size -= 1;
			}
			
			if (section.title) {
				write.anchor(section.title, toc.indent + 2);
				size -= 2;
				toc.incr();
			}
			else
				size -= 1;
			
			for (local i = 0; i < size; ++i) {
				if (section[i].class) 
					html("<div class='" + section[i].class + "'>");
				self[section[i].type](section[i], document);
				if (section[i].class) 
					html("</div>");
			}					
			if (section.title)	
				toc.decr();
			html("</div>");
		},
			
		method image (image) {
			if (image.wrap)	
				html("<div class=\"wrapimage");
			else			
				html("<div class=\"image");
				
			if (image.align)	
				html(image.align + "\">\n");
			else			
				html("left\">\n");
			
			if(image.align == "center")	
				html("<center>\n");
			html(
				"<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" > 
				<tr><td width=\"100%\" class=\"tableborder\" style=\"border-bottom-style: none; border-bottom-width: medium\"> 
				<p align=\"center\" ><img src=\"" + image.path + "\" border=\"0\"></p></td></tr> 
				<tr><td width=\"100%\" class=\"tableborder\" style=\"border-top-style: none; border-top-width: medium\"> 
				<p align=\"center\" class=\"imagetext\">" + image.descr + "</td></tr> 
				</table>\n"
			);		
			if (image.align == "center")	
				html("</center>\n");
			html("</div>");
		},
			
		method video (video) {
			local embed 	= "";
			local length 	= strlen(video.url);
			local index	 	= strsub(video.url, "watch?v=");
			if(index != -1)
				embed =	strslice(video.url, 0, index-1) 		+ 
						"v/"									+
						strslice(video.url, index+8, length-1) 	+ 
						"&hl=en&fs=1"
						;
			else 
				embed = video.url;
			html(
				"<p><center><object width=\"" + VIDEO_WIDTH + "\" height=\"" + VIDEO_HEIGHT + "344\">
				<param name=\"movie\" value=\"" + embed + "\">
				</param><param name=\"allowFullScreen\" value=\"true\">
				</param><param name=\"allowscriptaccess\" value=\"always\">
				</param><embed src="+ embed + 
				"type=\"application/x-shockwave-flash\" allowscriptaccess=\"always\" allowfullscreen=\"true\" width=\"425\" height=\"344\">
				</embed></object></center></p>\n"
			);
		},
			
		method text (text) {
			for (local n = tablength(text[0]), local i = 0; i < n; ++i) {
				local textitem = ExtractTextContent(text[0][i]);
				if (textitem.separator)
					html(separators[textitem.separator]);
				else
				if (textitem.styles){
					local trailer = "";
					foreach (local style, textitem.styles) {
						local is_pre = style == config.pre_style_name;
						html("<span class=\"" + style + "\">");
						if (is_pre)
							html("<pre>");
						trailer += "</span>";
						if (is_pre)
							trailer = "</pre>" + trailer;
					}
					
					html(textitem.text);
					html(trailer + "\n");
				}
				else
				if(textitem.text)
					html("<span class=\"normal\">" + textitem.text + "</span>"+"\n");
			}
		},	
			
		method link (link) {
			if(link.url)
				html("<a href=\""+link.url+"\"");
			else 
			if(link.sectionref)
				html("<a href=\"javascript:linkToSection('" + link.sectionref + "', false)\"");
			else 
			if(link.contentref)
				html("<a href=\"javascript:set_visible_document('" + link.contentref + "')\"");
				
			if(link.class) 
				html("style=\"text-decoration:none\"><span class='" + link.class + "'>" + link.text + "</span></a>");
			else 
				html(">" + link.text + "</a>");
		},
			
		method imagelink (imagelink) {
			if(imagelink.url)
				html("<div class=\"image\"><a href=\"" + imagelink.url + "\"><img alt=\"" + strlower(imagelink.descr ) +"\" src=\""+imagelink.path+"\" border=\"0\" class=\"left\" align=\"middle\"/></a></div>\n");
			else
				html("<div class=\"image\"><a href=\"javascript:set_visible_document('" + imagelink.contentref + "')\"><img alt=\""+strlower(imagelink.descr)+"\" src=\""+imagelink.path+"\" border=\"0\" class=\"left\" align=\"middle\"/></a></div>\n");
		},
			
		method table (table) {
			html("<p></p><center>\n<table border=\"3\" cellpadding=\"3\">\n");
			for(local i = 0; i<table.rows; i++){
				html("<tr>\n\t");
				for(local j = 0; j < table.collumns; ++j){
					html("<td>\n\t");
					self[table[i][j].type](table[i][j]);
					html("\t</td>\n");
				}
				html("\t</tr>\n");
			}
			html("</table>\n</center><p></p>\n");
		},
			
		method print (document) {	// Print all items of a given document
			
			local size = tablength(document) - 3; // FIXME: what is this '3' constant ?
			if(document.class)	
				size -= 1;
			if(document.index)	
				size -= 1;
					
			if (document.index)	// Add its indexing terms for itself (first).
				for (local terms = tablength(document.index), local j = 0; j < terms; ++j)
					indexingTerms.add(
						document.index[j], 
						document.itemref, 
						document.title,
						true
					);
				
			for (local i = 0; i < size; ++i){
				
				print("\tGenerating ", i + 1, " (out of ", size, ") document items.\n");

				if (document.index)	// Add its indexing terms for every section.
					if ((local section = document[i])[#type] == #section)
						for (local terms = tablength(document.index), local j = 0; j < terms; ++j)
							indexingTerms.add(
								document.index[j], 
								section.itemref, 
								section.title,
								false
							);
				
				if (document[i].class) 
					html("<div class='"+document[i].class+"'>\n");
					
				self[document[i].type](document[i],document);
				
				if (document[i].class) 
					html("</div>\n");
			}
		}
	];
}

/////////////////////////////////////////////////////////////

function HtmlGenerator {
	return [
		method print_menu (menu, style) {
			html("<div id=\"mainmenu\" class=\"menu\">\n<ul>\n\t");
			mainmenu.print(menu,style);
			html("\t</ul>\n</div><!--mainmenu-->\n");
		},
			
		method print_content (documents) {
			html("<div id=\"maincontent\" class=\"content\">\n\t");
			for(local size = tablength(documents), local i = 0; i < size; i++){
				print("Generating ", i + 1, " (out of ", size, ") documents.\n");
				if (documents[i].class) 
					html("<div class='" + documents[i].class + "'>\n");
				html(
					"<div id=\""			+
					documents[i].itemref	+
					"\" class=\""			+
					documents[i].type		+
					"\">\n\t"
				);
				write.anchor(documents[i].title, ANCHOR_TYPE_DOCUMENT);
				document.print(documents[i]);
				html("</div><!--" + documents[i].itemref + "-->\n");
				if (documents[i].class) 
					html("</div>\n");
			}
			indexingTerms.print();
			
			// The following document is used to present the search results.
			html(
				"<div class=\"document\" id=\"i_messageprint\"><p></p></div>
				</div><!--maincontent-->\n"
			);
		}	
	];
}

/////////////////////////////////////////////////////////////

function CssGenerator {
	return [
		method body (color) 
			{ css("body { margin: 0; padding: 0; background-color : " + color + "; height: 100%}\n"); },
		
		method titlebar (titlebar, primary, secondary, logo, menu) {
			
			(logo 							? local marginLeft 	= logo.width + 20 + "px" 	: marginLeft = "2%"				);
			(menu.direction == "horizontal" ? local height 		= "180px" 					: height = "122px"				);
			(titlebar 						? local visibility 	= "" 						: visibility = "display: none;"	);

			css(
				"div.titlebar{ z-index: 1; position: absolute; top: 0px; overflow-x: hidden; background: #FFFFFF url(" +
				titlebar.background 										+
				") repeat-x left top; background-color: " 					+
				titlebar.color												+
				"; width: 100%; height: "	 								+ 
				height 														+ 
				"; " 														+ 
				visibility 													+ 
				"}\n"
			);
			
			if (primary)
				css(
					".titlebar h1, .titlebar p { margin-left: "	+
					marginLeft									+
					"; line-height: normal; font-family: "		+
					primary.font								+
					"; font-weight: bold; color: "				+
					primary.color								+
					"; width: 800px; }\n"
				);
				
			css(
				".titlebar p { font-size: 10px; }\n"
				".titlebar h1 { position: absolute; top: 8px; font-size: 34px; }\n"
			);
			
			if (secondary)	
				css(
					".titlebar h2,"
					".titlebar h3 { position: absolute; top: 60px; margin-left: "	+ 
					marginLeft														+
					"; font-family: "												+
					secondary.font													+
					"; font-weight: normal; color: "								+
					secondary.color													+
					"; width: 800px; }\n"
				);
		},
		
		method toc (table, titlebar) {
			local fromTop = "80px";
			if(titlebar) 
				fromTop = "181px";
			css(
				"div.toc { position: absolute;	left: 1px; top: "		+	
				fromTop													+
				"; width: "												+
				table.width												+
				"%; height: 70%;  overflow : scroll; background-color:"	+
				table.bgColor											+
				"; }\n.toc a { text-decoration: none; font-family: "	+
				table.font												+
				"; font-size: "											+
				table.size												+	
				"; color: "												+
				table.color												+
				"; }\n"
			);
		},
		
		// When the height of the main content does not fit well
		// check that script.js, at 'content heigth calculations'.
			
		method content (background, toc, menu, titlebar) {
			
			local fromTop	= 80;
			local height 	= "80%";
			if (titlebar) 
				fromTop += 100;
				
			if (toc and menu.direction == "vertical")
				print("Cannot have both table of contents and a vertical menu!\n");
			if (toc)
				css(
					"div.content { position: absolute; top: 0px; right: 0px; width: "	+
					(91 - toc.width) + "%;" 											+ 
					" height: " + height + "; overflow : auto; border: hidden; padding: 0.5cm; 
					padding-right: 1cm; margin-top: " 									+ 
					fromTop + "px; background-color: " + background + "; }\n"
				);
			else 
			if (menu.direction == "vertical"){
				if(titlebar) 
					fromTop -= 58;
				css(
					"div.content { position: absolute; top: 0px; right: 0px; width: 80%; height: "	+
					height + "; overflow : scroll; border: hidden; padding: 0.5cm; padding-right: 1cm; margin-left: "	+
					menu.width + "px; margin-top: " + fromTop + "px; background-color: " + background + "; }\n"
				);
			}
			else
				css(
					"div.content { position: absolute; top: 0px; right: 0px; width: 100%; height: " + 
					height + "; overflow : auto; border: hidden; padding: 1cm; margin-top: "		+
					fromTop + "px; background-color: " + background + "; }\n"
				);
		},
		
		method menu (menu, menuButton, submenu,titlebar){
			local fromTop = 10;
			if (titlebar) 
				fromTop += 120;
			if (menu.direction == "horizontal")
				css(
					".menu{	z-index: 1; position: absolute; left: 1%;	top: " + fromTop + 
					"px; border: hidden; 
					font: bold 12px Verdana; padding-right: 10px; }\n
					.menu ul{ margin: 0; padding: 0; list-style-type: none; }\n
					/*Top level list items*/\n
					.menu ul li{ position: relative; display: inline; float: left; }\n
					/*Top level menu link items style*/\n
					.menu ul li a{	display: block;	background: white url(" + menuButton.background + ") top center repeat-x; 
					padding: 14px 17px 13px 17px; margin-right: 1px; 
					/*spacing between tabs*/
					border: 1px solid #778;	border-bottom-width: 0;	color: " + menuButton.textColor + "; text-decoration: none; }\n"
				);
			else 
			if (menu.direction == "vertical"){
				fromTop += 10;
				css(
					".menu{ z-index: 1; width: " + menu.width + "px; position: absolute; left: 1%; top: " + fromTop + 
					"px; border: hidden; font: bold 12px Verdana; }\n
					.menu ul{ margin: 0; padding: 0; list-style-type: none; }\n
					/*Top level list items*/\n
					.menu ul li{ position: relative; }\n
					/*Top level menu link items style*/\n
					.menu ul li a {	display: block;	background: white url(" + menuButton.background + ") repeat-x bottom left;	
					padding: 14px 17px 13px 17px; margin-right: 1px; /*spacing between tabs*/
					border: 1px solid #778;	border-bottom-width: 0;	color: "+ menuButton.textColor + 
					"; text-decoration: none; text-align: center; }\n"
				);
			}
			css(
				".menu ul li a:hover{ background-image: url(" + menuButton.backgroundMouseover + "); color: " + menuButton.textColorMouseOver + "; }\n	
				/*1st sub level menu*/\n
				.menu ul li ul{	position: absolute;	left: 0; z-index: 1; display: block; visibility: hidden; border-top: 1px solid black; }\n
				/*Sub level menu list items (undo style from Top level List Items)*/\n
				.menu ul li ul li{ display: list-item; float: none; }\n
				/*All subsequent sub menu levels vertical offset after 1st level sub menu */\n
				.menu ul li ul li ul{	top: 0; }\n
				/* Sub level menu links style */\n
				.menu ul li ul li a{ font: normal 13px Verdana; width: 160px; /*width of sub menus*/
				background: " + submenu.background + "; color: " + submenu.textColor + "; padding: 4px 5px;	
				margin: 0; border-top-width: 0;	border-bottom: 1px solid black; }\n
				.menu ul li ul li a:hover{ /*sub menus hover style*/ background: " + submenu.backgroundMouseover + 
				"; color: " + submenu.textColorMouseOver + "; }\n
				.downarrowclass {position: absolute;	top: 16px;	right: 5px; }\n
				.rightarrowclass{ position: absolute; top: 5px;	right: 5px; }\n"
			);
		},
		
		method class (text) {	// Produces the CSS from the style definitions in text.
			
			if (not static styleMap)
				styleMap = [
					[ #style		, 		"font-style"		],
					[ #family		,		"font-family" 		], 	
					[ #size			,		"font-size"			],
					[ #weight		,		"font-weight"		],
					[ #display		,		"display"			],
					[ #decoration 	, 		"text-decoration"	],	
					[ #color		,		"color"				],			
					[ #background	,		"background-color"	],
					[ #borderstyle	,		"border-style"		]
				];
				
			foreach (local tag, list_new("span", "div"))
				foreach (local item, text) {
					css(tag + "." + item.name + "{");
					foreach (local styleType, styleMap)
						if (local style = item[ styleType[0] ])
							css(styleType[1] + ":"	+ style + ";");
					css("}\n");
				}
		},
		
		method inline {
			function image_style {
				local style = Styles().imagetext;
				return 	"p.imagetext{ "	+
						"font-family: " + style.font 	+ ";" 	+
						"font-weight: " + style.weight 	+ ";"	+
						"font-size: "	+ style.size	+ ";"	+
						"color: "		+ style.color	+ ";" 	+
						"}";
			}
			
			css(
				"div.logo{ position: absolute; top: 16px; left: 10px; }
				div.search { z-index: 1; position: absolute; right: 10px; top: 10px; }
				div.titles { position: relative; top: 22px; }
				div.document{ display: none; }
				div.indent1, div.indent2, div.indent3, div.indent4, div.indent5{ margin-left: 0.5cm; display: none }
				p.photo	{ font-family: sans-serif; font-weight: bold; font-size: 11px }
				p.description{ font-family: sans-serif; font-weight: bold; font-size: 11px; text-align: right; width: 120px; }
				div.image{ margin: 2px; padding: 2px; }
				div.imageleft{	margin-left: 10px; margin-right: 10px; }
				div.wrapimageleft{ float: left; margin-left: 10px; margin-right: 10px; }
				div.imagecenter{ margin-left: 10px; margin-right: 10px; }
				div.wrapimagecenter{ margin-left: 10px;	margin-right: 10px; }
				div.imageright{	margin-bottom: 10px; margin-left: 86%; margin-right: 10px; }
				div.wrapimageright{ float: right; margin-left: 10px; margin-right: 10px; }" 

				// FIXME: add padding sizes in a config file so that they can be tuned manually
				// as needed.
				
				"span.navbutton li a { padding: 4px 7px 3px 8px; }"
				+
				image_style()
				+
				"div.wrapimage{ float: left; margin-top: 0.5cm; margin-bottom: 0.5cm; font-family: sans-serif; font-weight: bold; font-size: 11px }
				form {float: right;}"
				+ config.css_inline
			);
		},
		
		method print(output, style) {
			@body(style.pageBgColor);
			if (style.titlebar)
				@titlebar(	
					style.titlebar,
					style.primaryTitle,
					style.secondaryTitle,
					style.logo,
					style.menu	
				);
			if (style.toc) 
				@toc(style.toc,style.titlebar);
			@content(
				style.contentBgColor,
				style.toc,
				style.menu,
				style.titlebar
			);
			@menu(
				style.menu,
				style.menuButton,
				style.submenu,
				style.titlebar
			);
			@class(style.class);
			@inline();
			css.flush(output);
		}
	];
}


//////////////////////////////////////////////////////////////////////////////////

function Main {
	
	::html 				= File();
	::css				= File();
	::toc 				= TableOfContents();
	::sectionCounter 	= 0;
	::write 			= Writer();
	::document 			= DocumentItem();
	::data 				= HtmlGenerator();
	::indexingTerms 	= Map();
	::stylesheet 		= CssGenerator();
	::mainmenu 			= Menu();

	rcsetpreprocessor(cpp, "-I" + cwd);

	function LoadResourceData (rc) {
		if (not (local data = rcload(rc))) 		{
			print("Syntax error in rc file: ", rc, nl, rcloadgeterror(), nl);
			return nil;
		}
		else
			return data;
	}

	if (config  = LoadResourceData("Data\\config.rc")) {
		
		content = LoadResourceData(config.content);
		style 	= LoadResourceData(config.style);

		if (content and style){

			local ts = currenttime();
			
			write.initialize(config.script, config.output_css);
			if(style.titlebar)
				write.titlebar(content.maintitle,style.logo);
			if(style.toc)
				toc.print(content.maincontent);
			data.print_menu(content.mainmenu, style.menu);
			data.print_content(content.maincontent);
			write.searchbar();
			write.finalize(config.out_dir + config.output_html, style.menu);
			stylesheet.print(config.out_dir + config.output_css, style);

			ts = currenttime() - ts;
			print(
				"Site done in ",  ts / 1000, " seconds (", ts / 60000, " minutes).\n"
			);
		}
	}
}

Main();

//////////////////////////////////////////////////////////////////////////////////
