#ifndef __EDITOR_WORKSPACE__
#define __EDITOR_WORKSPACE__
/*
Copyright 2012 EngineerOfLies
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
#include <eol_level.h>

/*working level data*/
typedef struct
{
  eolBool         updated; /**<if modified since last frame*/
  eolBool         modified;/**<if modified since last save*/
  eolLine         path;
  eolLine         filename;
  eolLevel      * level;
  eolLevelLayer * activeLayer;
}EditorWorkspace;

/*workspace utility*/
/**
 * @brief closes current level and creates a new default level to edit
 * @param workspace the workspace where the level will be created
 */
void editor_workspace_create_new_level(eolWindow *workspace);

eolLevel * editor_workspace_get_level(eolWindow *workspace);

EditorWorkspace *editor_get_workspace(eolWindow *workspace);

/*layer operations*/

/**
 * @brief adds a new layer to the level
 * @param workspace the workspace to add a layer to
 */
void editor_workspace_add_layer(eolWindow *workspace);
void editor_workspace_delete_layer(eolWindow *workspace,eolUint layer);
void editor_workspace_select_layer(eolWindow *workspace,eolUint layer);

/*maintaining updates*/
eolBool editor_workspace_updated(eolWindow *workspace);
eolBool editor_workspace_modified(eolWindow *workspace);
void editor_workspace_clear_updated(eolWindow *workspace);

/*Loading and saving the level*/
void editor_workspace_load_level(eolWindow *workspace,eolLine filename);
void editor_workspace_save_level(eolWindow *workspace);


/*constructor*/
eolWindow* editor_workspace();

#endif
