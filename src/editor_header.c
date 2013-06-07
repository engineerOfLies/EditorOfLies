#include "editor_header.h"
#include "editor_filemenu.h"
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
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    switch (comp->id)
    {
      case 0:
        editor_file_menu_window(header->workspace);
        return eolTrue;
      case 1:
        workspace = editor_get_workspace(header->workspace);
        if (workspace)
        {
          editor_key_edit_window(workspace->level->keys);
        }
        return eolTrue;
    }
  }
  return eolFalse;
}

void editor_header_draw(eolWindow *win)
{
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
