#include "editor_panel.h"
#include "editor_layer_panel.h"

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
  EditorLevelData *levelData;
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
  eolComponent *labelComp = NULL;
  eolComponent *comp = NULL;
  eolEditorPanelData *data = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  data = (eolEditorPanelData*)win->customData;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    labelComp = eol_window_get_component_by_id(win,10);
    switch (comp->id)
    {
      case 1:
        editor_panel_hide_all(data);
        editor_panel_show(data,eolPanelModeLayer);
        eol_label_set_text(labelComp,"Layer Editor");
        return eolTrue;
      case 2:
        editor_panel_hide_all(data);
        editor_panel_show(data,eolPanelModeBackground);
        eol_label_set_text(labelComp,"Background Editor");
        return eolTrue;
      case 3:
        editor_panel_hide_all(data);
        editor_panel_show(data,eolPanelModeTile);
        eol_label_set_text(labelComp,"Panel Editor");
        return eolTrue;
      case 4:
        editor_panel_hide_all(data);
        editor_panel_show(data,eolPanelModeMask);
        eol_label_set_text(labelComp,"Mask Editor");
        return eolTrue;
      case 5:
        editor_panel_hide_all(data);
        editor_panel_show(data,eolPanelModeSpawn);
        eol_label_set_text(labelComp,"Spawn Editor");
        return eolTrue;
    }
  }
  return eolFalse;
}

void editor_panel_draw(eolWindow *win)
{
}

void editor_panel_window(EditorLevelData *levelData)
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
  data->childwindow[eolPanelModeLayer] = editor_layer_panel();
  data->levelData = levelData;
}

/*eol@eof*/
