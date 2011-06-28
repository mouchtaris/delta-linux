// ueditormixin.cpp
// Editor mixin layer implementation.
// A. Savidis, October 2006.
//

#include "ueditormixin.h"
#include "ustrings.h"

#include <string>

ueditor::udata* ueditor::clipboard	= (udata*) 0;
ueditor*		ueditor::editor		= (ueditor*) 0;

////////////////////////////////////////////////////////
// udata.
//
void ueditor::udata::normalize (void) {

	if (!isNormalized) {
		if (sel) {
			DDELETE(sel);
			sel = (uselection*) 0;
		}

		normalizeprivate();
		makesel();
		isNormalized = true;
	}
}

////////////////////////////////////////////////////////
// BASIC ACTIONS.
//

// INSERT
//
ueditor::uinsertaction::uinsertaction (const uposition& at, const udata& data) {
	pos	= at.clone();
	curr = data.clone();
	DPTR(curr)->normalize();

	uselection* sel = DPTR(curr)->toselection().offsetby(*pos);
	old	= ueditor::editor->insertaffected(*sel);
	if (old)	// Not necessarily affecting data.
		DPTR(old)->normalize();

	DDELETE(sel);
}

ueditor::uinsertaction::uinsertaction (const uinsertaction& action) {
	pos		= DPTR(action.pos)->clone();
	old		= old? DPTR(action.old)->clone() : 0;
	curr	= DPTR(action.curr)->clone();
}

ueditor::uinsertaction::~uinsertaction(){ 
	DDELETE(pos);
	DDELETE(curr);
	if (old)	// Null old means insert was the opposite of a delete.
		DDELETE(old);
}

//**************************

ueditor::uaction* ueditor::uinsertaction::clone (void) const {
	return DNEWCLASS(uinsertaction, (*this));
}

//**************************

ueditor::uaction* ueditor::uinsertaction::opposite(void) const {

	// If we have old data overwritten by the insert, it means
	// the opposite is a new insert with old and curr reversed.
	// Else, i.e. no old, the opposite is a delete with 'old' buffer
	// the 'curr' of the insert!
	//
	if (old)
		return	DNEWCLASS(
					uinsertaction,
					(*DPTR(pos), *DPTR(old), DPTR(curr))
				);
	else {
		uselection* sel = DPTR(curr)->toselection().offsetby(*pos);
		uaction*	del	= DNEWCLASS(udeleteaction, (*sel, *curr));
		DDELETE(sel);
		return del;
	}
}

//**************************

void ueditor::uinsertaction::apply(void) const {
	ueditor::editor->insert(*DPTR(pos), *DPTR(curr));
}

//**************************

const std::string ueditor::uinsertaction::description (void) const {
	std::string s("ins:");
	s.append(DPTR(pos)->description());
	s.append(DPTR(curr)->description());
	return s;
}

////////////////////////////////////////////////////////
// DELETE
//

ueditor::udeleteaction::udeleteaction (const uselection& sel) {
	area	= sel.clone();
	old		= ueditor::editor->undocopy(sel);
	DPTR(old)->normalize();
}

//**************************

ueditor::udeleteaction::udeleteaction (const udeleteaction& action) {
	area	= DPTR(action.area)->clone();
	old		= DPTR(action.old)->clone();
	DASSERT(DPTR(old)->isnormalized());
}

//**************************

ueditor::udeleteaction::udeleteaction (
			const uselection&	_area,
			const udata&		_old
		) {
	area	= _area.clone();
	old		= _old.clone();
	DASSERT(DPTR(old)->isnormalized());
}

//**************************

ueditor::udeleteaction::~udeleteaction(){ 
	DDELETE(area);
	DDELETE(old);
}

//**************************

ueditor::uaction* ueditor::udeleteaction::clone (void) const {
	return DNEWCLASS(udeleteaction, (*this));
}

//**************************
// The opposite of delete is always an insert action
// of the copied original data.
//
ueditor::uaction* ueditor::udeleteaction::opposite(void) const {

	return	DNEWCLASS(
				uinsertaction,
				(
					DPTR(area)->toposition(),
					*DPTR(old),
					(udata*) 0	// No old data, meaning it is from a delete.
				)
			);
}

//**************************

void ueditor::udeleteaction::apply(void) const {
	ueditor::editor->remove(*DPTR(area));
}

//**************************

