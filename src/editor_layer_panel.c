#include "editor_layer_panel.h"

eolBool editor_layer_panel_update(eolWindow *win,GList *updates)
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
        return eolTrue;
    }
  }
  return eolFalse;
}


void editor_layer_panel_draw(eolWindow *win)
{
}

eolWindow *editor_layer_panel()
{
  eolWindow *win;
  win = eol_window_load_from_file("ui/editor_layer_panel.def");
  if (win == NULL)return NULL;
  win->update = editor_layer_panel_update;
  win->draw = editor_layer_panel_draw;
  return win;
}

/*eol@eof*/
