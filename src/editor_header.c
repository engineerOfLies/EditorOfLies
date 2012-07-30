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
  eolWindow *win;
  eolComponent *comp;
  win = eol_window_load_from_file("ui/file_header.def");
  win->update = editor_header_update;
  win->draw = editor_header_draw;
  fprintf(stdout,"loaded window: %s\n",win->name);
  fprintf(stdout,"rect: %i,%i,%i,%i\n",
          win->rect.x,
          win->rect.y,
          win->rect.w,
          win->rect.h);
  fprintf(stdout,"drawGeneric: %i\n",win->drawGeneric);
  
    comp = eol_button_stock_new(
    3,
    "test_button2",
    eol_rectf(0.1,0.7,1,1),
    win->rect,
    "Test Entity",
    0,
    eolFalse
  );
  eol_window_add_component(win,comp);

}


/*eol@eof*/
