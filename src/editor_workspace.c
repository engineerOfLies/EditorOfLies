#include <eol_dialog.h>
#include <eol_level.h>
#include <eol_entity.h>
#include <eol_input.h>
#include <eol_camera.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>

typedef struct
{
  eolLevel *level;
  eolLevelLayer *activeLayer;
}editorWorkspaceData;

/*local global variabls*/

/*local function prototypes*/
eolLevel * editor_workspace_new_level();


/*function definitions*/

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
  editorWorkspaceData *data;
  if (!genericdata)return;
  data = (editorWorkspaceData *)genericdata;
  eol_level_free(&data->level);
  memset(data,0,sizeof(editorWorkspaceData));
  free(data);
}

void editor_workspace()
{
  eolWindow *win;
  editorWorkspaceData *data;
  win = eol_window_load_from_file("ui/workspace.def");
  win->update = editor_workspace_update;
  win->draw = editor_workspace_draw;
  win->custom_delete = editor_workspace_delete;
  win->customData = malloc(sizeof(editorWorkspaceData));
  eol_camera_config();
  eol_camera_init();
  if (win->customData == NULL)
  {
    eol_logger_message(EOL_LOG_ERROR,"editor_workspace: failed to allocate workspace data\n");
    return;
  }
  data = (editorWorkspaceData*)win->customData;
  data->level = editor_workspace_new_level();
}

eolLevel * editor_workspace_new_level()
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
  
  back->model = eol_model_load("models/testlevel.actor");
  layer->clipMesh = eol_mesh_load("models/levelmesh/testlevel_mask.obj");
  
  eol_level_add_mask_to_space(layer);
  eol_orientation_clear(&layer->clipMaskOri);
  
  eol_rectf_set(&layer->bounds, -10, -10, 20, 40);
  
  eol_level_set_active_layer(level, 0);
  eol_level_set_current_level(level);
  return level;
}

