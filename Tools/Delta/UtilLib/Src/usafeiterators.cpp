// usafeiterators.cpp
// Safe iterators when removal of elements (including clear) occurs
// in their associated container.
// ScriptFighter Project.
// A. Savidis, August 2009.
//

#include "usafeiterators.h"

void usafeiteratorbehavior::on_element_remove (const void* elem, void* self) {
	usafeiteratorbehavior* i = (usafeiteratorbehavior*) self;
	if (!i->is_at_end() && i->get_value_address() == elem)	// Removing current element.
		{ i->fwd(); i->ignoreNextFwd = true; }				// Fwd automatically.
}

void usafeiteratorbehavior:: on_container_clear (void* self) {
	usafeiteratorbehavior* i = (usafeiteratorbehavior*) self;
	if (!i->is_at_end()) {
		i->ignoreNextFwd = true;
		i->set_at_end();									// Fwd automatically.
	}
}