#include "editor_filemenu.h"

#include <eol_dialog.h>
#include <eol_input.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>


/*local global variabls*/

/*local function prototypes*/

/*function definitions*/

eolBool editor_file_menu_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolComponent *comp = NULL;
  if (win == NULL)return eolFalse;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    switch (comp->id)
    {
      case 1:/*new*/
        eol_window_free(&win);
        return eolTrue;
      case 2:/*save*/
        eol_window_free(&win);
        return eolTrue;
      case 3:/*save as*/
        eol_window_free(&win);
        return eolTrue;
      case 4:
        eol_dialog_quit();
        eol_window_free(&win);
        return eolTrue;
    }
  }
  eol_window_free_if_outside_click(&win);
  return eolFalse;
}

void editor_file_menu_draw(eolWindow *win)
{
}

void editor_file_menu_window()
{
  eolWindow *win;
  win = eol_window_load_from_file("ui/editor_file_menu.def");
  if (win == NULL)return;
  win->update = editor_file_menu_update;
  win->draw = editor_file_menu_draw;
}


/*eol@eof*/
