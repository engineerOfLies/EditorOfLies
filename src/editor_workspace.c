#include <eol_dialog.h>
#include <eol_input.h>
#include <eol_camera.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>


/*local global variabls*/

/*local function prototypes*/


/*function definitions*/

void editor_workspace_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolComponent *comp = NULL;
  if ((win == NULL)||(updates == NULL))return;
  
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    switch (comp->id)
    {
      case 0:
        break;
      case 1:
        break;
    }
  }
}

void editor_workspace_draw(eolWindow *win)
{
  
}

void editor_workspace()
{
  eolWindow *win;
  win = eol_window_load_from_file("ui/workspace.def");
  win->update = editor_workspace_update;
  win->draw = editor_workspace_draw;
  
}
