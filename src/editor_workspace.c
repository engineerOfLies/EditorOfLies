#include "editor_workspace.h"
#include <eol_dialog.h>
#include <eol_level.h>
#include <eol_draw.h>
#include <eol_3d_op.h>
#include <eol_entity.h>
#include <eol_input.h>
#include <eol_mouse.h>
#include <eol_camera.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>

/*local global variabls*/

eolLine _defaultTiles = "";
/*local function prototypes*/


/*function definitions*/

void editor_workspace_show_mouse_over_tile(eolWindow *workspace,eolBool show)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  wsData->drawMouseTile = show;
}


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
  eol_logger_message(EOL_LOG_WARN,"editor_workspace_save_level: saving level:\n%s",filepath);
  eol_level_save(filepath,wsData->level);

  wsData->modified = eolFalse;
}

eolLevel *editor_workspace_new_level()
{
  eolConfig *conf;
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
  eol_word_cpy(level->idName,"test_level");
  eol_word_cpy(layer->idName,"test_layer");

  
  back = eol_level_add_background_to_layer(layer);
  if (!back)
  {
    eol_level_free(&level);
    return NULL;
  }
  eol_orientation_clear(&back->ori);
  eol_line_cpy(back->modelFile,"models/testlevel.actor");

  back = eol_level_add_background_to_layer(layer);
  if (!back)
  {
    eol_level_free(&level);
    return NULL;
  }
  eol_orientation_clear(&back->ori);
  back->useAsClip = eolTrue;
  eol_line_cpy(back->modelFile,"models/testclip.actor");
  
  eol_rectf_set(&layer->bounds, -10, -10, 20, 40);

  conf = eol_config_load("system/editor.cfg");
  if (conf)
  {
    if (eol_config_get_line_by_tag(_defaultTiles,conf,"defaultTiles"))
    {
      eol_level_load_tile_set(level,_defaultTiles);
    }
    eol_config_free(&conf);
  }

  layer->tileMap = eol_tile_map_new();
  
  eol_level_set_active_layer(level, 0);
  eol_level_setup(level);
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

eolBool editor_workspace_mouse_click(eolWindow *workspace)
{
  return eol_window_mouse_inside(workspace);
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
  eol_logger_message(EOL_LOG_WARN,"editor_workspace_load_level: loading level:\n%s",filename);
  
  loadingLevel = eol_level_load(filename);
  if (loadingLevel);
  wsData->activeLayer = NULL;
  wsData->level = loadingLevel;
  wsData->modified = eolFalse;
  wsData->updated = eolTrue;
  eol_level_setup(loadingLevel);
  wsData->activeLayer = eol_level_get_layer_n(loadingLevel,loadingLevel->active);
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

void editor_workspace_move_layer_up(eolWindow *workspace,eolUint layer)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  eol_level_move_layer_n_up(wsData->level,layer);
  editor_workspace_level_modified(workspace);
}

void editor_workspace_move_layer_down(eolWindow *workspace,eolUint layer)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  eol_level_move_layer_n_down(wsData->level,layer);
  editor_workspace_level_modified(workspace);
}

void editor_workspace_rename_layer(eolWindow *workspace,eolUint layer,eolLine newName)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  eol_level_rename_layer_n(wsData->level,layer, newName);
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

void editor_workspace_hide_background(eolWindow *workspace,eolUint backgroundIndex,eolBool hide)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if ((!wsData)||(!wsData->activeLayer))return;
  eol_level_hide_background(wsData->activeLayer,backgroundIndex,hide);
  editor_workspace_level_modified(workspace);
}

void editor_workspace_add_background(eolWindow *workspace,eolLine modelFile)
{
  eolModel *model;
  eolBackground *background;
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return;
  if (!wsData->activeLayer)return;
  model = eol_model_load(modelFile);
  if (!model)return;
  background = eol_level_add_background_to_layer(wsData->activeLayer);
  if (!background)
  {
    eol_model_free(&model);
    return;
  }
  background->model = model;
  eol_line_cpy(background->modelFile,modelFile);
  editor_workspace_level_modified(workspace);
}

