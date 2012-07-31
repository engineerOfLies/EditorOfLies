#include "editor_panel.h"

#include <eol_dialog.h>
#include <eol_logger.h>
#include <eol_window.h>

/*local global variabls*/

/*local function prototypes*/


/*function definitions*/

eolBool editor_panel_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolComponent *comp = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    switch (comp->id)
    {
      case 0:
        fprintf(stdout,"File pressed!\n");
        return eolTrue;
      case 1:
        fprintf(stdout,"Edit Keys pressed!\n");
        return eolTrue;
    }
  }
  return eolFalse;
}

void editor_panel_draw(eolWindow *win)
{
}

void editor_panel_window()
{
  eolWindow *win;
  win = eol_window_load_from_file("ui/editor_panel.def");
  win->update = editor_panel_update;
  win->draw = editor_panel_draw;
  
}

/*eol@eof*/
