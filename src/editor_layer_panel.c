#include <eol_dialog.h>
#include "editor_layer_panel.h"
#include "editor_workspace.h"

void editor_layer_delete(void *data)
{
  eolWindow *win;
  eolComponent *comp = NULL;
  eolUint layerIndex = 0;
  if (!data)return;
  win = (eolWindow*)data;
  comp = eol_window_get_component_by_name(win,"layer_list");
  if(eol_component_list_get_selected_id(&layerIndex,comp))
  {
    editor_workspace_delete_layer(win->customData,layerIndex);
  }
}

eolBool editor_layer_panel_update(eolWindow *win,GList *updates)
{
  eolUint index;
  GList *c,*l;
  EditorWorkspace *workspace = NULL;
  eolComponent *comp = NULL,*listItem = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    if (eol_line_cmp(comp->name,"layer_actions") == 0)
    {
      /*something in the list of buttons*/
      for (l = eol_list_get_updates(comp);l != NULL; l = l->next)
      {
        if (l->data == NULL)continue;
        listItem = (eolComponent *)l->data;
        if (eol_line_cmp(listItem->name,"new_layer")==0)
        {
          workspace = editor_get_workspace(win->customData);
          if (workspace)
          {
            if (workspace->level != NULL)
            {
              editor_workspace_add_layer(win->customData);
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
        if (eol_line_cmp(listItem->name,"make_active")==0)
        {
          comp = eol_window_get_component_by_name(win,"layer_list");
          if(eol_component_list_get_selected_id(&index,comp))
          {
            editor_workspace_select_layer(win->customData,index);
          }
          else
          {
            eol_dialog_message("Warning","No Selected Layer to Make Active.");
          }
          return eolTrue;
        }
      }
    }
  }
  return eolFalse;
}


void editor_layer_panel_draw(eolWindow *win)
{
  int i = 0;
  GList *l;
  eolComponent *list;
  eolLevelLayer *layer;
  EditorWorkspace *workspace = NULL;
  workspace = editor_get_workspace(win->customData);
  list = eol_window_get_component_by_name(win,"layer_list");
  if ((list)
    &&(workspace)
    &&(workspace->level)
    &&(editor_workspace_updated(win->customData)))
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

eolWindow *editor_layer_panel(eolWindow *workspace)
{
  eolWindow *win;
  win = eol_window_load_from_file("ui/editor_layer_panel.def");
  if (win == NULL)return NULL;
  win->update = editor_layer_panel_update;
  win->draw = editor_layer_panel_draw;
  win->customData = workspace;
  return win;
}

/*eol@eof*/
