#include "editor_tile_panel.h"
#include "editor_orientation_edit.h"
#include "editor_workspace.h"
#include <eol_mouse.h>
#include <eol_dialog.h>
#include <eol_logger.h>

#define TEM_MODES  3

enum TileEditModes {
  TEM_Paint   = 0,  /**<add and subtract tiles to the map*/
  TEM_Select  = 1,  /**<Select and modify tiles in the map*/
  TEM_Type    = 2   /**<modify data from the tile type*/
};

typedef struct
{
  eolWindow     * workspace;
  eolLine         textBuffer;
  eolWindow     * orientationWindow;
  eolTileType   * selectedTileType;
  GList         * selectedTiles;    /**<selected tiles from the map*/
  eolUint         editingMode;
  eolOrientation  tempOri;   /**<catches orientation changes, so they can be applied to all selected tiles*/
}editorTileData;

editorTileData*editor_tile_get_data(eolWindow *win)
{
  if (!win)return NULL;
  if (eol_line_cmp(win->name,"edit_tile_panel")!=0)return NULL;
  return (editorTileData*)win->customData;
}

void editor_tile_orientation_update(void *data,eolOrientation ori)
{
  eolWindow *win;
  editorTileData*tileData;
  if (!data)return;
  win = (eolWindow*)data;
  tileData = editor_tile_get_data(win);
  if (!tileData)return;
  /*TODO: determine what orientation is alteredd*/
}


eolBool editor_tile_panel_update(eolWindow *win,GList *updates)
{
  eolUint id;
  GList *c,*l;
  editorTileData*tileData;
  eolComponent *comp = NULL,*listItem = NULL;
  if (win == NULL)return eolFalse;
  tileData = editor_tile_get_data(win);
  if (!tileData)return eolFalse;  
  
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    if (eol_line_cmp(comp->name,"mode_button")==0)
    {
      tileData->editingMode = (tileData->editingMode + 1)%TEM_MODES;
      switch(tileData->editingMode)
      {
        case TEM_Paint:
          eol_button_set_text(comp,"Tile Paint Mode");
          break;
        case TEM_Select:
          eol_button_set_text(comp,"Tile Modify Mode");
          break;
        case TEM_Type:
          eol_button_set_text(comp,"Archetype Modify Mode");
          break;
      }
      return eolTrue;
    }
    else if (eol_line_cmp(comp->name,"tile_type_list")==0)
    {
      if (eol_component_list_get_selected_id(&id,comp))
      {
        tileData->selectedTileType = editor_workspace_get_tile_by_id(tileData->workspace,id);
        return eolTrue;
      }
    }
    else if (eol_line_cmp(comp->name,"tile_actions") == 0)
    {
      /*something in the list of buttons*/
      for (l = eol_list_get_updates(comp);l != NULL; l = l->next)
      {
        if (l->data == NULL)continue;
        listItem = (eolComponent *)l->data;
        if (eol_line_cmp(listItem->name,"new_tile_type")==0)
        {
          return eolTrue;
        }
      }
    }
  }

  if (eol_mouse_input_released(eolMouseLeft))
  {
    if (editor_workspace_mouse_click(tileData->workspace))
    {
      switch(tileData->editingMode)
      {
        case TEM_Paint:
          if (!tileData->selectedTileType)
          {
            eol_logger_message(EOL_LOG_ERROR,"No tile to add");
            break;
          }
          editor_workspace_add_mouse_tile(tileData->workspace,tileData->selectedTileType->id);

          return eolTrue;
        default:
          eol_logger_message(EOL_LOG_ERROR,"not in paint mode");
      }
    }
  }
  
  return eolFalse;
}

void editor_tile_workspace_sync(eolWindow *win)
{
  int i;
  editorTileData *tileData;
  eolTileType *tileType;
  eolComponent *list;
  EditorWorkspace *workspace = NULL;
  if (!win)
  {
    eol_logger_message(EOL_LOG_ERROR,"tile panel has no window!");
    return;
  }
  tileData = editor_tile_get_data(win);
  if (!tileData)
  {
    eol_logger_message(EOL_LOG_ERROR,"tile panel has no data!");
    return;
  }

  editor_orientation_update_callback(
    tileData->orientationWindow,
    NULL,
    NULL,
    NULL);
  
  workspace = editor_get_workspace(tileData->workspace);
  if (!workspace)
  {
    eol_logger_message(EOL_LOG_ERROR,"No Workspace to sync to!");
    return;
  }
  
  list = eol_window_get_component_by_name(win,"tile_type_list");
  if (!list)
  {
    eol_logger_message(EOL_LOG_ERROR,"missing tile type list component!");
    return;
  }
  editor_workspace_show_mouse_over_tile(tileData->workspace,eolTrue);
  eol_component_list_clear(list);
  for (i = 0; i < eol_level_get_tile_set_count(workspace->level);i++)
  {
    tileType = eol_level_get_tile_set_nth(workspace->level,i);
    if (!tileType)
    {
      eol_logger_message(EOL_LOG_WARN,"editor_tile_workspace_sync:Failed to get %ith tile!\n",i);
      continue;
    }
    eol_list_add_actor_item(list,tileType->id,tileType->actorFile);
  }
}

void editor_tile_panel_draw(eolWindow *win)
{
  editorTileData*tileData;
  tileData = editor_tile_get_data(win);
  if (!tileData)return;
  if (editor_workspace_updated(tileData->workspace))
  {
    printf("building tile type list\n");
    
    editor_tile_workspace_sync(win);
  }
}

void editor_tile_setup_ori_edit(eolWindow *win,eolWindow *ori_edit)
{
  editorTileData*tileData;
  tileData = editor_tile_get_data(win);
  if (!tileData)return;
  tileData->orientationWindow = ori_edit;
}

eolWindow *editor_tile_panel(eolWindow *workspace)
{
  eolWindow *win;
  editorTileData*tileData;
  win = eol_window_load_from_file("ui/editor_tile_panel.def");
  if (win == NULL)return NULL;
  win->update = editor_tile_panel_update;
  win->draw = editor_tile_panel_draw;
  win->customData = malloc(sizeof(editorTileData));
  if (!win->customData)
  {
    eol_logger_message(EOL_LOG_ERROR,"editor_tile_panel: failed to allocate custom data!");
    eol_window_free(&win);
    return NULL;
  }
  memset(win->customData,0,sizeof(editorTileData));
  tileData = (editorTileData*)win->customData;
  tileData->workspace = workspace;
  return win;
}

/*eol@eof*/
