#include "editor_keydialog.h"

#include <eol_dialog.h>
#include <eol_input.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>

typedef struct
{
  void *clientData;
  void (*callback)(void *data,eolLine Key, eolLine Value);
}KeyDialogData;

/*local global variabls*/

/*local function prototypes*/

/*function definitions*/

eolBool editor_keydialog_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolComponent *comp = NULL;
  KeyDialogData *winData;
  eolLine key,value;
  if (win == NULL)return eolFalse;
  winData = (KeyDialogData*)win->customData;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    if (eol_line_cmp(comp->name,"cancel")==0)
    {
        eol_window_free(&win);
        return eolTrue;
    }
    if (eol_line_cmp(comp->name,"done")==0)
    {
      /*get key and value entry values*/
        eol_entry_get_line(eol_window_get_component_by_name(win,"keyEntry"),key);
        eol_entry_get_line(eol_window_get_component_by_name(win,"valueEntry"),value);
        if (winData)
        {
          if (winData->callback)
          {
            winData->callback(winData->clientData,key,value);
          }
        }
        eol_window_free(&win);
        return eolTrue;
    }
  }
  eol_window_free_if_outside_click(&win);
  return eolFalse;
}

void editor_keydialog_draw(eolWindow *win)
{
}

void editor_key_dialog_delete(void *genericdata)
{
  KeyDialogData *winData;
  if (!genericdata)return;
  winData = (KeyDialogData *)genericdata;
  free(winData);
}

void editor_key_dialog(eolLine Title, void *data,void (*callback)(void *data,eolLine Key, eolLine Value))
{
  eolWindow *win;
  KeyDialogData *winData;
  win = eol_window_load_from_file("ui/editor_key_entry.def");
  if (win == NULL)return;
  win->update = editor_keydialog_update;
  win->draw = editor_keydialog_draw;
  win->custom_delete = editor_key_dialog_delete;
  win->customData = malloc(sizeof(KeyDialogData));
  if (!win->customData)
  {
    eol_logger_message(EOL_LOG_WARN,"editor_key_dialog: failed to allocate custom Data\n");
    return;
  }
  winData = (KeyDialogData*)win->customData;
  winData->clientData = data;
  winData->callback = callback;
}


/*eol@eof*/
