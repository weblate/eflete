#include "shortcuts.h"
#include "config.h"
static Eina_List *handlers_stack = NULL;

static inline unsigned int
_keycode_convert(unsigned int keycode)
{
#ifdef __APPLE__
    #define KEY(mac, lin) case mac: return lin;
   switch (keycode)
   {
      KEY(18, 10) /* 1 */
      KEY(19, 11)
      KEY(20, 12)
      KEY(21, 13)
      KEY(23, 14)
      KEY(22, 15)
      KEY(26, 16)
      KEY(28, 17)
      KEY(25, 18)
      KEY(29, 19)
      KEY(27, 20) /* - */
      KEY(24, 21) /* = */

      KEY(12, 24) /* q */
      KEY(13, 25)
      KEY(14, 26)
      KEY(15, 27)
      KEY(17, 28)
      KEY(16, 29)
      KEY(32, 30)
      KEY(34, 31)
      KEY(31, 32)
      KEY(35, 33) /* p */
      KEY(33, 34) /* [ */
      KEY(30, 35) /* ] */
      KEY(42, 51) /* \ */

      KEY(0, 38) /* a */
      KEY(1, 39)
      KEY(2, 40)
      KEY(3, 41)
      KEY(5, 42)
      KEY(4, 43)
      KEY(38, 44)
      KEY(40, 45)
      KEY(37, 46)
      KEY(41, 47) /* ; */
      KEY(39, 48) /* ' */

      KEY(6, 52) /* z */
      KEY(7, 53)
      KEY(8, 54)
      KEY(9, 55)
      KEY(11, 56)
      KEY(45, 57)
      KEY(46, 58)
      KEY(43, 59)
      KEY(47, 60)
      KEY(44, 61) /* / */

      KEY(48, 23) /* tab */

      KEY(123, 113) /* left */
      KEY(124, 114) /* right */

      KEY(122, 67) /* F1 */
      KEY(120, 68) /* F2 */
      KEY(99, 69) /* F3 */
      KEY(118, 70) /* F4 */
      KEY(98, 73) /* F7 */
      KEY(100, 74) /* F8 */
      KEY(101, 75) /* F9 */
      KEY(109, 76) /* F10 */
      KEY(103, 77) /* F11 */
   }
   DBG("notconverted mac keycode: %d", keycode);
#undef KEY
#endif
   return keycode;
}

struct _Shortcut_Module
{
   Ecore_Event_Handler *shortcuts_wheel_handler; /**< handler for catching mouse wheel\
                                                   for shortcuts */
   Ecore_Event_Handler *shortcuts_handler; /**< handler for catching key presses\
                                                for shortcuts */
   Ecore_Event_Handler *shortcuts_handler_unpress; /**< handler for catching key
                                                        unpressing for
                                                        shortcuts */
   Eina_List *shortcuts;      /**< list of user's shortcuts */
   Eina_List *held_shortcuts; /**< list of functions that is being held */
   unsigned int last_modifiers;
   Eina_Bool disabled;  /**< If shortcuts disabled - there are no reaction to input > */
};

static int
_shortcut_cmp(Shortcut *s1, Shortcut *s2)
{
   if (s1->modifiers == s2->modifiers)
     {
        if (s1->keyname && s2->keyname)
          {
             return strcmp(s1->keyname, s2->keyname);
          }
     }
   return s1->modifiers - s2->modifiers;
}

