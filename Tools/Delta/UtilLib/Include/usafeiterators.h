// usafeiterators.h
// Safe iterators when removal of elements (including clear) occurs
// in their associated container.
// ScriptFighter Project.
// A. Savidis, August 2009.
//
#ifndef	USAFEITERATORS_H
#define	USAFEITERATORS_H

#include "utypes.h"
#include "ucallbacks.h"

//---------------------------------------------------------------

class usafecontainerbehavior {
	
	public:
	typedef void (*on_remove_f)(const void* elem, void* closure);
	typedef void (*on_clear_f)(void* closure);

	private:
	ucallbacklist<on_remove_f>	onRemove;
	ucallbacklist<on_clear_f>	onClear;

	public:
	void		add_on_remove (on_remove_f f, void* c = (void*) 0)
					{ onRemove.add(f,c); }
	void		remove_on_remove (on_remove_f f, void* c = (void*) 0)
					{ onRemove.remove(f,c); }
	void		notify_removed (const void* elem) 
					{ onRemove.call(elem); }

	void		add_on_clear (on_clear_f f, void* c = (void*) 0)
					{ onClear.add(f,c); }
	void		remove_on_clear (on_clear_f f, void* c = (void*) 0)
					{ onClear.remove(f,c); }
	void		notify_cleared (void) 
					{ onClear.call(); }
};

/////////////////////////////////////////////////////////////////

class UTILLIB_CLASS usafeiteratorbehavior {
		
	protected:
	bool ignoreNextFwd;
	virtual usafecontainerbehavior*	get_container (void)		= 0;
	virtual const void*				get_value_address (void)	= 0;
	virtual void					set_at_end (void)			= 0;
	virtual void					fwd (void)					= 0;
	virtual bool					is_at_end (void) const		= 0;

	void							on_before_set_container (void)		
										{ remove_callbacks(); }
	void							on_after_set_container (void)		
										{ add_callbacks(); }
	void							clear (void) 
										{ if (get_container()) remove_callbacks(); }

	private:
	static void on_element_remove (const void* elem, void* self);
	static void on_container_clear (void* self);

	void add_callbacks (void) {
		get_container()->add_on_remove(&on_element_remove, this);
		get_container()->add_on_clear(&on_container_clear, this);
	}

	void remove_callbacks (void) {
		get_container()->remove_on_remove(&on_element_remove, this);
		get_container()->remove_on_clear(&on_container_clear, this);
	}

	public:
	bool							should_ignore_next_fwd (void) const 
										{ return ignoreNextFwd; }
	void							reset_ignore_next_fwd (void)
										{ ignoreNextFwd = false; }

	usafeiteratorbehavior (void) : ignoreNextFwd(false){}
	usafeiteratorbehavior (const usafeiteratorbehavior& i) : ignoreNextFwd(i.ignoreNextFwd){}
	virtual ~usafeiteratorbehavior() {}
};

//---------------------------------------------------------------

#endif	// Do not add stuff beyond this point.
