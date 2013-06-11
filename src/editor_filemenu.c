#include "editor_filemenu.h"
#include "editor_workspace.h"

#include <eol_dialog.h>
#include <eol_input.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>

typedef struct
{
  eolWindow *workspace;
  eolLine    filename;
}FileMenuData;

/*local global variabls*/

/*local function prototypes*/

/*function definitions*/

void editor_save_level(eolLine path,eolLine filename,eolLevel *level)
{
  eolText filepath;
  snprintf(filepath,EOLTEXTLEN,"%s/%s",path,filename);
  eol_level_save(filepath,level);
}

void editor_load_level_prompt(eolWindow *self)
{
  /*
  eol_dialog_text_prompt(char *output,
                                  EOLLINELEN,
                                  0,
                                  "Enter File",
                                  self,
                                  eolWindowCallback onOk,
                                  eolWindowCallback onCancel);*/
}

eolBool editor_file_menu_update(eolWindow *win,GList *updates)
{
  GList *c,*l;
  EditorWorkspace * workspace;
  FileMenuData *fmData;
  eolComponent *comp = NULL;
  eolComponent *item = NULL;
  if (win == NULL)return eolFalse;
  fmData = (FileMenuData*)win->customData;
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
          editor_workspace_create_new_level(fmData->workspace);
          eol_window_free(&win);
          return eolTrue;
        }
        if (eol_line_cmp(item->name,"save_as_button")==0)
        {
          eol_window_free(&win);
          return eolTrue;
        }
        if (eol_line_cmp(item->name,"save_button")==0)
        {
          workspace = editor_get_workspace(fmData->workspace);
          if (workspace)
          {
            editor_save_level(workspace->path,workspace->filename,workspace->level);
          }
          else
          {
            eol_logger_message(EOL_LOG_ERROR,"editor_file_menu_update: no workspace data!");
          }
          eol_window_free(&win);
          return eolTrue;
        }
        if (eol_line_cmp(item->name,"load_button")==0)
        {
          editor_workspace_load_level(fmData->workspace,"saves/untitled.lvl");
          eol_window_free(&win);
          return eolTrue;
        }
        if (eol_line_cmp(item->name,"quit_button")==0)
        {
          eol_dialog_quit();
          eol_window_free(&win);
          return eolTrue;
        }
      }
    }
  }
  eol_window_free_if_outside_click(&win);
  return eolFalse;
}

void editor_file_menu_draw(eolWindow *win)
{
}

void editor_file_menu_window(eolWindow *workspace)
{
  FileMenuData *filedata;
  eolWindow *win;
  win = eol_window_load_from_file("ui/editor_file_menu.def");
  if (win == NULL)return;
  win->update = editor_file_menu_update;
  win->draw = editor_file_menu_draw;
  win->customData = malloc(sizeof(FileMenuData));
  if (!win->customData)return;
  filedata = (FileMenuData*)win->customData;
  filedata->workspace = workspace;
}


/*eol@eof*/
