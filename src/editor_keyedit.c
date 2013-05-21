#include "editor_keyedit.h"

#include <eol_dialog.h>
#include <eol_input.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>


/*local global variabls*/

/*local function prototypes*/

/*function definitions*/

eolBool editor_keyedit_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolComponent *comp = NULL;
  if (win == NULL)return eolFalse;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    if (eol_line_cmp(comp->name,"done")==0)
    {
        eol_window_free(&win);
        return eolTrue;
    }
  }
  eol_window_free_if_outside_click(&win);
  return eolFalse;
}

void editor_keyedit_draw(eolWindow *win)
{
}

void editor_key_edit_window()
{
  eolWindow *win;
  win = eol_window_load_from_file("ui/editor_key_edit.def");
  if (win == NULL)return;
  win->update = editor_keyedit_update;
  win->draw = editor_keyedit_draw;
}


/*eol@eof*/
