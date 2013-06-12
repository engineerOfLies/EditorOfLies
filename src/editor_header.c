#include "editor_header.h"
#include "editor_filemenu.h"
#include "editor_viewmenu.h"
#include "editor_keyedit.h"
#include "editor_workspace.h"

#include <eol_dialog.h>
#include <eol_input.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>

typedef struct
{
  eolWindow *workspace;
}editorHeaderData;

/*local global variabls*/

/*local function prototypes*/
void editor_on_title_change(void *data)
{
  eolWindow *win;
  editorHeaderData *header;
  EditorWorkspace *workspace;
  eolComponent *labelComp = NULL;
  
  if (!data)return;
  
  win = (eolWindow *)data;
  header = (editorHeaderData *)win->customData;
  if (!header)return;
  
  workspace = editor_get_workspace(header->workspace);
  if (!workspace)return;
  
  labelComp = eol_window_get_component_by_id(win,10);
  if (!labelComp)return;
  
  eol_label_set_text(labelComp,workspace->level->idName);
}

/*function definitions*/

eolBool editor_header_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolComponent *comp = NULL;
  EditorWorkspace *workspace = NULL;
  editorHeaderData *header = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  header = (editorHeaderData *)win->customData;
  workspace = editor_get_workspace(header->workspace);


  /*handle input updates*/
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    if (eol_line_cmp(comp->name,"file_button")==0)
    {
        editor_file_menu_window(header->workspace);
        return eolTrue;
    }
    if (eol_line_cmp(comp->name,"keys_button")==0)
    {
        workspace = editor_get_workspace(header->workspace);
        if (workspace)
        {
          if (workspace->level != NULL)
          {
            editor_key_edit_window(workspace->level->keys);
          }
          else
          {
            eol_dialog_message("Warning","No Open Level to Edit Keys for.");
          }
        }
        return eolTrue;
    }
    if (eol_line_cmp(comp->name,"view_button")==0)
    {
        editor_view_menu_window(header->workspace);
        return eolTrue;
    }
  }
  return eolFalse;
}

void editor_header_draw(eolWindow *win)
{
  eolComponent *labelComp = NULL;
  EditorWorkspace *workspace = NULL;
  editorHeaderData *header = NULL;
  if (win == NULL)return;
  header = (editorHeaderData *)win->customData;
  workspace = editor_get_workspace(header->workspace);
  /*maintenance updates*/
  if ((workspace != NULL)
    && (editor_workspace_updated(header->workspace)))
  {
    if (workspace->activeLayer)
    {
      labelComp = eol_window_get_component_by_name(win,"layer_name");
      eol_label_set_text(labelComp,workspace->activeLayer->idName);
    }
    if (workspace->level)
    {
      labelComp = eol_window_get_component_by_name(win,"level_name");
      eol_label_set_text(labelComp,workspace->level->idName);
    }
  }
}

void editor_header_window(eolWindow *workspace)
{
  eolWindow *win;
  editorHeaderData * headerData;
  win = eol_window_load_from_file("ui/editor_header.def");
  if (win == NULL)return;
  win->update = editor_header_update;
  win->draw = editor_header_draw;
  win->customData = malloc(sizeof(editorHeaderData));
  memset(win->customData,0,sizeof(editorHeaderData));
  headerData = (editorHeaderData*)win->customData;
  headerData->workspace = workspace;
}


/*eol@eof*/
