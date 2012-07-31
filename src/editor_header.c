#include <eol_dialog.h>
#include <eol_input.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>

/*local global variabls*/

/*local function prototypes*/


/*function definitions*/

eolBool editor_header_update(eolWindow *win,GList *updates)
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

void editor_header_draw(eolWindow *win)
{
}

void editor_header_window()
{
  eolWindow *win;
  win = eol_window_load_from_file("ui/editor_header.def");
  if (win == NULL)return;
  win->update = editor_header_update;
  win->draw = editor_header_draw;

}


/*eol@eof*/
