#include "editor_viewmenu.h"
#include "editor_workspace.h"

#include <eol_dialog.h>
#include <eol_input.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>


/*local global variabls*/

/*local function prototypes*/

/*function definitions*/

eolBool editor_view_menu_update(eolWindow *win,GList *updates)
{
  GList *c;
  GList *l;
  eolComponent *comp = NULL;
  eolComponent *item = NULL;
  if (win == NULL)return eolFalse;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    if (comp->id == 1000)
    {
      for (l = eol_list_get_updates(comp);l != NULL; l = l->next)
      {
        if (l->data == NULL)continue;
        item = (eolComponent *)l->data;
        if (eol_line_cmp(item->name,"new_button")==0)
        {
          return eolTrue;
        }

      }
    }
  }
  eol_window_free_if_outside_click(&win);
  return eolFalse;
}

void editor_view_menu_draw(eolWindow *win)
{
}

void editor_view_menu_window()
{
  eolWindow *win;
  win = eol_window_load_from_file("ui/editor_view_menu.def");
  if (win == NULL)return;
  win->update = editor_view_menu_update;
  win->draw = editor_view_menu_draw;
}


/*eol@eof*/
