/////////////////////////////////////////////////////////////////////////////
// Interactive web document generation from content defintions.
// Diploma thesis supervisor:	Anthony Savidis
// Student:						Konstantinos Mousikos
// Delivery date:				03/11/2009
// Refactorings and extensions:	Anthony Savidis	
//

/////////////////////////////////////////////////////////////////////////////
// Constants that are dependent on the content generation.

var mainContentId 					= "maincontent";
var documentClassName				= "document";
var sectionClassName				= "section";
var searchResultsSectionId			= "i_messageprint";
var homePageDocumentId				= "homepage";
var tocDocumentId					= "tableofcontents";
var verticalMenuDocumentId			= "verticalMenu";
var deltaTitleBarDocumentId			= "deltabar";
var documentTitleDocumentId			= "documenttitle";
var indexingTermsDocumentId			= "i_indexingterms";
var indexingTermsDocumentTitle		= "Indexing Terms";
var indexingTermsSuffix				= "IndexingTerm";
var buttonImageNamePrefix			= "widget";
var mainMenuDocumentId				= "mainmenu";
var cssGeneratedFile				= "styles.css";
var indexingVisibleBySearch			= false;

/////////////////////////////////////////////////////////////////////////////
// Constants for images used in the site map.

var minus_img   = "minus.gif";
var plus_img    = "plus.gif";
var img_dir     = "images/";
var plus_len    = plus_img.length;

///////////////////////////////////////////////////////////////////
// List

function List_new_node (data, nextNode) {
	this.data 		= data;
	this.next 		= nextNode || null;
	this.getdata 	= function () 			{ return this.data; };
	this.setdata 	= function (data) 		{ this.data = data; };
	this.getnext 	= function () 			{ return this.next; };
	this.setnext 	= function (nextNode) 	{ this.next = nextNode; };
	this.to_string 	= function () 			{ return this.data.to_string(); };
}

//***************************

function List (name) {
	this.name 		= name || "a list";
	this.firstNode 	= this.lastNode = null;
	this.getname 	= function () 		{ return this.name; };
	this.setname 	= function (name) 	{ this.name = name; };
	this.empty 		= function () 		{ return this.firstNode == null; };
}

//***************************

function List_push_front (insertItem) {
	if (this.empty())
		this.firstNode = this.lastNode = new List_new_node(insertItem);
	else
		this.firstNode = new List_new_node(insertItem, this.firstNode);
}

//***************************

function List_push_back (insertItem) {
	if (this.empty())
		this.firstNode = this.lastNode = new List_new_node(insertItem);
	else {
		this.lastNode.setnext( new List_new_node(insertItem) );
		this.lastNode = this.lastNode.getnext();
	}
}

//***************************

function List_front()
	{ return this.firstNode; }

function List_back()
	{ return this.lastNode; }

//***************************

function List_pop_front() {
	if (this.empty())
		return null;
	var removeItem = this.firstNode.getdata();
	if (this.firstNode == this.lastNode)
		this.firstNode = this.lastNode = null;
	else
		this.firstNode = this.firstNode.getnext();
	return removeItem;
}

//***************************

function List_pop_back() {
	if ( this.empty() )
		return null;
	var removeItem = this.lastNode.getdata();
	if ( this.firstNode == this.lastNode )
		this.firstNode = this.lastNode = null;
	else {
		var p = this.firstNode;
		while (p.getnext() != this.lastNode)
			p = p.getnext();
		this.lastNode = p;
		p.setnext(null);
	}
	return removeItem;
}

//***************************

function List_to_string() {
	var p = this.firstNode,
	str = "";
	while (p) {
		str += p + " ";
		p = p.next;
	}
	return str;
}

//***************************

List.prototype.push_front 		= List_push_front;
List.prototype.push_back 		= List_push_back;
List.prototype.first 			= List_front;
List.prototype.last 			= List_back;
List.prototype.pop_front 		= List_pop_front;
List.prototype.pop_back 		= List_pop_back;
List.prototype.to_string 		= List_to_string;

///////////////////////////////////////////////////////////////////

function Stack(name) {
	this.base = List;
	this.base(name);
	this.top 	= function ()			{ return this.first(); };
	this.push 	= function (insertItem)	{ this.push_front( insertItem ); };
	this.pop 	= function () 			{ return this.pop_front(); };
}

Stack.prototype = new List;

