#include "editor_keyedit.h"
#include "editor_keydialog.h"

#include <eol_dialog.h>
#include <eol_input.h>
#include <eol_logger.h>
#include <eol_graphics.h>
#include <eol_window.h>

/*local global variabls*/

/*local function prototypes*/

/*function definitions*/

void make_key_value_text(eolLine KeyValue,eolLine Key, eolLine Value)
{
  snprintf(KeyValue,EOLLINELEN,"%s :\t %s",Key,Value);
}

void new_key_callback(void *data,eolLine Key, eolLine Value)
{
  eolKeychain *keys;
  eolLine KeyValue;
  eolUint count;
  eolWindow *win = (eolWindow *)data;
  eolComponent *listComp;
  if (!data)return;
  keys = (eolKeychain*)win->customData;
  if (!keys)return;
  make_key_value_text(KeyValue,Key, Value);
  listComp = eol_window_get_component_by_name(win,"key_list");
  eol_component_list_get_count(&count,listComp);
  eol_list_add_text_item(listComp,count,KeyValue);
}

void editor_keyedit_update_keychain(eolKeychain *keyList,eolComponent *listBox)
{
  int i;
  eolUint count = 0;
  eolLine itemText;
  eolLine key,value;
  eolKeychain *newItem, *hashValue;
  char * delimeter;
  eolComponent *item;
  if ((!keyList) || (!listBox))return;
  eol_component_list_get_count(&count,listBox);
  eol_keychain_list_clear(keyList);
  for (i = 0; i < count;i++)
  {
    item = NULL;
    eol_component_list_get_nth_item(&item,listBox,i);
    if (item)
    {
      eol_label_get_text(item,itemText);
      delimeter = strstr(itemText,":\t");
      if (delimeter != NULL)
      {
        eol_line_clear(key);
        eol_line_clear(value);
        strncpy(key,itemText,(delimeter - itemText) - 1);
        key[(delimeter - itemText)] = '\0';
        eol_line_cpy(value,&delimeter[3]);
        newItem = eol_keychain_new_hash();
        if (newItem)
        {
          hashValue = eol_keychain_new_string(key);
          eol_keychain_hash_insert(newItem,"Key",hashValue);
          hashValue = eol_keychain_new_string(value);
          eol_keychain_hash_insert(newItem,"Value",hashValue);
          eol_keychain_list_append(keyList,newItem);
        }
      }
    }
  }
}

eolBool editor_keyedit_update(eolWindow *win,GList *updates)
{
  GList *c;
  eolComponent *comp = NULL;
  if (win == NULL)return eolFalse;
  for (c = updates;c != NULL;c
 = c->next)
  {
    if (c->data == NULL)continue;
    comp = (eolComponent *)c->data;
    if (eol_line_cmp(comp->name,"done")==0)
    {
      editor_keyedit_update_keychain(win->customData,eol_window_get_component_by_name(win,"key_list"));
      eol_window_free(&win);
      return eolTrue;
    }
    else if (eol_line_cmp(comp->name,"cancel")==0)
    {
      
      eol_window_free(&win);
      return eolTrue;
    }
    else if (eol_line_cmp(comp->name,"newkey")==0)
    {
      editor_key_dialog("Enter New Key", win,new_key_callback);
      return eolTrue;
    }
    else if (eol_line_cmp(comp->name,"deletekey")==0)
    {
      eol_component_list_delete_selected_item(
        eol_window_get_component_by_name(win,"key_list"));
      return eolTrue;
    }
  }
  eol_window_free_if_outside_click(&win);
  return eolFalse;
}

void editor_keyedit_draw(eolWindow *win)
{
}

void editor_key_edit_populate_list(eolWindow *win,eolKeychain *keys)
{
  int i = 0;
  eolComponent * listComp = NULL;
  eolKeychain *item;
  eolLine Key,Value;
  eolLine KeyValue;
  if (!win)return;
  listComp = eol_window_get_component_by_name(win,"key_list");
  if (!listComp)return;
  for (i = 0; i < keys->itemCount; i++)
  {
    eol_line_clear(Key);
    eol_line_clear(Value);
    item = eol_keychain_get_list_nth(keys, i);
    eol_keychain_get_hash_value_as_line(Key, item, "Key");
    eol_keychain_get_hash_value_as_line(Value, item, "Value");
    if ((Key[0] != '\0') && (Value[0] != '\0'))
    {
      make_key_value_text(KeyValue,Key, Value);
      eol_list_add_text_item(listComp,i,KeyValue);
    }
  }  
}

void editor_key_edit_window(eolKeychain *keys)
{
  eolWindow *win;
  if ((!keys) || (keys->keyType != eolKeychainList))
  {
    eol_logger_message(EOL_LOG_WARN,"editor_key_edit_window: editing keychain is not a list");
    return;
  }
  win = eol_window_load_from_file("ui/editor_key_edit.def");
  if (win == NULL)return;
  win->update = editor_keyedit_update;
  win->draw = editor_keyedit_draw;
  win->customData = keys;
  editor_key_edit_populate_list(win,keys);
}


/*eol@eof*/
