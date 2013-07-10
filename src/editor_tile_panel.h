#ifndef __EDITOR_TILEPANEL__
#define __EDITOR_TILEPANEL__
/*
Copyright 2012-2013 EngineerOfLies
This file is part of the Engine of Lies game engine library and Editor of Lies
level editor.

The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

EOL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the EOL game engine and the EOL level editor.  If not,
see <http://www.gnu.org/licenses/>.
*/

#include <eol_window.h>


eolWindow * editor_tile_panel(eolWindow * workspace);
void editor_tile_workspace_sync(eolWindow *win);
void editor_tile_orientation_update(void *data,eolOrientation ori);
void editor_tile_setup_ori_edit(eolWindow *win,eolWindow *ori_edit);

#endif