/////////////////////////////////////////////////////////////////////////////
// Undo / Redo

var visibledocument = "none";
var redoStack 		= new Stack( "redo stack" );
var undoStack 		= new Stack( "undo stack" );
var trace 			= "";

//***************************

function undo_redo_handle_visible(){
	undoStack.push(visibledocument);
	redoStack = new Stack( "redo stack" );
}

//***************************

function undo(){
	if (!undoStack.empty()){
		var undoDoc = undoStack.pop();
		redoStack.push(visibledocument);
		apply_set_visible_document(undoDoc);
	}
}

//***************************

function redo(){
	if (!redoStack.empty()){
		var redoDoc = redoStack.pop();
		undoStack.push(visibledocument);
		apply_set_visible_document(redoDoc);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Site map generation.

function sitemap(){

	var values 	= document.getElementById(mainContentId).childNodes;
	var len 	= values.length;

	for (var i = 0; i < len; ++i) {
		var curr = values[i];
		if (curr.className == documentClassName)
			sitemap_recursion(curr, 1, curr.id);
	}
	set_visible_document(searchResultsSectionId);
}

//*******************************

function sitemap_recursion(node, indent, ref){
	var values 	= node.childNodes;
	var len 	= values.length;
	
	for (var i = 0; i < len; ++i) {
		var curr = values[i];
		if (curr.className == sectionClassName)
			sitemap_recursion(curr, indent + 1, ref);
		else 
		if(curr.nodeName == "H2")
			appendLink(ref, (curr.textContent ? curr.textContent : curr.innerText) );
	}	
}

/////////////////////////////////////////////////////////////////////////////

var rv = -1;

if (navigator.appName == 'Microsoft Internet Explorer'){
	var ua 	= navigator.userAgent;
	var re  = new RegExp("MSIE ([0-9]{1,}[\.0-9]{0,})");
	if (re.exec(ua) != null)
		rv = parseFloat( RegExp.$1 );
}

function load(){
	if (visibledocument != "none")
		document.getElementById(visibledocument).style.display 	= "none";   
	document.getElementById(homePageDocumentId).style.display 	= "block";
	visibledocument = homePageDocumentId;

	checkAndParseURL();
	if(rv != 6)	
		handleResizes();
}

/////////////////////////////////////////////////////////////////////////////
// Content height calculations.

function handleResizes() {

	var tocExists 		= document.getElementById(tocDocumentId);
	var menuVertical 	= document.getElementById(verticalMenuDocumentId);
	var titleExists 	= document.getElementById(deltaTitleBarDocumentId);
	var contentH 		= -118;
	var contentW 		= -80;
	var tocH 			= -80;

	if (titleExists && menuVertical) 
		contentH = -198;
	else 
	if (titleExists){
		if (tocExists) 
			contentH = -225;
		else 
			contentH = -242;

		tocH = -182;
	}
	else 
	if (!titleExists) 
		contentH -= 54;
		
	if (window.innerHeight){
		contentH 	+= window.innerHeight;
		contentW 	+= window.innerWidth;
		tocH 		+= window.innerHeight;
	}
	else 
	if (document.body.clientHeight){
		contentH 	+= document.body.clientHeight;
		contentW 	+= document.body.clientWidth;
		tocH 		+= document.body.clientHeight;
	}
	else 
	if (document.documentElement.clientHeight){
		contentH	+= document.documentElement.clientHeight;
		contentW 	+= document.documentElement.clientWidth;
		tocH 		+= document.documentElement.clientHeight;
	}
	
	if (menuVertical){
		contentH	+=	54;
		contentW 	-= document.getElementById(mainMenuDocumentId).clientWidth;
		document.getElementById(mainContentId).style.width = contentW + "px";
	}

	document.getElementById(mainContentId).style.height = contentH + "px";
	if (tocExists)
		document.getElementById(tocDocumentId).style.height = tocH + "px";
}

if(rv != 6) 
	window.onresize = handleResizes;

function checkAndParseURL() {
     var state = window.location.hash;
     restorePage(state);
}

/////////////////////////////////////////////////////////////////////////////

function linkToSection (state, set_location_flag) {

	var str = state.replace("#","");
	if (state == "" || str == "")
		return;
	var divs = document.getElementsByTagName("DIV");
	var len  = divs.length;

	for (var i = 0; i < len; ++i)
		if (divs[i].className == sectionClassName) {

			var children = divs[i].childNodes;
			var num 	 = children.length;

			for (var j = 0; j < num; ++j)
				if (children[j].nodeName == "A" && children[j].name == str) {
					var parent = divs[i];
					while(parent.className != documentClassName)
						parent = parent.parentNode;
					set_visible_document(parent.id);
					if (set_location_flag)
						window.location = state;
				}
		}
}

//***************************

function restorePage (state) 
	{ linkToSection(state, true); }

//***************************

function changeTitle (newdocument){
	
	function changeTitle_recursion (nodes, f_inner_nodes) {
		for (var i = 0; i < nodes.length; ++i) {
			if (nodes[i].className == documentTitleDocumentId) {
				document.title = (nodes[i].textContent ? nodes[i].textContent : nodes[i].innerText);
				return;
			}
			f_inner_nodes(nodes[i]);
		}
	}

	changeTitle_recursion(
		document.getElementById(newdocument).childNodes, 
		function (node) {
			if (node.nodeName == "A")
				changeTitle_recursion(node.childNodes, function(node){});
		}
	);
}

//***************************

function apply_set_visible_document (newdocument){
	document.getElementById(visibledocument).style.display = "none";	// Hidden.
	visibledocument = newdocument;
	if (newdocument == indexingTermsDocumentId)
		document.title = indexingTermsDocumentTitle;
	changeTitle(newdocument);
	document.getElementById(visibledocument).style.display = "block"; // Revealed.
}

//***************************

function set_visible_document (newdocument){
	if (!document.getElementById(newdocument))
		return;
	if (newdocument == visibledocument)
		if (newdocument != indexingTermsDocumentId || !indexingVisibleBySearch)
			return;
	if (newdocument == indexingTermsDocumentId) {
		getReferences(indexingTermsDocumentId);
		indexingVisibleBySearch = false;
	}
	undo_redo_handle_visible();
	apply_set_visible_document(newdocument);
}

/////////////////////////////////////////////////////////////////////////////
// Search facility.

function appendMessage (text){
	var node = document.getElementById(searchResultsSectionId);
	trace += node.nodeName;
	node.appendChild(document.createTextNode(text));
	node.appendChild(document.createElement("p"));
}

function appendLink (ref, text){
	var newElem = document.createElement("p");
	var newHtml = "<a href=\"javascript:set_visible_document('"+ref+"')\">"+text+"</a>";
	var node = document.getElementById(searchResultsSectionId).firstChild;
	newElem.innerHTML = newHtml;
	node.appendChild(newElem);
}

function printMessage (text) {
	appendMessage(text);
	set_visible_document(searchResultsSectionId);
}

function getReferences (indexingTerm){
	var key 	= document.getElementById(indexingTerm);
	var values  = key.childNodes;
	var len 	= values.length;
	for (var i = 0; i < len; ++i) {
		if(values[i].nodeName == "DIV")
			values[i].style.display = "block";
	}
}

function search (str){
	var key 		= document.getElementById(indexingTermsDocumentId);
	var values 		= key.childNodes;
	var len 		= values.length;
	var userText	= str.srchval.value;
	var termId		= userText.toLowerCase() + indexingTermsSuffix;

	if (document.getElementById(termId) == null){
		printMessage("Did not find any references of \"" + userText + "\".");
		return;
	}

	for (var i = 0; i < len; ++i) {
		if(values[i].nodeName == "DIV")
			values[i].style.display = "block";
	}

	set_visible_document(indexingTermsDocumentId);
	indexingVisibleBySearch = true;
	getReferences(termId);

	for (var i = 0; i < len; ++i) {
		if(values[i].nodeName == "DIV" && values[i].id != termId)
			values[i].style.display = "none";
	}
}

/////////////////////////////////////////////////////////////////////////////

function expand (section) {
	var key     = document.getElementById(sectionClassName + section);
	var values  = key.childNodes;
	var len     = values.length;
    
   // Switch image from plus to minus.
	
	var img = document.images[buttonImageNamePrefix + section];
	var len = img.src.length;
	img.src = (img.src.substr(len - plus_len, plus_len) == plus_img) ? (img_dir + minus_img) : (img_dir + plus_img);
		
	window.focus();		
	for (var i = 0; i < len; ++i){
		if (values[i].nodeName == "DIV") {
			values[i].style.display = (values[i].style.display == "block") ? "none" : "block";
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

function printDocument() {

	var displaySetting 		= "toolbar=yes,location=no,directories=yes,menubar=yes,scrollbars=yes,left=100,top=25";
	var contentInnerHtml 	= document.getElementById(visibledocument).innerHTML;
	var documentPrint		= window.open("","", displaySetting);
	var autoPrintPrompt		= 'onLoad="self.print();"';	// Make it empty to deactivate automatic print prompt.

	documentPrint.document.open();
	documentPrint.document.write(
		'<html><head><title>Print</title><link rel="stylesheet" href="' + 
		cssGeneratedFile +
		'" type="text/css" ></head>'
	);
	documentPrint.document.write('<body ' + autoPrintPrompt + ' >');  
	documentPrint.document.write(contentInnerHtml);
	documentPrint.document.write('</body></html>');
}

/////////////////////////////////////////////////////////////////////////////

/*********************
//* jQuery Multi Level CSS Menu (horizontal)- By Dynamic Drive DHTML code library: http://www.dynamicdrive.com
//* Menu instructions page: http://www.dynamicdrive.com/dynamicindex1/ddlevelsmenu/
//* Last modified: Sept 6th, 08'. Usage Terms: http://www.dynamicdrive.com/style/csslibrary/tos/
*********************/

//Specify full URL to down and right arrow images (25 is padding-right to add to top level LIs with drop downs):
var arrowimages = { 
    down    : ['downarrowclass', 'images/arrow-down.gif', 25], 
    right   : ['rightarrowclass', 'images/arrow-right.gif']
}

var jquerycssmenu={

fadesettings: {overduration: 350, outduration: 100}, //duration of fade in/ out animation, in milliseconds
effects: {enableswipe: true, enablefade: false, duration: 500},

buildmenu:function(menuid, arrowsvar){
	jQuery(document).ready(function($){
		var verticalMenu = document.getElementById("verticalMenu");
		var $mainmenu=$("#"+menuid+">ul")
		var $headers=$mainmenu.find("ul").parent()
		$headers.each(function(i){
			var $curobj=$(this)
			var $subul=$(this).find('ul:eq(0)')
			this._dimensions={w:this.offsetWidth, h:this.offsetHeight, subulw:$subul.outerWidth(), subulh:$subul.outerHeight()}
			this.istopheader=$curobj.parents("ul").length==1? true : false
			if(verticalMenu){
				$subul.css({top: 0})
				$curobj.children("a:eq(0)").css(this.istopheader? {paddingRight: arrowsvar.down[2]} : {}).append(
					'<img src="'+ arrowsvar.right[1]
					+'" class="' + (this.istopheader? arrowsvar.down[0] : arrowsvar.right[0])
					+ '" style="border:0; z-index =1;" />'
				)
			}
			else{
				$subul.css({top:this.istopheader? this._dimensions.h+"px" : 0})
				$curobj.children("a:eq(0)").css(this.istopheader? {paddingRight: arrowsvar.down[2]} : {}).append(
					'<img src="'+ (this.istopheader? arrowsvar.down[1] : arrowsvar.right[1])
					+'" class="' + (this.istopheader? arrowsvar.down[0] : arrowsvar.right[0])
					+ '" style="border:0;" z-index =1;/>'
				)
			}
			$curobj.hover(
				function(e){
					var $targetul=$(this).children("ul:eq(0)")
					this._offsets={left:$(this).offset().left, top:$(this).offset().top}
					if(verticalMenu)
						var menuleft=this._dimensions.w
					else
						var menuleft=this.istopheader? 0 : this._dimensions.w
					menuleft=(this._offsets.left+menuleft+this._dimensions.subulw>$(window).width())? (this.istopheader? -this._dimensions.subulw+this._dimensions.w : -this._dimensions.w) : menuleft
					$targetul.css({left:menuleft+"px"}).fadeIn(jquerycssmenu.fadesettings.overduration)
				},
				function(e){
					$(this).children("ul:eq(0)").fadeOut(jquerycssmenu.fadesettings.outduration)
				}
			) //end hover
		}) //end $headers.each()
		$mainmenu.find("ul").css({display:'none', visibility:'visible'})
	}) //end document.ready
}
}

jquerycssmenu.buildmenu(mainMenuDocumentId, arrowimages)

/////////////////////////////////////////////////////////////////////////////