void editor_workspace_move_background_down(eolWindow *workspace,eolUint backgroundIndex)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if ((!wsData)||(!wsData->activeLayer))return;
  eol_level_move_background_down(wsData->activeLayer,backgroundIndex);
  editor_workspace_level_modified(workspace);
}

void editor_workspace_move_background_up(eolWindow *workspace,eolUint backgroundIndex)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if ((!wsData)||(!wsData->activeLayer))return;
  eol_level_move_background_up(wsData->activeLayer,backgroundIndex);
  editor_workspace_level_modified(workspace);
}

void editor_workspace_delete_background(eolWindow *workspace,eolUint backgroundIndex)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if ((!wsData)||(!wsData->activeLayer))return;
  eol_level_delete_background(wsData->activeLayer,backgroundIndex);
  editor_workspace_level_modified(workspace);
}

eolBackground *editor_workspace_get_background(eolWindow *workspace,eolUint index)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if ((!wsData)||(!wsData->activeLayer))return NULL;
  return eol_level_get_background(wsData->activeLayer,index);
}

/* tiles */

eolTileType *editor_workspace_get_tile_by_id(eolWindow *workspace,eolUint typeId)
{
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if (!wsData)return NULL;
  return eol_level_get_tile_set_by_id(wsData->level,typeId);
}

void editor_workspace_add_mouse_tile(eolWindow *workspace,eolUint tileIndex)
{
  eolUint x,y;
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(workspace);
  if ((!wsData)||(!wsData->activeLayer))return;
  if (wsData->activeLayer->tileMap == NULL)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "editor_workspace_add_mouse_tile: no tile map ");
    return;
  }
  if (!eol_tile_get_tilexy_by_mouse(wsData->activeLayer->tileMap, wsData->activeLayer->ori, &x, &y))
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "editor_workspace_add_mouse_tile: not adding tile, mouse not in map");
    return;
  }
  if (!eol_tile_add_to_map(wsData->activeLayer->tileMap,x, y,tileIndex))
  {
    eol_logger_message(
      EOL_LOG_INFO,
      "editor_workspace_add_mouse_tile: did not add a tile"
    );
  }
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

void editor_workspace_draw_mouse_tile(eolWindow *win)
{
  eolOrientation ori;
  eolVec3D mousePosition;
  eolQuad3D quad;
  eolRectFloat tileRect;
  EditorWorkspace *wsData;
  wsData = editor_get_workspace(win);
  if ((!wsData)||(!wsData->activeLayer))return;
  if (!wsData->drawMouseTile)return;

  eol_orientation_copy(&ori,wsData->activeLayer->ori);

  ori.color.x *= 1; 
  ori.color.y *= 0;
  ori.color.z *= 1;
  
  quad = eol_tile_map_get_bounding_quad(wsData->activeLayer->tileMap);

  /*apply bounding quad orientation*/
  eol_3d_op_transform_quad_by_ori(&quad, quad,ori);

  if (!eol_mouse_get_quad3d_intersect(&mousePosition,quad))
  {
    return;
  }
  
  eol_draw_dot_3D(mousePosition,
                  8,
                  eol_vec3d(1,0,1),
                  1);

  eol_3d_op_transform_vec3d_inverse_by_ori(&mousePosition, mousePosition,ori);
  
  if (!eol_tile_map_get_tile_rect(wsData->activeLayer->tileMap,&tileRect,eol_vec2d(mousePosition.x,mousePosition.y)))
  {
    return;
  }
  
  eol_draw_rect_3D(tileRect, 2, ori);
}

void editor_workspace_draw(eolWindow *win)
{
  glPushMatrix();
  
  eol_camera_setup();
  
  eol_level_draw_current();
  
  editor_workspace_draw_mouse_tile(win);
  
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
    eol_config_get_vec3d_by_tag(&data->selectColor,conf,"selectColor");
    eol_config_get_vec3d_by_tag(&data->mouseOverColor,conf,"mouseOverColor");
    eol_config_free(&conf);
  }
  eol_level_enable_tile_grid_draw(eolFalse);
  eol_camera_config();
  eol_camera_init();
  return win;
}