const std::string ueditor::udeleteaction::description (void) const {
	std::string s("del:");
	s.append(DPTR(area)->description());
	return s;
}

////////////////////////////////////////////////////////
// SET ATTRIBUTES
//

ueditor::usetattrsaction::usetattrsaction (
				const uselection&	_area, 
				const uattrs&		_attrs
			) {
	sw		= true;
	area	= _area.clone();
	attrs	= _attrs.clone();
	old		= ueditor::editor->undocopy(_area);
	DPTR(old)->normalize();
}

//**************************

ueditor::usetattrsaction::usetattrsaction (const usetattrsaction& action) {
	sw		= action.sw;
	area	= DPTR(action.area)->clone();
	attrs	= DPTR(attrs)->clone();
	DASSERT(DPTR(action.old)->isnormalized());
	old		= DPTR(action.old)->clone();
}

//**************************

ueditor::usetattrsaction::usetattrsaction (	
			const uselection&	_area,	 
			const uattrs&		_attrs,
			const udata&		_old,
			bool				_sw
			) {

	area	=	_area.clone();
	attrs	= _attrs.clone();
	DASSERT(_old.isnormalized());
	old		= _old.clone();
	sw		= _sw;
}

//**************************

ueditor::usetattrsaction::~usetattrsaction(){ 
	DDELETE(area);
	DDELETE(attrs);
	DDELETE(old);
}

//**************************

ueditor::uaction* ueditor::usetattrsaction::clone (void) const {
	return DNEWCLASS(usetattrsaction, (*this));
}

//**************************
// The opposite of set attrs is essentially an insert but
// internally in the form of a setattrs.
//
ueditor::uaction* ueditor::usetattrsaction::opposite(void) const {
	DASSERT(area && attrs && old);
	return	DNEWCLASS(
				usetattrsaction,
				(*DPTR(area), *DPTR(attrs), *DPTR(old), !sw)	// We only invert the switch!
			);

}

//**************************

void ueditor::usetattrsaction::apply(void) const {
	if (sw)	// Explicit setattrs with 'attrs'
		ueditor::editor->setattrs(*DPTR(area), *DPTR(attrs));
	else	// Insert with 'old'
		ueditor::editor->insert(DPTR(area)->toposition(), *DPTR(old));
}

//**************************

const std::string ueditor::usetattrsaction::description (void) const {

	std::string s("attrs:");

	if (sw) {
		s.append(DPTR(area)->description());
		s.append(DPTR(attrs)->description());
	}
	else {
		s.append(DPTR(area)->toposition().description());
		s.append(DPTR(old)->description());
	}

	return s;
}

////////////////////////////////////////////////////////

const std::string ueditor::uundoredo::description(
						const char*					id, 
						const std::list<uaction*>&	st
					) const {

	std::string s(id);

	if (st.empty())
		s.append("[0]");
	else {
		s.append(uconstructstr("[%d]", st.size()));
		s.append(DPTR(*st.begin())->description());
	}

	return s;
}

//**************************

const std::string ueditor::uundoredo::description (void) const {
	return description("U:", undoStack) + description("R:", redoStack);
}

////////////////////////////////////////////////////////
// CLIPBOARD MANAGEMENT.
//
void ueditor::cut (void) {
	if (currSelection && currSelection->isactive()) {
		copy();
		remove();
	}
}

//**************************

void ueditor::copy (void) {
	if (currSelection && currSelection->isactive()) {
		if (clipboard)
			DDELETE(clipboard);
		clipboard = deepcopy(*currSelection);
	}
}

//**************************

void ueditor::paste (void) {
	if (clipboard) {
		if (validate(clipboard)) {
			DASSERT(focusPosition);
			uinsertaction action(*focusPosition, *clipboard);
			edit(action);
		}
		else {
			DDELETE(clipboard);
			clipboard = (udata*) 0;
		}
    }
}

//**************************

void ueditor::remove (void) {
	if (currSelection && currSelection->isactive()) {
		udeleteaction action(*currSelection);
		edit(action);
	}
}

////////////////////////////////////////////////////////

ueditor::ueditor (void): undoRedo(av), av(this){
	clipboard		= (udata*) 0;
	currSelection	= (uselection*) 0;
	focusPosition	= (uposition*) 0;
}

//**************************

ueditor::~ueditor(){
	if (clipboard) {
		DDELETE(clipboard);
		clipboard = (udata*) 0;
	}
}

////////////////////////////////////////////////////////