static void
_shortcut_handle(Shortcut_Type type)
{
   int num;
   Evas_Object *handler;
#define SHORTCUT(NAME, SIGNAL) \
 case SHORTCUT_TYPE_##NAME: \
    assert(SIGNAL != NULL); \
    evas_object_smart_callback_call(handler, SIGNAL, NULL); \
    break;

#define SHORTCUT_GLOBAL_ONLY(NAME, SIGNAL) \
 case SHORTCUT_TYPE_##NAME: \
    assert(SIGNAL != NULL); \
    evas_object_smart_callback_call(ap.win, SIGNAL, NULL); \
    break;

#define SHORTCUT_NUM(NAME, SIGNAL, NUM) \
 case SHORTCUT_TYPE_##NAME: \
    assert(SIGNAL != NULL); \
    num = NUM; \
    evas_object_smart_callback_call(handler, SIGNAL, &num); \
    break;

   if (handlers_stack)
     handler = eina_list_data_get(handlers_stack);
   else
     handler = ap.win;

   switch (type)
     {
        SHORTCUT_GLOBAL_ONLY(QUIT, signals.shortcut.quit);
        SHORTCUT_GLOBAL_ONLY(SAVE, signals.shortcut.save);
        SHORTCUT(REDO, signals.shortcut.history.redo);
        SHORTCUT(UNDO, signals.shortcut.history.undo);
        SHORTCUT(ADD_GROUP, signals.shortcut.add.group);
        SHORTCUT(ADD_PART, signals.shortcut.add.part);
        SHORTCUT(ADD_STATE, signals.shortcut.add.state);
        SHORTCUT(ADD_ITEM, signals.shortcut.add.item);
        SHORTCUT(ADD_PROGRAM, signals.shortcut.add.program);
        SHORTCUT(ADD_DATA_ITEM, signals.shortcut.add.data_item);
        SHORTCUT(DEL, signals.shortcut.del);
        SHORTCUT(STATE_NEXT, signals.shortcut.workspace.select.state_next);
        SHORTCUT(PART_NEXT, signals.shortcut.workspace.select.part_next);
        SHORTCUT(PART_PREV, signals.shortcut.workspace.select.part_prev);
        SHORTCUT(PART_SHOWHIDE, signals.shortcut.workspace.show_hide.part);
        SHORTCUT(ALL_PARTS_SHOWHIDE, signals.shortcut.workspace.show_hide.all_parts);
        SHORTCUT_NUM(TAB_NUM1, signals.shortcut.tab.num, 1);
        SHORTCUT_NUM(TAB_NUM2, signals.shortcut.tab.num, 2);
        SHORTCUT_NUM(TAB_NUM3, signals.shortcut.tab.num, 3);
        SHORTCUT_NUM(TAB_NUM4, signals.shortcut.tab.num, 4);
        SHORTCUT_NUM(TAB_NUM5, signals.shortcut.tab.num, 5);
        SHORTCUT_NUM(TAB_NUM6, signals.shortcut.tab.num, 6);
        SHORTCUT_NUM(TAB_NUM7, signals.shortcut.tab.num, 7);
        SHORTCUT_NUM(TAB_NUM8, signals.shortcut.tab.num, 8);
        SHORTCUT_NUM(TAB_NUM9, signals.shortcut.tab.num, 9);
        SHORTCUT_NUM(TAB_NUM10, signals.shortcut.tab.num, 10);
        SHORTCUT(TAB_NEXT, signals.shortcut.tab.next);
        SHORTCUT(TAB_PREV, signals.shortcut.tab.prev);
        SHORTCUT(TAB_CLOSE, signals.shortcut.tab.close);
        SHORTCUT(TAB_IMAGE_MANAGER, signals.shortcut.manager.image);
        SHORTCUT(TAB_SOUND_MANAGER, signals.shortcut.manager.sound);
        SHORTCUT(TAB_COLOR_CLASS_MANAGER, signals.shortcut.manager.color_class);
        SHORTCUT(TAB_STYLE_MANAGER, signals.shortcut.manager.style);
        SHORTCUT(TAB_SCRIPT_MANAGER, signals.shortcut.manager.script);
        SHORTCUT(MODE_NORMAL, signals.shortcut.workspace.mode.normal);
        SHORTCUT(MODE_CODE, signals.shortcut.workspace.mode.code);
        SHORTCUT(MODE_DEMO, signals.shortcut.workspace.mode.demo);
        SHORTCUT(ZOOM_IN, signals.shortcut.workspace.zoom.in);
        SHORTCUT(ZOOM_OUT, signals.shortcut.workspace.zoom.out);
        SHORTCUT(ZOOM_RESET, signals.shortcut.workspace.zoom.reset);
        SHORTCUT(FILL, signals.shortcut.workspace.fill);
        SHORTCUT(FIT, signals.shortcut.workspace.zoom.fit);
        SHORTCUT(RULERS_SHOW, signals.shortcut.workspace.show_hide.rulers);
        SHORTCUT(OBJECT_AREA, signals.shortcut.workspace.show_hide.object_area);
        SHORTCUT(CANCEL, signals.shortcut.popup.cancel);
        SHORTCUT(DONE, signals.shortcut.popup.done);
        SHORTCUT(HELP, signals.shortcut.help);

      case SHORTCUT_TYPE_NONE:
         break;
      case SHORTCUT_TYPE_LAST:
         CRIT("Incorrect shortcut type");
         abort();
     }
}

