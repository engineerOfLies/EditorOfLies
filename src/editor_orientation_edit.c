#include "editor_orientation_edit.h"
#include "editor_workspace.h"

#include <eol_mouse.h>
#include <eol_dialog.h>
#include <eol_draw.h>
#include <eol_input.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>


enum OriModes
{
  OM_NONE   = 0,
  OM_GRAB   = 1,
  OM_ROT    = 2,
  OM_SCALE  = 3,
  OM_COLOR  = 4,
  OM_TRANS  = 5
};

enum OriAxis
{
  OA_X = 1,
  OA_Y = 2,
  OA_Z = 4
};

typedef struct
{
  eolUint omMode;
  eolUint secondaryMode;  /*bit mask*/
  eolVec2D  mouseBegin;   /*coordinates of mouse at start of operation*/
  eolOrientation ori;
  eolOrientation oldOri;
  eolOrientation copyOri;
  eolBool        copySet;
  void(*callback)(void *data,eolOrientation ori);
  void *callbackData;
}OriData;

/*local global variabls*/

/*local function prototypes*/
/**
 * @brief called whenever the orientation has been changed.
 * calls the callback if defined.
 * @param oriData the oriData for the window
 */
void editor_orientation_ori_updated(eolWindow *win);

/*function definitions*/

OriData *editor_orientation_get_data(eolWindow *win)
{
  if (!win)return NULL;
  if (eol_line_cmp(win->name,"edit_backgrounds_panel") !=0)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "editor_orientation_get_data: Passed a window not my own");
    return NULL;
  }
  return (OriData*)win->customData;
}

void editor_orientation_reset_ori(void *data)
{
  OriData *oriData;
  oriData = editor_orientation_get_data((eolWindow *)data);
  if (!oriData)return;
  eol_orientation_clear(&oriData->ori);
}

void editor_orientation_update_callback(
  eolWindow *win,
  eolOrientation ori,
  void *callbackData,
  void(*callback)(void *data,eolOrientation ori))
{
  OriData *oriData;
  oriData = editor_orientation_get_data(win);
  if (!oriData)return;
  if (!callback)
  {
    eol_window_hide(win);
  }
  else
  {
    eol_window_show(win);
  }
  oriData->callback = callback;
  oriData->callbackData = callbackData;
  eol_orientation_copy(&oriData->ori,ori);
  editor_orientation_ori_updated(win);
}

void editor_orientation_update_ori(
  eolWindow *win,
  eolOrientation ori)
{
  OriData *oriData;
  oriData = editor_orientation_get_data(win);
  if (!oriData)return;
  eol_orientation_copy(&oriData->ori,ori);
  editor_orientation_ori_updated(win);
}

void editor_orientation_ori_updated(eolWindow *win)
{
  OriData *oriData;
  eolLine labelText;
  eolComponent *labelComp = NULL;
  eolComponent *listComp = NULL;
  oriData = editor_orientation_get_data(win);
  if (!oriData)return;
  
  listComp = eol_window_get_component_by_name(win,"orientation");
  if (!listComp)return;
  snprintf(labelText,
           EOLLINELEN,
           "Position ( %f, %f, %f )",
           oriData->ori.position.x,
           oriData->ori.position.y,
           oriData->ori.position.z);
  labelComp = eol_component_list_get_item_by_name(listComp,"position");
  eol_button_set_text(labelComp,labelText);

  snprintf(labelText,
           EOLLINELEN,
           "Rotation ( %f, %f, %f )",
           oriData->ori.rotation.x,
           oriData->ori.rotation.y,
           oriData->ori.rotation.z);
  labelComp = eol_component_list_get_item_by_name(listComp,"rotation");
  eol_button_set_text(labelComp,labelText);

  snprintf(labelText,
           EOLLINELEN,
           "Scale ( %f, %f, %f )",
           oriData->ori.scale.x,
           oriData->ori.scale.y,
           oriData->ori.scale.z);
  labelComp = eol_component_list_get_item_by_name(listComp,"scale");
  eol_button_set_text(labelComp,labelText);

  snprintf(labelText,
           EOLLINELEN,
           "Color ( %f, %f, %f )",
           oriData->ori.color.x,
           oriData->ori.color.y,
           oriData->ori.color.z);
  labelComp = eol_component_list_get_item_by_name(listComp,"color");
  eol_button_set_text(labelComp,labelText);

  snprintf(labelText,EOLLINELEN,"Alpha  %f",oriData->ori.alpha);
  labelComp = eol_component_list_get_item_by_name(listComp,"alpha");
  eol_button_set_text(labelComp,labelText);
  
  if (oriData->callback != NULL)
  {
    oriData->callback(oriData->callbackData,oriData->ori);
  }
}

