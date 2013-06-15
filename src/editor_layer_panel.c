#include "editor_layer_panel.h"
#include "editor_workspace.h"
#include <eol_dialog.h>
#include <eol_logger.h>

typedef struct
{
  eolWindow *workspace;
  eolLine textBuffer;
}editorLayerData;

editorLayerData *editor_layer_get_data(eolWindow *win)
{
  if (!win)return NULL;
  if (eol_line_cmp(win->name,"edit_layer_panel")!=0)return NULL;/*duck typing!*/
  return (editorLayerData*)win->customData;
}

void editor_layer_delete(void *data)
{
  eolWindow *win;
  eolComponent *comp = NULL;
  eolUint layerIndex = 0;
  editorLayerData *layerData;
  if (!data)return;
  win = (eolWindow*)data;
  layerData = editor_layer_get_data(win);
  comp = eol_window_get_component_by_name(win,"layer_list");
  if(eol_component_list_get_selected_id(&layerIndex,comp))
  {
    editor_workspace_delete_layer(layerData->workspace,layerIndex);
  }
}

void editor_layer_rename(void *data)
{
  eolWindow *win;
  eolComponent *comp = NULL;
  editorLayerData *layerData;
  eolUint layerIndex = 0;
  if (!data)return;
  win = (eolWindow*)data;
  layerData = editor_layer_get_data(win);
  if (!layerData)return;
  comp = eol_window_get_component_by_name(win,"layer_list");
  if(eol_component_list_get_selected_id(&layerIndex,comp))
  {
    editor_workspace_rename_layer(layerData->workspace,layerIndex,layerData->textBuffer);
  }
}


eolBool editor_layer_panel_update(eolWindow *win,GList *updates)
{
  eolUint index;
  GList *c,*l;
  eolLine buttonText;
  EditorWorkspace *workspace = NULL;
  editorLayerData *layerData;
  eolLevelLayer *layer;
  eolComponent *comp = NULL,*listItem = NULL;
  eolComponent *hideButton = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  layerData = editor_layer_get_data(win);
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    if (eol_line_cmp(comp->name,"layer_list")==0)
    {
      hideButton = eol_window_get_component_by_name(win,"layer_actions");      
      hideButton = eol_component_list_get_item_by_name(hideButton,"hide_layer");
      if (!hideButton)continue;
      if(eol_component_list_get_selected_id(&index,comp))
      {
        layer = editor_workspace_get_layer(layerData->workspace,index);
        if ((!layer)||(!layer->hidden))
        {
          eol_button_set_text(hideButton,"Hide Layer");
        }
        else
        {
          eol_button_set_text(hideButton,"Unhide Layer");
        }
      }
      else
      {
        eol_button_set_text(hideButton,"Hide Layer");
      }
      continue;
    }
    if (eol_line_cmp(comp->name,"layer_actions") == 0)
    {
      /*something in the list of buttons*/
      for (l = eol_list_get_updates(comp);l != NULL; l = l->next)
      {
        if (l->data == NULL)continue;
        listItem = (eolComponent *)l->data;
        if (eol_line_cmp(listItem->name,"new_layer")==0)
        {
          workspace = editor_get_workspace(layerData->workspace);
          if (workspace)
          {
            if (workspace->level != NULL)
            {
              editor_workspace_add_layer(layerData->workspace);
            }
            else
            {
              eol_dialog_message("Warning","No Open Level to add a layer to!");
            }
          }
          return eolTrue;          
        }
        if (eol_line_cmp(listItem->name,"delete_layer")==0)
        {
          comp = eol_window_get_component_by_name(win,"layer_list");
          if(eol_component_list_get_selected_id(&index,comp))
          {
            eol_dialog_yes_no("Delete Selected Layer?",
                              win,
                              editor_layer_delete,
                              NULL);
          }
          else
          {
            eol_dialog_message("Warning","No Selected Layer to Delete.");
          }
          return eolTrue;
        }
        if (eol_line_cmp(listItem->name,"hide_layer")==0)
        {
          comp = eol_window_get_component_by_name(win,"layer_list");
          if(eol_component_list_get_selected_id(&index,comp))
          {
            eol_button_get_text(buttonText,listItem);
            if (eol_line_cmp(buttonText,"Hide Layer")==0)
            {
              editor_workspace_hide_layer(layerData->workspace, index, eolTrue);
            }
            else if (eol_line_cmp(buttonText,"Unhide Layer")==0)
            {
              editor_workspace_hide_layer(layerData->workspace, index, eolFalse);
            }
          }
          else
          {
            eol_dialog_message("Warning","No Selected Layer to Hide.");
          }
          return eolTrue;
        }
        if (eol_line_cmp(listItem->name,"make_active")==0)
        {
          comp = eol_window_get_component_by_name(win,"layer_list");
          if(eol_component_list_get_selected_id(&index,comp))
          {
            editor_workspace_select_layer(layerData->workspace,index);
          }
          else
          {
            eol_dialog_message("Warning","No Selected Layer to Make Active.");
          }
          return eolTrue;
        }
        if (eol_line_cmp(listItem->name,"move_layer_up")==0)
        {
          comp = eol_window_get_component_by_name(win,"layer_list");
          if(eol_component_list_get_selected_id(&index,comp))
          {
            editor_workspace_move_layer_up(layerData->workspace,index);
          }
          else
          {
            eol_dialog_message("Warning","No Selected Layer to move up.");
          }
          return eolTrue;
        }
        if (eol_line_cmp(listItem->name,"move_layer_down")==0)
        {
          comp = eol_window_get_component_by_name(win,"layer_list");
          if(eol_component_list_get_selected_id(&index,comp))
          {
            editor_workspace_move_layer_down(layerData->workspace,index);
          }
          else
          {
            eol_dialog_message("Warning","No Selected Layer to move down.");
          }
          return eolTrue;
        }
        if (eol_line_cmp(listItem->name,"rename_layer")==0)
        {
          comp = eol_window_get_component_by_name(win,"layer_list");
          if(eol_component_list_get_selected_id(&index,comp))
          {
            eol_dialog_text_prompt(layerData->textBuffer,
                                  EOLLINELEN,
                                  EOLLINELEN,  /*0 means no limit*/
                                  "Enter New Layer Name:",
                                  win,
                                  editor_layer_rename,
                                  NULL);
          }
          else
          {
            eol_dialog_message("Warning","No Selected Layer to Rename.");
          }
          return eolTrue;
        }
      }
    }
  }
  return eolFalse;
}