static Eina_Bool
_mouse_wheel_event_cb(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Event_Mouse_Wheel *ev = event;

   if (ap.shortcuts->disabled)
     return ECORE_CALLBACK_PASS_ON;

   if ((ev->modifiers & 255) != MOD_CTRL)
     return ECORE_CALLBACK_PASS_ON;

   if (ev->z > 0)
     _shortcut_handle(SHORTCUT_TYPE_ZOOM_OUT);
   else if (ev->z < 0)
     _shortcut_handle(SHORTCUT_TYPE_ZOOM_IN);

   return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_key_press_event_cb(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Event_Key *ev = (Ecore_Event_Key *)event;
   Shortcut sc, *shortcut;
   const char *obj_name;

   if (ap.shortcuts->disabled)
     return ECORE_CALLBACK_PASS_ON;

   /*
    *  (ev->modifiers && 255) because modifiers contain both locks and modifs,
    *  so if Caps Lock is clicked, then with MOD_SHIFT it will return not 1, but 257.
    *  So we need to set a mask for real modifiers (Ctrl, Shift, Alt etc)
    */
   sc.modifiers = ev->modifiers & 255;
   sc.keycode = _keycode_convert(ev->keycode);
   switch(sc.keycode)
     {
      case 37: /*MOD_CTRL*/
      case 105: /*MOD_CTRL*/
         sc.modifiers |= MOD_CTRL;
         break;
      case 64: /*MOD_ALT*/
      case 108: /*MOD_ALT*/
         sc.modifiers |= MOD_ALT;
         break;
      case 50: /*MOD_SHIFT*/
      case 62: /*MOD_SHIFT*/
         sc.modifiers |= MOD_SHIFT;
         break;
      case 133: /*MOD_SUPER*/
      case 134: /*MOD_SUPER*/
         sc.modifiers |= MOD_SUPER;
         break;
      default:
         break;
     }

   sc.keyname = (char *)ev->keyname;
   if (!sc.keyname)
     return ECORE_CALLBACK_PASS_ON;
   /* ignore hotkey if */
   obj_name = evas_object_type_get(elm_object_focused_object_get(ap.win));
   if (/* it is without modifier or with shift-only */
      ((sc.modifiers == MOD_NONE) || (sc.modifiers == MOD_SHIFT)) &&
      /* is not F1 - F12 */
      !(!strcmp(sc.keyname, "F1") ||
        !strcmp(sc.keyname, "F2") ||
        !strcmp(sc.keyname, "F3") ||
        !strcmp(sc.keyname, "F4") ||
        !strcmp(sc.keyname, "F5") ||
        !strcmp(sc.keyname, "F6") ||
        !strcmp(sc.keyname, "F7") ||
        !strcmp(sc.keyname, "F8") ||
        !strcmp(sc.keyname, "F9") ||
        !strcmp(sc.keyname, "F10") ||
        !strcmp(sc.keyname, "F11") ||
        !strcmp(sc.keyname, "F12") ||
        !strcmp(sc.keyname, "Return") ||
        !strcmp(sc.keyname, "Escape"))  &&
      /* elm_entry is in focus */
      (obj_name ? (!strcmp("elm_entry", obj_name)) : false))
     {
        DBG("entry focused, ignoring hotkeys with MOD_SHIFT or MODE_NONE");
        return ECORE_CALLBACK_PASS_ON;
     }

   DBG("key_down: %s %d name[%s], mod: %d", ev->key, ev->keycode, ev->keyname, ev->modifiers & 255);

   ap.shortcuts->last_modifiers = sc.modifiers;

   /* check if shortcut already pressed */
   shortcut = eina_list_search_sorted(ap.shortcuts->held_shortcuts, (Eina_Compare_Cb)_shortcut_cmp, &sc);
   if (shortcut)
     return ECORE_CALLBACK_PASS_ON;

   shortcut = eina_list_search_sorted(ap.shortcuts->shortcuts, (Eina_Compare_Cb)_shortcut_cmp, &sc);
   if (shortcut)
     {
        ap.shortcuts->held_shortcuts = eina_list_sorted_insert(ap.shortcuts->held_shortcuts,
                                                               (Eina_Compare_Cb)_shortcut_cmp, shortcut);
        _shortcut_handle(shortcut->type_press);
        if (shortcut->type_press != SHORTCUT_TYPE_NONE)
          return ECORE_CALLBACK_DONE;
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_key_unpress_event_cb(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Event_Key *ev = (Ecore_Event_Key *)event;
   Shortcut sc, *shortcut;
   unsigned int unpressed_modifiers;
   Eina_List *l, *l_n;
   if (ap.shortcuts->disabled)
     return ECORE_CALLBACK_PASS_ON;

   /*
    *  (ev->modifiers && 255) because modifiers contain both locks and modifs,
    *  so if Caps Lock is clicked, then with SHIFT it will return not 1, but 257.
    *  So we need to set a mask for real modifiers (Ctrl, Shift, Alt etc)
    */
   sc.modifiers = ev->modifiers & 255;
   sc.keycode = _keycode_convert(ev->keycode);
   switch(sc.keycode)
     {
      case 37: /*MOD_CTRL*/
      case 105: /*MOD_CTRL*/
         sc.modifiers &= ~MOD_CTRL;
         break;
      case 64: /*MOD_ALT*/
      case 108: /*MOD_ALT*/
         sc.modifiers &= ~MOD_ALT;
         break;
      case 50: /*MOD_SHIFT*/
      case 62: /*MOD_SHIFT*/
         sc.modifiers &= ~MOD_SHIFT;
         break;
      case 133: /*MOD_SUPER*/
      case 134: /*MOD_SUPER*/
         sc.modifiers &= ~MOD_SUPER;
         break;
      default:
         break;
     }
   DBG("key_up  : %s %d, mod: %d", ev->key, ev->keycode, ev->modifiers & 255);

   sc.keyname = (char *)ev->keyname;
   unpressed_modifiers = ap.shortcuts->last_modifiers & (~sc.modifiers);
   EINA_LIST_FOREACH_SAFE(ap.shortcuts->held_shortcuts, l, l_n, shortcut)
     {
        if (shortcut->modifiers & unpressed_modifiers)
          {
             _shortcut_handle(shortcut->type_unpress);
             ap.shortcuts->held_shortcuts = eina_list_remove_list(ap.shortcuts->held_shortcuts, l);
          }
     }
   ap.shortcuts->last_modifiers = sc.modifiers;
   /* check if shortcut already pressed */
   l = eina_list_search_sorted_list(ap.shortcuts->held_shortcuts, (Eina_Compare_Cb)_shortcut_cmp, &sc);
   shortcut = eina_list_data_get(l);
   if (!shortcut)
     return ECORE_CALLBACK_PASS_ON;

   _shortcut_handle(shortcut->type_unpress);
   ap.shortcuts->held_shortcuts = eina_list_remove_list(ap.shortcuts->held_shortcuts, l);
   if (shortcut->type_unpress != SHORTCUT_TYPE_NONE)
     return ECORE_CALLBACK_DONE;

   return ECORE_CALLBACK_PASS_ON;
}

static void
_win_unfocused_cb(void *data __UNUSED__,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   Shortcut *shortcut;

   EINA_LIST_FREE(ap.shortcuts->held_shortcuts, shortcut)
      _shortcut_handle(shortcut->type_unpress);
}

static char *
_combination_string_get(Shortcut *sc)
{
   Eina_Strbuf *buf = eina_strbuf_new();
   char *result = NULL;
   if (sc->modifiers & MOD_CTRL)
     eina_strbuf_append_length(buf, "Ctrl + ", strlen("Ctrl + "));
   if (sc->modifiers & MOD_ALT)
     eina_strbuf_append_length(buf, "Alt + ", strlen("Alt + "));
   if (sc->modifiers & MOD_SHIFT)
     eina_strbuf_append_length(buf, "Shift + ", strlen("Shift + "));
   if (sc->modifiers & MOD_SUPER)
     eina_strbuf_append_length(buf, "Super + ", strlen("Super + "));
   if (sc->modifiers & MOD_META)
     eina_strbuf_append_length(buf, "Meta + ", strlen("Meta + "));
   if (sc->modifiers & MOD_HYPER)
     eina_strbuf_append_length(buf, "Hyper + ", strlen("Hyper + "));
   if (sc->modifiers & MOD_CAPS)
     eina_strbuf_append_length(buf, "Caps Lock + ", strlen("Caps Lock + "));
   eina_strbuf_append_length(buf, sc->keyname, strlen(sc->keyname));
   result = eina_strbuf_string_steal(buf);
   eina_strbuf_free(buf);
   return result;
}
static void
_add_shortcut(Shortcut_Type type_press,
              Shortcut_Type type_unpress,
              unsigned int modifiers,
              unsigned int keycode,
              char *keyname)
{
   Shortcut *sc;
   assert(ap.shortcuts != NULL);

   sc = mem_calloc(1, sizeof(Shortcut));
   sc->type_press = type_press;
   sc->type_unpress = type_unpress;
   sc->keycode = keycode;
   sc->keyname = keyname;
   sc->modifiers = modifiers;
   sc->combination = _combination_string_get(sc);
   ap.shortcuts->shortcuts = eina_list_sorted_insert(ap.shortcuts->shortcuts,
                                                       (Eina_Compare_Cb)_shortcut_cmp, sc);
}

static void
_default_shortcuts_add()
{
   assert(ap.shortcuts != NULL);
#if HAVE_TIZEN
   _add_shortcut(SHORTCUT_TYPE_QUIT, SHORTCUT_TYPE_NONE,
                 MOD_ALT, 70, "F4" /*q*/);
#else
   _add_shortcut(SHORTCUT_TYPE_QUIT, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 24, "q" /*q*/);
#endif

   _add_shortcut(SHORTCUT_TYPE_UNDO, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 52, "z" /*z*/);
   _add_shortcut(SHORTCUT_TYPE_REDO, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 29, "y"/*y*/);
   _add_shortcut(SHORTCUT_TYPE_SAVE, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 39, "s"/*s*/);
   _add_shortcut(SHORTCUT_TYPE_ADD_GROUP, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 57, "n"/*n*/);
   _add_shortcut(SHORTCUT_TYPE_ADD_PART, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 24, "q"/*q*/);
   _add_shortcut(SHORTCUT_TYPE_ADD_STATE, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 25, "w"/*w*/);
   _add_shortcut(SHORTCUT_TYPE_ADD_ITEM, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 26, "e"/*e*/);
   _add_shortcut(SHORTCUT_TYPE_ADD_PROGRAM, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 27, "r"/*r*/);
   _add_shortcut(SHORTCUT_TYPE_ADD_DATA_ITEM, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 28, "t"/*t*/);
   _add_shortcut(SHORTCUT_TYPE_DEL, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 119, "Delete"/*del*/);
   _add_shortcut(SHORTCUT_TYPE_STATE_NEXT, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 39, "s"/*s*/);
   _add_shortcut(SHORTCUT_TYPE_PART_NEXT, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 53, "x"/*x*/);
   _add_shortcut(SHORTCUT_TYPE_PART_PREV, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 52, "z"/*z*/);
   _add_shortcut(SHORTCUT_TYPE_PART_SHOWHIDE, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 43, "h"/*h*/);
   _add_shortcut(SHORTCUT_TYPE_ALL_PARTS_SHOWHIDE, SHORTCUT_TYPE_NONE,
                 MOD_SHIFT, 43, "h"/*h*/);

   _add_shortcut(SHORTCUT_TYPE_TAB_NUM1, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 10, "1"/*1*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_NUM2, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 11, "2"/*2*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_NUM3, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 12, "3"/*3*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_NUM4, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 13, "4"/*4*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_NUM5, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 14, "5"/*5*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_NUM6, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 15, "6"/*6*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_NUM7, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 16, "7"/*7*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_NUM8, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 17, "8"/*8*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_NUM9, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 18, "9"/*9*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_NUM10, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 19, "0"/*0*/);

   _add_shortcut(SHORTCUT_TYPE_TAB_NEXT, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 23, "Tab"/*TAB*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_PREV, SHORTCUT_TYPE_NONE,
                 MOD_CTRL|MOD_SHIFT, 23, "Tab"/*TAB*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_NEXT, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 117, "Next"/*PG_DOWN*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_PREV, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 112, "Prior"/*PG_UP*/);

   _add_shortcut(SHORTCUT_TYPE_TAB_CLOSE, SHORTCUT_TYPE_NONE,
                 MOD_CTRL, 25, "w"/*w*/);

   _add_shortcut(SHORTCUT_TYPE_HELP, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 67, "F1"/*F1*/);
   _add_shortcut(SHORTCUT_TYPE_MODE_NORMAL, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 68, "F2"/*F2*/);
   _add_shortcut(SHORTCUT_TYPE_MODE_CODE, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 69, "F3"/*F3*/);
   _add_shortcut(SHORTCUT_TYPE_MODE_DEMO, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 70, "F4"/*F4*/);

   _add_shortcut(SHORTCUT_TYPE_TAB_IMAGE_MANAGER, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 73, "F7"/*F7*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_SOUND_MANAGER, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 74, "F8"/*F8*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_STYLE_MANAGER, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 75, "F9"/*F9*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_COLOR_CLASS_MANAGER, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 76, "F10"/*F10*/);
   _add_shortcut(SHORTCUT_TYPE_TAB_SCRIPT_MANAGER, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 77, "F11"/*F11*/);

   _add_shortcut(SHORTCUT_TYPE_ZOOM_IN, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 86, "KP_Add"/*KP_+*/);
   _add_shortcut(SHORTCUT_TYPE_ZOOM_OUT, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 82, "KP_Subtract"/*KP_-*/);
   _add_shortcut(SHORTCUT_TYPE_ZOOM_RESET, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 106, "KP_Divide"/*KP_/ */);
   _add_shortcut(SHORTCUT_TYPE_OBJECT_AREA, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 32, "o"/*o*/);

   _add_shortcut(SHORTCUT_TYPE_CANCEL, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 9, "Escape"/*ESC*/);
   _add_shortcut(SHORTCUT_TYPE_DONE, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 36, "Return"/*ENTER*/);
   _add_shortcut(SHORTCUT_TYPE_DONE, SHORTCUT_TYPE_NONE,
                 MOD_NONE, 104, "KP_Enter"/*KP_ENTER*/);

}

/*=============================================*/
/*               PUBLIC API                    */
/*=============================================*/
Eina_Bool
shortcuts_profile_load(Profile *profile __UNUSED__)
{
   return false;
}

Eina_Bool
shortcuts_init(void)
{
   assert(ap.shortcuts == NULL);
   assert(ap.win != NULL);

   ap.shortcuts = mem_calloc(1, sizeof(Shortcut_Module));

   ap.shortcuts->shortcuts_wheel_handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_WHEEL,
                                                                   _mouse_wheel_event_cb,
                                                                   NULL);
   ap.shortcuts->shortcuts_handler = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
                                                   _key_press_event_cb,
                                                   NULL);
   ap.shortcuts->shortcuts_handler_unpress = ecore_event_handler_add(ECORE_EVENT_KEY_UP,
                                                                      _key_unpress_event_cb,
                                                                      NULL);
   evas_object_smart_callback_add(ap.win, signals.elm.win.unfocused, _win_unfocused_cb, NULL);

   ap.shortcuts->shortcuts = ((Profile *)profile_get())->shortcuts;

   if (!ap.shortcuts->shortcuts)
     {
        _default_shortcuts_add();
        ((Profile *)profile_get())->shortcuts = ap.shortcuts->shortcuts;
     }
   return true;
}

Eina_Bool
shortcuts_shutdown(void)
{
   assert(ap.shortcuts != NULL);
   assert(ap.shortcuts->shortcuts_handler != NULL);

   ecore_event_handler_del(ap.shortcuts->shortcuts_wheel_handler);
   ap.shortcuts->shortcuts_wheel_handler = NULL;
   ecore_event_handler_del(ap.shortcuts->shortcuts_handler);
   ap.shortcuts->shortcuts_handler = NULL;
   ecore_event_handler_del(ap.shortcuts->shortcuts_handler_unpress);
   ap.shortcuts->shortcuts_handler_unpress = NULL;
   free(ap.shortcuts);
   ap.shortcuts = NULL;

   return true;
}


void
shortcuts_object_push(Evas_Object *obj)
{
   assert(obj != NULL);

   handlers_stack = eina_list_prepend(handlers_stack, obj);
}

void
shortcuts_object_check_pop(Evas_Object *obj)
{
   assert(obj != NULL);
   assert(handlers_stack != NULL);
   assert(obj == eina_list_data_get(handlers_stack));

   handlers_stack = eina_list_remove_list(handlers_stack, handlers_stack);
}

void
shortcuts_shortcut_send(Shortcut_Type type)
{
   _shortcut_handle(type);
}



Eina_Bool
shortcuts_shortcut_new_set(Shortcut_Type type, Evas_Event_Key_Up *ev)
{
   Shortcut sc, *shortcut;
   int ctrl, alt, shift, win, meta;

   ctrl = evas_key_modifier_is_set(ev->modifiers, "Control");
   alt = evas_key_modifier_is_set(ev->modifiers, "Alt");
   shift = evas_key_modifier_is_set(ev->modifiers, "Shift");
   win = evas_key_modifier_is_set(ev->modifiers, "Super");
   meta = evas_key_modifier_is_set(ev->modifiers, "Meta") ||
          evas_key_modifier_is_set(ev->modifiers, "AltGr") ||
          evas_key_modifier_is_set(ev->modifiers, "ISO_Level3_Shift");

   sc.modifiers = 0;
   if (ctrl) {sc.modifiers |= MOD_CTRL; }
   if (alt)  {sc.modifiers |= MOD_ALT;  }
   if (shift){sc.modifiers |= MOD_SHIFT;}
   if (win)  {sc.modifiers |= MOD_SUPER;}
   if (meta) {sc.modifiers |= MOD_META; }

   sc.keycode = _keycode_convert(ev->keycode);
   sc.keyname = (char *)ev->keyname;
   sc.type_press = type;
   sc.type_unpress = SHORTCUT_TYPE_NONE;

   Eina_List *l=NULL, *ln = NULL;
   /* check if shortcut combination already uses */
   shortcut = eina_list_search_sorted(ap.shortcuts->shortcuts, (Eina_Compare_Cb)_shortcut_cmp, &sc);
   if (shortcut) return false;

   EINA_LIST_FOREACH_SAFE(ap.shortcuts->shortcuts, l, ln, shortcut)
     {
        if (sc.type_press == shortcut->type_press)
          {
             ap.shortcuts->shortcuts = eina_list_remove_list(ap.shortcuts->shortcuts, l);
             break;
          }
        shortcut = NULL;
     }

   if (shortcut)
     {
        shortcut->modifiers = sc.modifiers;
        shortcut->keycode = sc.keycode;
        shortcut->keyname = strdup(sc.keyname);
        shortcut->combination = _combination_string_get(shortcut);
        ap.shortcuts->shortcuts = eina_list_sorted_insert(ap.shortcuts->shortcuts,
                                                          (Eina_Compare_Cb) _shortcut_cmp, shortcut);
        ((Profile *)profile_get())->shortcuts = ap.shortcuts->shortcuts;
     }
   else return false;

   return true;
}

const char *
shortcuts_shortcut_combination_get(Shortcut_Type type)
{
   Eina_List *l, *l_n = NULL;
   Shortcut *shortcut = NULL;
   EINA_LIST_FOREACH_SAFE(ap.shortcuts->shortcuts, l, l_n, shortcut)
     {
        if (shortcut->type_press ==  type)
          {
             break;
          }
     }
   if (shortcut == NULL) return NULL;

   return shortcut->combination;
}

void
shortcuts_disabled_set(Eina_Bool disabled)
{
   ap.shortcuts->disabled = disabled;
}

void
shortcuts_shortcut_reset()
{
   assert(ap.shortcuts != NULL);
   Eina_List *l = NULL, *ln = NULL;
   Shortcut *shortcut = NULL;
   Eina_List *default_shortcuts = NULL;

   EINA_LIST_FOREACH_SAFE(ap.shortcuts->shortcuts, l, ln, shortcut)
     {
        ap.shortcuts->shortcuts = eina_list_remove_list(ap.shortcuts->shortcuts, l);
        switch (shortcut->type_press)
          {
#if HAVE_TIZEN
           case SHORTCUT_TYPE_QUIT:
              shortcut->modifiers =  MOD_ALT;
              shortcut->keycode = 70;
              shortcut->keyname =  "F4";
              break;
#else
           case SHORTCUT_TYPE_QUIT:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keycode = 24;
              shortcut->keyname =  "q";
              break;
#endif
           case SHORTCUT_TYPE_UNDO:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keycode = 52;
              shortcut->keyname =  "z";
              break;
           case SHORTCUT_TYPE_REDO:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "y";
              break;
           case SHORTCUT_TYPE_SAVE:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "s";
              break;
           case SHORTCUT_TYPE_ADD_GROUP:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "n";
              break;
           case SHORTCUT_TYPE_ADD_PART:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "q";
              break;
           case SHORTCUT_TYPE_ADD_STATE:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "w";
              break;
           case SHORTCUT_TYPE_ADD_ITEM:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "e";
              break;
           case SHORTCUT_TYPE_ADD_PROGRAM:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "r";
              break;
           case SHORTCUT_TYPE_ADD_DATA_ITEM:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "t";
              break;
           case SHORTCUT_TYPE_DEL:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "Delete";
              break;
           case SHORTCUT_TYPE_STATE_NEXT:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "s";
              break;
           case SHORTCUT_TYPE_PART_NEXT:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "x";
              break;
           case SHORTCUT_TYPE_PART_PREV:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "z";
              break;
           case SHORTCUT_TYPE_PART_SHOWHIDE:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "h";
              break;
           case SHORTCUT_TYPE_ALL_PARTS_SHOWHIDE:
              shortcut->modifiers =  MOD_SHIFT;
              shortcut->keyname =  "h";
              break;
           case SHORTCUT_TYPE_TAB_NUM1:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "1";
              break;
           case SHORTCUT_TYPE_TAB_NUM2:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "2";
              break;
           case SHORTCUT_TYPE_TAB_NUM3:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "3";
              break;
           case SHORTCUT_TYPE_TAB_NUM4:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "4";
              break;
           case SHORTCUT_TYPE_TAB_NUM5:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "5";
              break;
           case SHORTCUT_TYPE_TAB_NUM6:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "6";
              break;
           case SHORTCUT_TYPE_TAB_NUM7:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "7";
              break;
           case SHORTCUT_TYPE_TAB_NUM8:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "8";
              break;
           case SHORTCUT_TYPE_TAB_NUM9:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "9";
              break;
           case SHORTCUT_TYPE_TAB_NUM10:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "0";
              break;
           case SHORTCUT_TYPE_TAB_NEXT:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "Tab";
              break;
           case SHORTCUT_TYPE_TAB_PREV:
              shortcut->modifiers =  MOD_CTRL | MOD_SHIFT;
              shortcut->keyname =  "Tab";
              break;
           case SHORTCUT_TYPE_TAB_CLOSE:
              shortcut->modifiers =  MOD_CTRL;
              shortcut->keyname =  "w";
              break;
           case SHORTCUT_TYPE_HELP:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "F1";
              break;
           case SHORTCUT_TYPE_MODE_NORMAL:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "F2";
              break;
           case SHORTCUT_TYPE_MODE_CODE:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "F3";
              break;
           case SHORTCUT_TYPE_MODE_DEMO:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "F4";
              break;
           case SHORTCUT_TYPE_TAB_IMAGE_MANAGER:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "F7";
              break;
           case SHORTCUT_TYPE_TAB_SOUND_MANAGER:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "F8";
              break;
           case SHORTCUT_TYPE_TAB_STYLE_MANAGER:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "F9";
              break;
           case SHORTCUT_TYPE_TAB_COLOR_CLASS_MANAGER:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "F10";
              break;
           case SHORTCUT_TYPE_TAB_SCRIPT_MANAGER:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "F11";
              break;
           case SHORTCUT_TYPE_ZOOM_IN:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "KP_Add";
              break;
           case SHORTCUT_TYPE_ZOOM_OUT:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "KP_Subtract";
              break;
           case SHORTCUT_TYPE_ZOOM_RESET:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "KP_Divide";
              break;
           case SHORTCUT_TYPE_OBJECT_AREA:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "o";
              break;
           case SHORTCUT_TYPE_CANCEL:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "Escape";
              break;
           case SHORTCUT_TYPE_DONE:
              shortcut->modifiers =  MOD_NONE;
              shortcut->keyname =  "Return";
              break;
           default:
              continue;
          }

        shortcut->combination = _combination_string_get(shortcut);
        default_shortcuts = eina_list_sorted_insert(default_shortcuts,
                                                    (Eina_Compare_Cb)_shortcut_cmp, shortcut);
     }
   ap.shortcuts->shortcuts = default_shortcuts;
   ((Profile *)profile_get())->shortcuts = ap.shortcuts->shortcuts;
   return;
}