void editor_orientation_complete_op(eolWindow *win)
{
  eolComponent *operationLabel;
  OriData *oriData;
  oriData = editor_orientation_get_data(win);
  if (!oriData)return;
  oriData->omMode = OM_NONE;
  oriData->secondaryMode = 0;
  editor_orientation_ori_updated(win);
  operationLabel = eol_window_get_component_by_name(win,"operation");
  eol_label_set_text(operationLabel,"Operation");
}

void editor_orientation_reset_op(eolWindow *win)
{
  OriData *oriData;
  oriData = editor_orientation_get_data(win);
  if (!oriData)return;
  if (oriData->omMode == OM_NONE)return;
  eol_orientation_copy(&oriData->ori,oriData->oldOri);
}

void editor_orientation_cancel_op(eolWindow *win)
{
  eolComponent *operationLabel;
  OriData *oriData;
  oriData = editor_orientation_get_data(win);
  if (!oriData)return;
  if (oriData->omMode == OM_NONE)return;
    
  editor_orientation_update_ori(
    win,
    oriData->oldOri);
  oriData->omMode = OM_NONE;
  oriData->secondaryMode = 0;
  operationLabel = eol_window_get_component_by_name(win,"operation");
  eol_label_set_text(operationLabel,"Operation");
}

void editor_orientation_set_mode(eolWindow *win,eolUint mode)
{
  eolComponent *operationLabel;
  OriData *oriData;
  oriData = editor_orientation_get_data(win);
  if (!oriData)return;
  if (mode == oriData->omMode)return;
  if (!oriData->callback)
  {
    eol_dialog_message("Warning","No Orientation data selected!");
    return;
  }
  if (oriData->omMode == OM_NONE)
  {
    /*cache starting value*/
    eol_orientation_copy(&oriData->oldOri,oriData->ori);
  }
    
  oriData->omMode = mode;
  /*get starting mouse position for motion based changes*/
  eol_mouse_get_position_vec2d(&oriData->mouseBegin);
  operationLabel = eol_window_get_component_by_name(win,"operation");
  switch (mode)
  {
    case OM_GRAB:
      eol_label_set_text(operationLabel,"GRAB");
      oriData->secondaryMode = OA_X | OA_Y;
      break;
    case OM_ROT:
      eol_label_set_text(operationLabel,"ROTATE");
      oriData->secondaryMode = OA_Z;
      break;
    case OM_SCALE:
      eol_label_set_text(operationLabel,"SCALE");
      oriData->secondaryMode = OA_X | OA_Y | OA_Z;
      break;
    case OM_COLOR:
      eol_label_set_text(operationLabel,"COLOR TRANSFORM");
      break;
    case OM_TRANS:
      eol_label_set_text(operationLabel,"ALPHA ADJUST");
      break;
  }
}

