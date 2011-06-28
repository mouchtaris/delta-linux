/**
 *	ComponentUserCommandDesc.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef IDE_COMPONENTUSERCOMMANDDESC_H
#define IDE_COMPONENTUSERCOMMANDDESC_H

#include "Common.h"
#include "IDECommon.h"
#include "Message.h"

#include <vector>

namespace ide {

////////////////////////////////////////////////////////////////////////

enum UserCommandDescFlags {
	UC_MAIN		= 1 << 0,
	UC_CTX		= 1 << 1,
	UC_TOOLBAR	= 1 << 2,
	UC_NMAIN	= UC_CTX  | UC_TOOLBAR,
	UC_NTOOLBAR	= UC_MAIN | UC_CTX,
	UC_ALL		= UC_MAIN | UC_CTX | UC_TOOLBAR,
};

////////////////////////////////////////////////////////////////////////

class _IDE_API UserCommandDesc {
public:

	// Component class id, Function id
	//
	typedef std::pair<std::string, std::string> Callback;

	UserCommandDesc (void);
	UserCommandDesc (
		const Callback&		callback,
		bool				isCheck = false,
		long				flags = UC_ALL,
		bool				isStatic = true,
		bool				isEnabled = true,
		const String		image = String()
	) : m_isCheck(isCheck), m_isStatic(isStatic), m_isEnabled(isEnabled), m_callback(callback),
		m_flags(flags), m_image(image) {}

	//******************************************************************

	bool			IsValid	(void) const { return !m_callback.first.empty() && !m_callback.second.empty(); }

	const Callback& GetCallback (void) const { return m_callback; }
	void			SetCallback (const Callback& cb) { m_callback = cb; }

	bool			IsCheck (void) const { return m_isCheck; }
	void			SetIsCheck (bool value) { m_isCheck = value; }

	long			GetFlags (void) const { return m_flags; }
	void			SetFlags (long flags) { m_flags = flags; }

	bool			IsStatic (void) const { return m_isStatic; }
	void			SetStatic (bool value) { m_isStatic = value; }

	bool			IsEnabled (void) const { return m_isEnabled; }
	void			SetEnabled (bool value) { m_isEnabled = value; }

	const String&	GetImage (void) const { return m_image; }
	void			SetImage (const String& image) { m_image = image; }

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize (Archive& ar, const unsigned int version)
		{ ar & m_callback & m_isCheck & m_flags & m_isEnabled & m_image; }

	Callback	m_callback;
	bool		m_isCheck;
	long		m_flags;
	bool		m_isEnabled;
	bool		m_isStatic;
	String		m_image;
};

////////////////////////////////////////////////////////////////////////

} // namespace ide

#endif // IDE_COMPONENTUSERCOMMANDDESC_H