void editor_layer_panel_workspace_sync(eolWindow *win)
{
  int i = 0;
  GList *l;
  eolComponent *list;
  eolLevelLayer *layer;
  EditorWorkspace *workspace = NULL;
  editorLayerData *layerData;
  layerData = editor_layer_get_data(win);
  if (!layerData)return;
  workspace = editor_get_workspace(layerData->workspace);
  list = eol_window_get_component_by_name(win,"layer_list");
  if ((list)
    &&(workspace)
    &&(workspace->level))
  {
    eol_component_list_clear(list);
    for (l = workspace->level->layers;l != NULL; l = l->next)
    {
      if (!l->data)continue;
      layer = (eolLevelLayer*)l->data;
      eol_list_add_text_item(list,i++,layer->idName);
    }
  }
}

void editor_layer_panel_draw(eolWindow *win)
{
  editorLayerData *layerData;
  layerData = editor_layer_get_data(win);
  if (!layerData)return;
  if (editor_workspace_updated(layerData->workspace))
  {
    editor_layer_panel_workspace_sync(win);
  }
}

eolWindow *editor_layer_panel(eolWindow *workspace)
{
  eolWindow *win;
  editorLayerData *layerData;
  win = eol_window_load_from_file("ui/editor_layer_panel.def");
  if (win == NULL)return NULL;
  win->update = editor_layer_panel_update;
  win->draw = editor_layer_panel_draw;
  win->customData = malloc(sizeof(editorLayerData));
  if (!win->customData)
  {
    eol_logger_message(EOL_LOG_ERROR,"editor_layer_panel: failed to allocate custom data!");
    eol_window_free(&win);
    return NULL;
  }
  memset(win->customData,0,sizeof(editorLayerData));
  layerData = (editorLayerData*)win->customData;
  layerData->workspace = workspace;
  return win;
}

/*eol@eof*/