eolBool editor_orientation_update(eolWindow *win,GList *updates)
{
  GList *c;
  GList *l;
  OriData *oriData;
  eolComponent *comp = NULL;
  eolComponent *item = NULL;
  oriData = editor_orientation_get_data(win);
  if (oriData == NULL)return eolFalse;
  for (c = updates;c != NULL;c = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    if (eol_line_cmp(comp->name,"beginGrab")==0)
    {
      editor_orientation_set_mode(win,OM_GRAB);
      return eolTrue;
    }
    if (eol_line_cmp(comp->name,"beginScale")==0)
    {
      editor_orientation_set_mode(win,OM_SCALE);
      return eolTrue;
    }
    if (eol_line_cmp(comp->name,"beginRotate")==0)
    {
      editor_orientation_set_mode(win,OM_ROT);
      return eolTrue;
    }
    if (eol_line_cmp(comp->name,"xLock")==0)
    {
      if (oriData->omMode)
      {
        if (eol_input_is_mod_down(KMOD_SHIFT))
        {
          oriData->secondaryMode = OA_Z | OA_Y;
          editor_orientation_reset_op(win);
        }
        else
        {
          oriData->secondaryMode = OA_X;
          editor_orientation_reset_op(win);
        }
      }
    }
    if (eol_line_cmp(comp->name,"yLock")==0)
    {
      if (oriData->omMode)
      {
        if (eol_input_is_mod_down(KMOD_SHIFT))
        {
          oriData->secondaryMode = OA_Z | OA_X;
          editor_orientation_reset_op(win);
        }
        else
        {
          oriData->secondaryMode = OA_Y;
          editor_orientation_reset_op(win);
        }
      }
    }
    if (eol_line_cmp(comp->name,"zLock")==0)
    {
      if (oriData->omMode)
      {
        if (eol_input_is_mod_down(KMOD_SHIFT))
        {
          oriData->secondaryMode = OA_X | OA_Y;
          editor_orientation_reset_op(win);
        }
        else
        {
          oriData->secondaryMode = OA_Z;
          editor_orientation_reset_op(win);
        }
      }
    }
    if (eol_line_cmp(comp->name,"cancel")==0)
    {
      editor_orientation_cancel_op(win);
      return eolTrue;
    }
    if (comp->id == 1000)
    {
      for (l = eol_list_get_updates(comp);l != NULL; l = l->next)
      {
        if (l->data == NULL)continue;
        item = (eolComponent *)l->data;
      }
    }
    if (comp->id == 1001)
    {
      for (l = eol_list_get_updates(comp);l != NULL; l = l->next)
      {
        if (l->data == NULL)continue;
        item = (eolComponent *)l->data;
        if (eol_line_cmp(item->name,"copy")==0)
        {
          if (!oriData->callback)
          {
            eol_dialog_message("Warning","No Selected Orientation to Copy.");
          }
          else
          {
            eol_orientation_copy(&oriData->copyOri,oriData->ori);
            oriData->copySet = eolTrue;
          }
          return eolTrue;
        }
        if (eol_line_cmp(item->name,"paste")==0)
        {
          if (!oriData->copySet)
          {
            eol_dialog_message("Warning","No Orientation data in clipboard!");
          }
          else if (!oriData->callback)
          {
            eol_dialog_message("Warning","No Orientation data selected!");
          }
          else 
          {
            editor_orientation_update_ori(win,oriData->copyOri);
          }
          return eolTrue;
        }
        if (eol_line_cmp(item->name,"reset")==0)
        {
          if (!oriData->callback)
          {
            eol_dialog_message("Warning","No Selected Orientation to Reset.");
          }
          else
          {
            eol_dialog_yes_no("Reset Selected Orientation?",
                              win,
                              editor_orientation_reset_ori,
                              NULL);
          }
          return eolTrue;
        }
      }
    }
  }
  if (eol_mouse_input_state(eolMouseLeft))
  {
    if (oriData->omMode != OM_NONE)
    {
      editor_orientation_complete_op(win);
      return eolTrue;
    }
  }
  if (eol_mouse_input_state(eolMouseRight))
  {
    editor_orientation_cancel_op(win);
    return eolTrue;
  }
  return eolFalse;
}

eolFloat editor_fine_snap(eolFloat in,eolBool fineMode, eolBool snapMode)
{
  eolInt  tempInt;
  eolFloat tempFloat;
  eolFloat diff;
  if (snapMode)
  {
    if (fineMode)
    {
      tempFloat = in * 10;
    }
    else tempFloat = in;
    tempInt = (eolInt)tempFloat;
    diff = in - tempInt;
    if (diff > 0.5)tempInt++;
    if (fineMode)
    {
      tempFloat = tempInt;
      tempFloat *= 0.1;
      return tempFloat;
    }
    return (eolFloat)tempInt;
  }
  return in;
}

