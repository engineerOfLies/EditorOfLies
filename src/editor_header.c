#include <eol_dialog.h>
#include <eol_input.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>

/*local global variabls*/

/*local function prototypes*/


/*function definitions*/

void editor_header_update(eolWindow *win,GList *updates)
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
        fprintf(stdout,"File pressed!\n");
        break;
      case 1:
        fprintf(stdout,"Edit Keys pressed!\n");
        break;
    }
  }
}

void editor_header_draw(eolWindow *win)
{
}

void editor_header_window()
{
  eolWindow *win;
  win = eol_window_load_from_file("ui/file_header.def");
  win->update = editor_header_update;
  win->draw = editor_header_draw;

}


/*eol@eof*/
