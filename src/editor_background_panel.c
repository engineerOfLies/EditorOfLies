#include "editor_background_panel.h"
#include "editor_orientation_edit.h"
#include "editor_workspace.h"
#include <eol_dialog.h>
#include <eol_logger.h>

typedef struct
{
  eolWindow *workspace;
  eolLine textBuffer;
  eolWindow *orientationWindow;
}editorBackgroundData;

editorBackgroundData *editor_background_get_data(eolWindow *win)
{
  if (!win)return NULL;
  if (eol_line_cmp(win->name,"edit_backgrounds_panel")!=0)return NULL;/*duck typing!*/
  return (editorBackgroundData*)win->customData;
}

void editor_background_orientation_update(void *data,eolOrientation ori)
{
  eolWindow *win;
  eolComponent *comp = NULL;
  eolBackground *background;
  editorBackgroundData *backgroundData;
  eolUint index = 0;
  if (!data)return;
  win = (eolWindow*)data;
  backgroundData = editor_background_get_data(win);
  if (!backgroundData)return;
  comp = eol_window_get_component_by_name(win,"background_list");
  if(eol_component_list_get_selected_id(&index,comp))
  {
    background = editor_workspace_get_background(backgroundData->workspace,index);
    if (background)
    {
      eol_orientation_copy(&background->ori,ori);
    }
  }
}

void editor_background_load(void *data)
{
  eolWindow *win;
  editorBackgroundData *backgroundData;
  if (!data)return;
  win = (eolWindow*)data;
  backgroundData = editor_background_get_data(win);
  if (!backgroundData)return;
  editor_workspace_add_background(backgroundData->workspace,backgroundData->textBuffer);
}

void editor_background_delete(void *data)
{
  eolWindow *win;
  eolComponent *comp = NULL;
  eolUint backgroundIndex = 0;
  editorBackgroundData *backgroundData;
  if (!data)return;
  win = (eolWindow*)data;
  backgroundData = editor_background_get_data(win);
  comp = eol_window_get_component_by_name(win,"background_list");
  if(eol_component_list_get_selected_id(&backgroundIndex,comp))
  {
    editor_workspace_delete_background(backgroundData->workspace,backgroundIndex);
  }
}

eolBool editor_background_panel_update(eolWindow *win,GList *updates)
{
  eolUint index;
  GList *c,*l;
  eolLine buttonText;
  editorBackgroundData *backgroundData;
  eolBackground *background;
  eolComponent *comp = NULL,*listItem = NULL;
  eolComponent *hideButton = NULL;
  if ((win == NULL)||(updates == NULL))return eolFalse;
  backgroundData = editor_background_get_data(win);
  if (!backgroundData)return eolFalse;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    if (eol_line_cmp(comp->name,"background_list")==0)
    {
      hideButton = eol_window_get_component_by_name(win,"background_actions");
      hideButton = eol_component_list_get_item_by_name(hideButton,"hide_background");
      if (!hideButton)continue;
      if(eol_component_list_get_selected_id(&index,comp))
      {      
        background = editor_workspace_get_background(backgroundData->workspace,index);
        if (background)
        {
          editor_orientation_update_callback(
            backgroundData->orientationWindow,
            background->ori,
            win,
            editor_background_orientation_update);
        }
        if ((!background)||(!background->hidden))
        {
          eol_button_set_text(hideButton,"Hide Background");
        }
        else
        {
          eol_button_set_text(hideButton,"Unhide Background");
        }
      }
      else
      {
        eol_button_set_text(hideButton,"Hide Background");
        editor_orientation_update_callback(
          backgroundData->orientationWindow,
          background->ori,
          NULL,
          NULL);
      }
      continue;
    }
    if (eol_line_cmp(comp->name,"background_actions") == 0)
    {
      /*something in the list of buttons*/
      for (l = eol_list_get_updates(comp);l != NULL; l = l->next)
      {
        if (l->data == NULL)continue;
        listItem = (eolComponent *)l->data;
        if (eol_line_cmp(listItem->name,"new_background")==0)
        {
          eol_dialog_text_prompt(backgroundData->textBuffer,
                                  EOLLINELEN,
                                  EOLLINELEN,  /*0 means no limit*/
                                  "Enter the path for the new background model",
                                  win,
                                  editor_background_load,
                                  NULL);
          return eolTrue;
        }
        if (eol_line_cmp(listItem->name,"delete_background")==0)
        {
          comp = eol_window_get_component_by_name(win,"background_list");
          if(eol_component_list_get_selected_id(&index,comp))
          {
            eol_dialog_yes_no("Delete Selected Background?",
                              win,
                              editor_background_delete,
                              NULL);
          }
          else
          {
            eol_dialog_message("Warning","No Selected Background to Delete.");
          }
          return eolTrue;
        }
        if (eol_line_cmp(listItem->name,"hide_background")==0)
        {
          comp = eol_window_get_component_by_name(win,"background_list");
          if(eol_component_list_get_selected_id(&index,comp))
          {
            eol_button_get_text(buttonText,listItem);
            if (eol_line_cmp(buttonText,"Hide Background")==0)
            {
              editor_workspace_hide_background(backgroundData->workspace, index, eolTrue);
            }
            else if (eol_line_cmp(buttonText,"Unhide Background")==0)
            {
              editor_workspace_hide_background(backgroundData->workspace, index, eolFalse);
            }
          }
          else
          {
            eol_dialog_message("Warning","No Selected Background to Hide.");
          }
          return eolTrue;
        }
        if (eol_line_cmp(listItem->name,"move_background_up")==0)
        {
          comp = eol_window_get_component_by_name(win,"background_list");
          if(eol_component_list_get_selected_id(&index,comp))
          {
            editor_workspace_move_background_up(backgroundData->workspace,index);
          }
          else
          {
            eol_dialog_message("Warning","No Selected Background to move up.");
          }
          return eolTrue;
        }
        if (eol_line_cmp(listItem->name,"move_background_down")==0)
        {
          comp = eol_window_get_component_by_name(win,"background_list");
          if(eol_component_list_get_selected_id(&index,comp))
          {
            editor_workspace_move_background_down(backgroundData->workspace,index);
          }
          else
          {
            eol_dialog_message("Warning","No Selected Background to move down.");
          }
          return eolTrue;
        }
      }
    }
  }
  return eolFalse;
}

