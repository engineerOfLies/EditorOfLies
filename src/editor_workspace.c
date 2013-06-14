#include "editor_workspace.h"
#include <eol_dialog.h>
#include <eol_level.h>
#include <eol_entity.h>
#include <eol_input.h>
#include <eol_camera.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>

/*local global variabls*/

/*local function prototypes*/


/*function definitions*/

void editor_workspace_clear_updated(eolWindow *workspace)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  wsData->updated = eolFalse;
}

eolBool editor_workspace_updated(eolWindow *workspace)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return eolFalse;
  return wsData->updated;
}

eolBool editor_workspace_modified(eolWindow *workspace)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return eolFalse;
  return wsData->modified;
}


void editor_workspace_level_modified(eolWindow *workspace)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  wsData->modified = eolTrue;
  wsData->updated = eolTrue;
}

void editor_workspace_save_level(eolWindow *workspace)
{
  EditorWorkspace *wsData;
  eolText filepath;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  snprintf(filepath,EOLTEXTLEN,"%s/%s",wsData->path,wsData->filename);
  eol_level_save(filepath,wsData->level);

  wsData->modified = eolFalse;
}

eolLevel *editor_workspace_new_level()
{
  eolLevel *level = NULL;
  eolLevelLayer *layer = NULL;
  eolBackground *back = NULL;
  level = eol_level_new();
  if (!level)return NULL;
  layer = eol_level_add_layer(level);
  if (!layer)
  {
    eol_level_free(&level);
    return NULL;
  }
  back = eol_level_add_background_to_layer(layer);
  if (!back)
  {
    eol_level_free(&level);
    return NULL;
  }
  eol_word_cpy(level->idName,"test_level");
  eol_word_cpy(layer->idName,"test_layer");
  eol_orientation_clear(&back->ori);
  
  eol_line_cpy(back->modelFile,"models/testlevel.actor");
  
  eol_line_cpy(layer->clipMeshFile,"models/levelmesh/testlevel_mask.obj");
  
  eol_orientation_clear(&layer->clipMaskOri);
  
  eol_rectf_set(&layer->bounds, -10, -10, 20, 40);
  
  eol_level_setup(level);
  eol_level_set_active_layer(level, 0);
  eol_level_set_current_level(level);
  return level;
}

void editor_workspace_create_new_level(eolWindow *workspace)
{
  EditorWorkspace *wsData;
  if (!workspace)return;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  /*close old level*/
  /*TODO: check to see if it has been saved since last edit*/
  wsData = editor_get_workspace(workspace);
  eol_level_free(&wsData->level);
  /*make a new one*/
  wsData->level = editor_workspace_new_level();
  wsData->activeLayer = eol_level_get_layer_n(wsData->level,0);
  wsData->modified = eolFalse;
  wsData->updated = eolTrue;
}

eolLevel * editor_workspace_get_level(eolWindow *workspace)
{
  EditorWorkspace *wsData;
  if (!workspace)return NULL;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return NULL;
  return wsData->level; 
}

void editor_workspace_load_level(eolWindow *workspace,eolLine filename)
{
  EditorWorkspace *wsData;
  eolLevel *loadingLevel;
  if (!workspace)return;
  wsData = editor_get_workspace(workspace);

  eol_level_free(&wsData->level);
  loadingLevel = eol_level_load(filename);
  if (loadingLevel);
  wsData->activeLayer = NULL;
  wsData->level = loadingLevel;
  wsData->modified = eolFalse;
  wsData->updated = eolTrue;
  eol_level_setup(loadingLevel);
  eol_level_set_active_layer(loadingLevel, 0);
  eol_level_set_current_level(loadingLevel);
}

eolLevelLayer *editor_workspace_get_layer(eolWindow *workspace,eolUint layerIndex)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return NULL;
  return eol_level_get_layer_n(wsData->level,layerIndex);
}

void editor_workspace_hide_layer(eolWindow *workspace, eolUint layer, eolBool hide)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  eol_level_hide_layer(wsData->level, layer, hide);
  editor_workspace_level_modified(workspace);
}

void editor_workspace_add_layer(eolWindow *workspace)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  eol_level_add_layer(wsData->level);
  editor_workspace_level_modified(workspace);
}

void editor_workspace_delete_layer(eolWindow *workspace,eolUint layer)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  eol_level_delete_Layer_n(wsData->level,layer);
  editor_workspace_level_modified(workspace);
}


void editor_workspace_select_layer(eolWindow *workspace,eolUint layer)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  wsData->activeLayer = eol_level_set_active_layer(wsData->level, layer);
  editor_workspace_level_modified(workspace);
}

EditorWorkspace *editor_get_workspace(eolWindow *workspace)
{
  if (!workspace)return NULL;
  if (eol_line_cmp(workspace->name,"__workspace__")!=0)return NULL;
  return (EditorWorkspace*)workspace->customData;
}


/*window functions*/
eolBool editor_workspace_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolComponent *comp = NULL;
  eol_camera_update();
  if ((win == NULL)||(updates == NULL))return eolFalse;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    switch (comp->id)
    {
      case 0:
        return eolTrue;
      case 1:
        return eolTrue;
    }
  }
  return eolFalse;
}

void editor_workspace_draw(eolWindow *win)
{
  glPushMatrix();
  
  eol_camera_setup();
  
  eol_level_draw_current();
  
  eol_entity_draw_all();
    
  glPopMatrix();
  
}

void editor_workspace_delete(void *genericdata)
{
  EditorWorkspace *data;
  if (!genericdata)return;
  data = (EditorWorkspace *)genericdata;
  eol_level_free(&data->level);
  memset(data,0,sizeof(EditorWorkspace));
  free(data);
}

eolWindow *editor_workspace()
{
  eolWindow *win;
  EditorWorkspace *data;
  eolConfig *conf;
  win = eol_window_load_from_file("ui/editor_workspace.def");
  if (win == NULL)return NULL;
  win->update = editor_workspace_update;
  win->draw = editor_workspace_draw;
  win->custom_delete = editor_workspace_delete;
  win->customData = malloc(sizeof(EditorWorkspace));
  if (win->customData == NULL)
  {
    eol_logger_message(EOL_LOG_ERROR,"editor_workspace: failed to allocate workspace data\n");
    return NULL;
  }
  memset(win->customData,0,sizeof(EditorWorkspace));
  data = (EditorWorkspace*)win->customData;
  eol_line_cpy(data->filename,"untitled.lvl");
  eol_line_cpy(data->path,"saves");
  conf = eol_config_load("system/editor.cfg");
  if (conf)
  {
    eol_config_get_line_by_tag(data->path,conf,"defaultPath");
  }
  eol_camera_config();
  eol_camera_init();
  return win;
}
