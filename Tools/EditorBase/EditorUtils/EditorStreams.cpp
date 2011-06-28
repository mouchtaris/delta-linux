/**
 *	EditorStreams.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "EditorStreams.h"
#include "EditorMain/EditorWindow.h"

namespace editor {

////////////////////////////////////////////////////////////////////////
// EditorReadDevice
//
void EditorReadDevice::SetRange (size_t pos, size_t end)
{
	m_pos = pos;
	m_end = end;
}

//**********************************************************************

std::streamsize EditorReadDevice::read (char* s, std::streamsize n)
{
	std::streamsize i = 0;
	int c;
	while (n--) {

		if (m_pos < m_end && (c = m_editor->GetCharAt((int) m_pos++)))
			s[i++] = (char) c;
		else
			return (i == 0) ? -1 : i;
	}
	return i;
}

//**********************************************************************

////////////////////////////////////////////////////////////////////////

} // namespace editor