void editor_background_workspace_sync(eolWindow *win)
{
  int i = 0;
  GList *l;
  eolComponent *list;
  eolBackground *background;
  EditorWorkspace *workspace = NULL;
  editorBackgroundData *backgroundData;
  backgroundData = editor_background_get_data(win);
  if (!backgroundData)return;
  workspace = editor_get_workspace(backgroundData->workspace);
  list = eol_window_get_component_by_name(win,"background_list");
  if ((list)
    &&(workspace)
    &&(workspace->activeLayer))
  {
    eol_component_list_clear(list);
    for (l = workspace->activeLayer->backgrounds;l != NULL; l = l->next)
    {
      if (!l->data)continue;
      background = (eolBackground*)l->data;
      eol_list_add_text_item(list,i++,background->modelFile);
    }
  }
  editor_orientation_update_callback(
    backgroundData->orientationWindow,
    background->ori,
    NULL,
    NULL);
}

void editor_background_panel_draw(eolWindow *win)
{
  editorBackgroundData *backgroundData;
  backgroundData = editor_background_get_data(win);
  if (!backgroundData)return;
  if (editor_workspace_updated(backgroundData->workspace))
  {
    editor_background_workspace_sync(win);
  }
}

void editor_background_setup_ori_edit(eolWindow *win,eolWindow *ori_edit)
{
  editorBackgroundData *backgroundData;
  backgroundData = editor_background_get_data(win);
  if (!backgroundData)return;
  backgroundData->orientationWindow = ori_edit;
}

eolWindow *editor_background_panel(eolWindow *workspace)
{
  eolWindow *win;
  editorBackgroundData *backgroundData;
  win = eol_window_load_from_file("ui/editor_background_panel.def");
  if (win == NULL)return NULL;
  win->update = editor_background_panel_update;
  win->draw = editor_background_panel_draw;
  win->customData = malloc(sizeof(editorBackgroundData));
  if (!win->customData)
  {
    eol_logger_message(EOL_LOG_ERROR,"editor_background_panel: failed to allocate custom data!");
    eol_window_free(&win);
    return NULL;
  }
  memset(win->customData,0,sizeof(editorBackgroundData));
  backgroundData = (editorBackgroundData*)win->customData;
  backgroundData->workspace = workspace;
  return win;
}

/*eol@eof*/
