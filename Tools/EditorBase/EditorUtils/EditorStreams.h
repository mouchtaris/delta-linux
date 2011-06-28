/**
 *	EditorStreams.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef EDITOR_EDITORSTREAMS_H
#define EDITOR_EDITORSTREAMS_H

#include "EditorCommon.h"

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>

namespace editor {

////////////////////////////////////////////////////////////////////////

class EditorWindow;

////////////////////////////////////////////////////////////////////////

class _EDITOR_API EditorReadDevice {
public:
	typedef char							char_type;
	typedef boost::iostreams::source_tag	category;

	EditorReadDevice (EditorWindow* win, size_t pos, size_t end = (size_t) -1) :
		m_editor(win), m_pos(pos), m_end(end) {}

	void SetEditor (EditorWindow* editor) { m_editor = editor; }
	void SetRange (size_t pos, size_t end = (size_t) -1);

	std::streamsize read (char* s, std::streamsize n);

private:
	EditorWindow*	m_editor;
	size_t			m_pos;
	size_t			m_end;
};

////////////////////////////////////////////////////////////////////////

class _EDITOR_API EditorInputStream : public boost::iostreams::stream<EditorReadDevice> {
public:
	EditorInputStream (EditorWindow* win, size_t pos = 0, size_t end = (size_t) -1) :
		boost::iostreams::stream<EditorReadDevice>(EditorReadDevice(win, pos, end), 100, 0) {}
};

////////////////////////////////////////////////////////////////////////

} // namespace editor

#endif // EDITOR_EDITORSTREAMS_H