void editor_orientation_draw(eolWindow *win)
{
  /*draw happend every frame, so here is where we update mouse based updates*/
  eolVec2D mouseNow;
  OriData *oriData;
  eolBool snapMode = eolFalse;
  eolBool  fineMode = eolFalse;
  eolFloat moveFactor = 0.01;
  eolFloat scaleLimit = 0.001;
  eolFloat mouseDistance = 0;
  eolFloat zeroDistance = 0;
  eolFloat rotAngle = 0;
  eolVec2D mouseVector = {0,0};
  oriData = editor_orientation_get_data(win);
  if (!oriData)return;
  if (oriData->omMode == OM_NONE)
  {
    /*no need to go further*/
    return;
  }
  /*TODO take camera distance into consideration as well*/
  if (eol_input_is_mod_down(KMOD_CTRL))/*snap to whole number*/
  {
    snapMode = eolTrue;
  }
  if (eol_input_is_mod_down(KMOD_SHIFT))
  {
    fineMode = eolTrue;
  }
  eol_mouse_get_position_vec2d(&mouseNow);
  eol_vec2d_set(mouseVector,(mouseNow.x - oriData->mouseBegin.x),(oriData->mouseBegin.y - mouseNow.y));
  switch(oriData->omMode)
  {
    case OM_GRAB:
      if (oriData->secondaryMode & OA_X)
      {
        oriData->ori.position.x = oriData->oldOri.position.x + (mouseVector.x * moveFactor);
        oriData->ori.position.x = editor_fine_snap(oriData->ori.position.x,fineMode, snapMode);
      }
      if (oriData->secondaryMode & OA_Y)
      {
        oriData->ori.position.y = oriData->oldOri.position.y + (mouseVector.y * moveFactor);
        oriData->ori.position.y = editor_fine_snap(oriData->ori.position.y,fineMode, snapMode);
      }
      if (oriData->secondaryMode & OA_Z)
      {
        oriData->ori.position.z = oriData->oldOri.position.z + (mouseVector.y * moveFactor);
        oriData->ori.position.z = editor_fine_snap(oriData->ori.position.z,fineMode, snapMode);
      }
      editor_orientation_ori_updated(win);
      break;
    case OM_ROT:
      eol_draw_line_2D(oriData->mouseBegin,mouseNow,1,eol_vec3d(0.5,1,0.5),1);
      eol_draw_line_2D(oriData->mouseBegin,eol_vec2d(oriData->mouseBegin.x + 100,oriData->mouseBegin.y),1,eol_vec3d(1,1,1),1);
      rotAngle = eol_vec2d_angle(mouseVector) - 180;
      rotAngle = editor_fine_snap(rotAngle * 0.2,fineMode,snapMode) * 5;
      if (fineMode && !snapMode)
      {
        rotAngle *= 0.1;
      }
      
      if (oriData->secondaryMode & OA_X)
      {
        oriData->ori.rotation.x = oriData->oldOri.rotation.x + rotAngle;
      }
      if (oriData->secondaryMode & OA_Y)
      {
        oriData->ori.rotation.y = oriData->oldOri.rotation.y + rotAngle;
      }
      if (oriData->secondaryMode & OA_Z)
      {
        oriData->ori.rotation.z = oriData->oldOri.rotation.z + rotAngle;
      }
      editor_orientation_ori_updated(win);
      eol_draw_cirlce_2D(oriData->mouseBegin,
                         100,
                         32,
                         eol_vec3d(1,1,1),
                         1);
      break;
    case OM_SCALE:
      eol_draw_line_2D(oriData->mouseBegin,mouseNow,1,eol_vec3d(1,0.5,0.5),1);
      mouseDistance = eol_vec2d_magnitude(mouseVector);
      zeroDistance = 100;
      if (fineMode && !snapMode)
      {
        moveFactor *= 0.1;
        mouseDistance *= 0.1;
      }
      mouseDistance -= zeroDistance;
      eol_draw_cirlce_2D(oriData->mouseBegin,
                         zeroDistance,
                         32,
                         eol_vec3d(1,1,1),
                         1);

      if (oriData->secondaryMode & OA_X)
      {
        oriData->ori.scale.x = oriData->oldOri.scale.x + (mouseDistance  * moveFactor);
        oriData->ori.scale.x = editor_fine_snap(oriData->ori.scale.x,fineMode, snapMode);
      }
      if (oriData->secondaryMode & OA_Y)
      {
        oriData->ori.scale.y = oriData->oldOri.scale.y + (mouseDistance  * moveFactor);
        oriData->ori.scale.y = editor_fine_snap(oriData->ori.scale.y,fineMode, snapMode);
      }
      if (oriData->secondaryMode & OA_Z)
      {
        oriData->ori.scale.z = oriData->oldOri.scale.z + (mouseDistance  * moveFactor);
        oriData->ori.scale.z = editor_fine_snap(oriData->ori.scale.z,fineMode, snapMode);
      }
      
      if ((oriData->ori.scale.x >=0) && (oriData->ori.scale.x < scaleLimit))
      {
        oriData->ori.scale.x = scaleLimit;
      }
      else if ((oriData->ori.scale.x < 0) && (oriData->ori.scale.x > -scaleLimit))
      {
        oriData->ori.scale.x = -scaleLimit;
      }

      if ((oriData->ori.scale.y >=0) && (oriData->ori.scale.y < scaleLimit))
      {
        oriData->ori.scale.y = scaleLimit;
      }
      else if ((oriData->ori.scale.y < 0) && (oriData->ori.scale.y > -scaleLimit))
      {
        oriData->ori.scale.y = -scaleLimit;
      }

      if ((oriData->ori.scale.z >=0) && (oriData->ori.scale.z < scaleLimit))
      {
        oriData->ori.scale.z = scaleLimit;
      }
      else if ((oriData->ori.scale.z < 0) && (oriData->ori.scale.z > -scaleLimit))
      {
        oriData->ori.scale.z = -scaleLimit;
      }
      
      
      editor_orientation_ori_updated(win);
      break;
    case OM_COLOR:
      break;
    case OM_TRANS:
      break;
  }
  /*draw axis*/
  if (oriData->secondaryMode & OA_X)
  {
    eol_draw_line_2D(eol_vec2d(oriData->mouseBegin.x - 100,oriData->mouseBegin.y),
                     eol_vec2d(oriData->mouseBegin.x + 100,oriData->mouseBegin.y),
                     1,
                     eol_vec3d(1,0,0),
                     1);
  }
  if (oriData->secondaryMode & OA_Y)
  {
    eol_draw_line_2D(eol_vec2d(oriData->mouseBegin.x,oriData->mouseBegin.y - 100),
                     eol_vec2d(oriData->mouseBegin.x,oriData->mouseBegin.y + 100),
                     1,
                     eol_vec3d(0,1,0),
                     1);
  }
  if (oriData->secondaryMode & OA_Z)
  {
    eol_draw_line_2D(eol_vec2d(oriData->mouseBegin.x + 50,oriData->mouseBegin.y - 50),
                     eol_vec2d(oriData->mouseBegin.x - 50,oriData->mouseBegin.y + 50),
                     1,
                     eol_vec3d(0,0,1),
                     1);
  }
}

eolWindow *editor_orientation_edit()
{
  eolWindow *win;
  OriData *oriData;
  win = eol_window_load_from_file("ui/editor_orientation_edit.def");
  if (win == NULL)return NULL;
  win->update = editor_orientation_update;
  win->draw = editor_orientation_draw;
  win->customData = malloc(sizeof(OriData));
  if (!win->customData)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "editor_orientation_edit: failed to allocate window data");
    eol_window_free(&win);
    return NULL;
  }
  memset(win->customData,0,sizeof(OriData));
  oriData = (OriData*)win->customData;
  eol_orientation_clear(&oriData->ori);
  eol_orientation_clear(&oriData->oldOri);
  return win;
}


/*eol@eof*/
