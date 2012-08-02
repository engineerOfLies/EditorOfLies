/*
Copyright 2012 Engineer of Lies
This file is part of the Engine of Lies game engine library

The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

EOL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.
*/
/*editor includes*/
#include "editor_header.h"
#include "editor_workspace.h"
#include "editor_panel.h"

/*eol includes*/
#include <eol.h>
#include <eol_graphics.h>
#include <eol_input.h>
#include <eol_window.h>
#include <eol_mouse.h>
#include <eol_level.h>
#include <eol_entity.h>
#include <eol_dialog.h>
#include <eol_lighting.h>
#include <string.h>
#include <stdio.h>

void Init_All(const char *argv);

void MakeTestWindow();

int main(int argc, char *argv[])
{
  int done;
  int i;
  for(i = 1;i < argc;i++)
  {
    if(strcmp(argv[i],"-fs")== 0)
    {
    }
    if(strcmp(argv[i],"-wm")== 0)
    {
    }
  }
  Init_All(argv[0]);
  done = 0;
  eol_mouse_show();
  eol_lighting_setup_rep_plot();
  editor_workspace();
  editor_panel_window();
  editor_header_window();
  do
  {
    eol_input_update();
    eol_mouse_update();

    eol_window_update_all();

    eol_graphics_frame_begin();
    eol_window_draw_all();

    eol_mouse_draw();
    eol_graphics_frame_end();

    if((eol_input_quit_check()) ||
      ((eol_input_is_key_released(SDLK_F4)) && (eol_input_is_mod_held(KMOD_ALT))) ||
      ((eol_input_is_key_released(SDLK_q)) && (eol_input_is_mod_held(KMOD_ALT))))
    {
      eol_dialog_quit();
    }
  }while(!done);
  exit(0);
  return 0;
}

void Init_All(const char *argv)
{
  eol_init(EOL_ALL);
  eol_entity_config();
  eol_entity_init();
  eol_level_config();
  eol_level_init();
}

/*eol @ eof*/
