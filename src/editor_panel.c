#include "editor_panel.h"
#include "editor_layer_panel.h"
#include "editor_background_panel.h"
#include "editor_orientation_edit.h"
#include "editor_tile_panel.h"
#include "editor_workspace.h"

#include <eol_dialog.h>
#include <eol_logger.h>
#include <eol_window.h>

enum eolEditorPanelModes {
  eolPanelModeLayer       = 0,
  eolPanelModeBackground  = 1,
  eolPanelModeMask        = 2,
  eolPanelModeTile        = 3,
  eolPanelModeSpawn       = 4
};
#define eolPanelModeMax     5

typedef struct
{
  eolLine     title;
  eolUint     mode;
  eolComponent *titleLabel;
  eolWindow * childwindow[eolPanelModeMax];
  eolWindow * workspace;
  eolWindow * orientationEditor;
}eolEditorPanelData;
/*local global variabls*/

/*local function prototypes*/
void editor_panel_hide_all(eolEditorPanelData *data);


/*function definitions*/

void editor_panel_show(eolEditorPanelData *data,eolUint panel)
{
  if (panel >= eolPanelModeMax)return;
  if (!data)return;
  editor_panel_hide_all(data);
  eol_window_show(data->childwindow[panel]);
  eol_window_wakeup(data->childwindow[panel]);
  switch(panel)
  {
    case eolPanelModeLayer:
      eol_label_set_text(data->titleLabel,"Layer Editor");
      editor_layer_panel_workspace_sync(data->childwindow[eolPanelModeLayer]);
      break;
    case eolPanelModeBackground:
      eol_label_set_text(data->titleLabel,"Background Editor");
      editor_background_workspace_sync(data->childwindow[eolPanelModeBackground]);
      break;
    case eolPanelModeTile:
      eol_label_set_text(data->titleLabel,"Tile Editor");
      editor_tile_workspace_sync(data->childwindow[eolPanelModeTile]);
      break;
    case eolPanelModeMask:
      eol_label_set_text(data->titleLabel,"Mask Editor");
      break;
    case eolPanelModeSpawn:
      eol_label_set_text(data->titleLabel,"Spawn Editor");
      break;
  }
}

void editor_panel_hide_all(eolEditorPanelData *data)
{
  int i;
  if (!data)return;
  editor_workspace_show_mouse_over_tile(data->workspace,eolFalse);
  for (i = 0; i < eolPanelModeMax;i++)
  {
    eol_window_hide(data->childwindow[i]);
    eol_window_sleep(data->childwindow[i]);
  }
}

eolBool editor_panel_update(eolWindow *win,GList *updates)
{
  GList *c;
  GList *l;
  eolComponent *list = NULL;
  eolComponent *comp = NULL;
  eolEditorPanelData *data = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  data = (eolEditorPanelData*)win->customData;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    list = (eolComponent *)c->data;
    if (list->id == 1000)
    {
      for (l = eol_list_get_updates(list);l != NULL; l = l->next)
      {
        if (l->data == NULL)continue;
        comp = (eolComponent *)l->data;
        if (eol_line_cmp(comp->name,"layers_button")==0)
        {
          editor_panel_show(data,eolPanelModeLayer);
          return eolTrue;
        }
        if (eol_line_cmp(comp->name,"bg_button")==0)
        {
          editor_panel_show(data,eolPanelModeBackground);
          return eolTrue;
        }
        if (eol_line_cmp(comp->name,"tile_button")==0)
        {
          editor_panel_show(data,eolPanelModeTile);
          return eolTrue;
        }
        if (eol_line_cmp(comp->name,"mask_button")==0)
        {
          editor_panel_show(data,eolPanelModeMask);
          return eolTrue;
        }
        if (eol_line_cmp(comp->name,"spawn_button")==0)
        {
          editor_panel_show(data,eolPanelModeSpawn);
          return eolTrue;
        }
      }
    }
  }
  return eolFalse;
}

void editor_panel_draw(eolWindow *win)
{
}

void editor_panel_window(eolWindow *workspace)
{
  eolWindow *win;
  eolEditorPanelData *data = NULL;
  win = eol_window_load_from_file("ui/editor_panel.def");
  if (win == NULL)return;
  win->update = editor_panel_update;
  win->draw = editor_panel_draw;
  
  win->customData = malloc(sizeof(eolEditorPanelData));
  if (win->customData == NULL)return;
  memset(win->customData,0,sizeof(eolEditorPanelData));
  data = (eolEditorPanelData*)win->customData;
  
  data->childwindow[eolPanelModeLayer] = editor_layer_panel(workspace);
  data->childwindow[eolPanelModeBackground] = editor_background_panel(workspace);
  data->childwindow[eolPanelModeTile] = editor_tile_panel(workspace);
  
  data->workspace = workspace;
  data->orientationEditor = editor_orientation_edit();
  data->titleLabel = eol_window_get_component_by_name(win,"title");
  
  editor_layer_setup_ori_edit(data->childwindow[eolPanelModeLayer],data->orientationEditor );
  editor_background_setup_ori_edit(data->childwindow[eolPanelModeBackground],data->orientationEditor );
  editor_tile_setup_ori_edit(data->childwindow[eolPanelModeTile],data->orientationEditor );
  
  editor_panel_show(data,eolPanelModeLayer);
}

/*eol@eof*/
