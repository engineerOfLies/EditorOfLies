#include "editor_panel.h"
#include "editor_layer_panel.h"
#include "editor_background_panel.h"

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
  eolWindow * childwindow[eolPanelModeMax];
  eolWindow * workspace;
}eolEditorPanelData;
/*local global variabls*/

/*local function prototypes*/


/*function definitions*/

void editor_panel_show(eolEditorPanelData *data,eolUint panel)
{
  if (panel >= eolPanelModeMax)return;
  if (!data)return;
  eol_window_show(data->childwindow[panel]);
  eol_window_wakeup(data->childwindow[panel]);
}
void editor_panel_hide_all(eolEditorPanelData *data)
{
  int i;
  if (!data)return;
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
  eolComponent *labelComp = NULL;
  eolComponent *list = NULL;
  eolComponent *comp = NULL;
  eolEditorPanelData *data = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  data = (eolEditorPanelData*)win->customData;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    list = (eolComponent *)c->data;
    labelComp = eol_window_get_component_by_id(win,10);
    if (list->id == 1000)
    {
      for (l = eol_list_get_updates(list);l != NULL; l = l->next)
      {
        if (l->data == NULL)continue;
        comp = (eolComponent *)l->data;
        if (eol_line_cmp(comp->name,"layers_button")==0)
        {
          editor_panel_hide_all(data);
          editor_panel_show(data,eolPanelModeLayer);
          eol_label_set_text(labelComp,"Layer Editor");
          editor_layer_panel_workspace_sync(data->childwindow[eolPanelModeLayer]);
          return eolTrue;
        }
        if (eol_line_cmp(comp->name,"bg_button")==0)
        {
          editor_panel_hide_all(data);
          editor_panel_show(data,eolPanelModeBackground);
          eol_label_set_text(labelComp,"Background Editor");
          editor_background_workspace_sync(data->childwindow[eolPanelModeBackground]);
          return eolTrue;
        }
        if (eol_line_cmp(comp->name,"tile_button")==0)
        {
          editor_panel_hide_all(data);
          editor_panel_show(data,eolPanelModeTile);
          eol_label_set_text(labelComp,"Tile Editor");
          return eolTrue;
        }
        if (eol_line_cmp(comp->name,"mask_button")==0)
        {
          editor_panel_hide_all(data);
          editor_panel_show(data,eolPanelModeMask);
          eol_label_set_text(labelComp,"Mask Editor");
          return eolTrue;
        }
        if (eol_line_cmp(comp->name,"spawn_button")==0)
        {
          editor_panel_hide_all(data);
          editor_panel_show(data,eolPanelModeSpawn);
          eol_label_set_text(labelComp,"Spawn Editor");
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
  data->workspace = workspace;
}

/*eol@eof*/
