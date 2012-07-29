#include <eol_dialog.h>
#include <eol_input.h>
#include <eol_camera.h>
#include <eol_logger.h>
#include <eol_graphics.h>

/*local global variabls*/

/*local function prototypes*/


/*function definitions*/

void editor_header_update(eolWindow *win,GList *updates)
{
  
}

void editor_header_draw(eolWindow *win)
{
}

void editor_header_window()
{
  eolFloat lineHeight;
  eolWindow *win = eol_window_new();
  eolComponent *comp = NULL;
  eolUint w,h;
  if (!win)
  {
    eol_logger_message(
      EOL_LOG_ERROR,
      "editor_header:failed to create base window!\n");
    return;
  }
  eol_graphics_get_size(&w, &h);
  strncpy(win->name,"editor_header",EOLLINELEN);
  eol_rect_copy(&win->rect,eol_rect(0,0,w,h));
  win->canHasFocus = eolTrue;
  win->hasFocus = eolTrue;
  win->drawGeneric = eolFalse;
  win->componentCount = 0;
  win->components = NULL;
  win->customData = NULL;
  win->custom_delete = NULL;
  win->draw = NULL;
  win->update = editor_header_update;
  win->draw = editor_header_draw;
/*  win->customData = malloc(sizeof(TestData));
  memset(win->customData,0,sizeof(TestData));*/
  eol_camera_set_height(30);
  
}


/*eol@eof*/
