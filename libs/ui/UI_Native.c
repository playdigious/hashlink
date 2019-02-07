#include <hl.h>

#include <SDL.h>
#include <SDL_messagebox.h>
#include <SDL_syswm.h>

#include <stdbool.h>
#include <unistd.h>

#define MAX_BUTTON_ON_MESSAGEBOX 3

#if defined(HL_ANDROID)
extern SDL_Window* Android_Window;
#elif defined(HL_IOS) || defined(HL_TVOS)
SDL_Window* global_sdl_window = NULL;
#endif

typedef struct _ui_sentinel ui_sentinel;
struct _ui_sentinel {
	hl_thread *thread;
	hl_thread *original;
	void *callback;
	double timeout;
	int ticks;
	bool pause;
} ui_sentinel_d ;

typedef struct _ui_window ui_window;

struct _ui_window {
	ui_window* parent;
	SDL_Window* win;
	SDL_MessageBoxData datas;
	vclosure* callb;
	int width;
	int height;
} ui_window_d ;

const SDL_MessageBoxColorScheme colorScheme = {
		{ /* .colors (.r, .g, .b) */
				/* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
				{ 255, 255, 255 },
				/* [SDL_MESSAGEBOX_COLOR_TEXT] */
				{   1,   1,   1 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
				{ 150, 150, 150 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
				{ 200, 200, 200 },
				/* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
				{ 225, 225, 225 }
		}
};

ui_window* mainWindow;
ui_window* currentWindow;

void ui_ui_sentinel_tick (ui_sentinel* sent)
{
	/* Removed by Florian BOEUF on 2019 Feb 7th
	As part of integrating v1.1 of Deadcells
	While merging with a newer version of hashlink, some thread related
	functions have been removed or replaced. As such, the sentinel code
	does not compile anymore. 
	Taking ui_ui_sentinel_tick code from ui_stub.c, doing nothing.
	Code kept for archives

	sent->ticks++;
	*/
}

void ui_ui_sentinel_pause (ui_sentinel* sent, bool b)
{
	/* Removed by Florian BOEUF on 2019 Feb 7th
	As part of integrating v1.1 of Deadcells
	While merging with a newer version of hashlink, some thread related
	functions have been removed or replaced. As such, the sentinel code
	does not compile anymore. 
	Taking ui_ui_sentinel_pause code from ui_stub.c, doing nothing.
	Code kept for archives

	sent->pause = b;
	*/
}

/* see ui_ui_start_sentinel
static void sentinel_loop( ui_sentinel *s )
{
	int time_ms = (int)((s->timeout * 1000.) / 16.);
	hl_thread_registers *regs = (hl_thread_registers*)malloc(sizeof(int_val) * hl_thread_context_size());
	int eip = hl_thread_context_index("eip");
	int esp = hl_thread_context_index("esp");
	while( true ) {
		int k = 0;
		int tick = s->ticks;
		while( true ) {
			sleep(time_ms);
			if( tick != s->ticks || s->pause ) break;
			if( hl_is_blocking() ) continue;
			k++;
			if( k == 16 ) {
				// pause
				hl_thread_pause(s->original, true);
				hl_thread_get_context(s->original,regs);
				// simulate a call
				int_val* resp = (int_val*)regs[esp];
				*--resp = regs[eip];
				*--resp = regs[esp];
				regs[eip] = (int_val)s->callback;
				// resume
				hl_thread_set_context(s->original,regs);
				hl_thread_pause(s->original, false);
				break;
			}
		}
	}
}*/

ui_sentinel* ui_ui_start_sentinel (double timeout, vclosure* vc)
{
	/* Removed by Florian BOEUF on 2019 Feb 7th
	As part of integrating v1.1 of Deadcells
	While merging with a newer version of hashlink, some thread related
	functions have been removed or replaced. As such, the sentinel code
	does not compile anymore. 
	Taking ui_ui_start_sentinel code from ui_stub.c, and returning null only.
	Code kept for archives

	ui_sentinel *sent = &ui_sentinel_d;
	if( vc->hasValue ) hl_error("Cannot set sentinel on closure callback");
#ifdef HL_DEBUG
	timeout *= 2;
#endif
	sent->timeout = timeout;
	sent->ticks = 0;
	sent->pause = false;
	sent->original = hl_thread_current();
	sent->callback = vc->fun;
	sent->thread = hl_thread_start(sentinel_loop,sent,false);
	return sent;*/
	return NULL;
}

bool ui_ui_sentinel_is_paused(ui_sentinel* sent)
{
	/* Removed by Florian BOEUF on 2019 Feb 7th
	As part of integrating v1.1 of Deadcells
	While merging with a newer version of hashlink, some thread related
	functions have been removed or replaced. As such, the sentinel code
	does not compile anymore. 
	Taking ui_ui_sentinel_is_paused code from ui_stub.c, doing nothing.
	Code kept for archives

	return sent->pause;
	*/
	return false;
}

void ui_ui_win_destroy (ui_window* win)
{
	if(win->parent->win == NULL)
	{
		currentWindow = mainWindow;
	}
	else
	{
		currentWindow = win->parent;
	}

	free(win->callb);
    if(win->win != NULL)
        SDL_DestroyWindow(win->win);
    free(win);
}

Uint32 buttonId = 0;

ui_window* ui_ui_button_new (ui_window* win, vbyte* text, vclosure* callb)
{
	SDL_MessageBoxButtonData buttonData;
	buttonData.text = hl_to_utf8(text);
	buttonData.flags = buttonId;
	buttonData.buttonid = buttonId;
	SDL_MessageBoxButtonData* buttonDatas = malloc(sizeof(SDL_MessageBoxButtonData) * (win->datas.numbuttons + 1));
	for (int i = 0; i < win->datas.numbuttons; ++i)
	{
		buttonDatas[i] = win->datas.buttons[i];
	}
	win->callb[win->datas.numbuttons] = *callb;
	buttonDatas[win->datas.numbuttons++] = buttonData;
	win->datas.buttons = buttonDatas;

	buttonId++;

	return win;
}

#if defined(HL_ANDROID)
ANativeWindow* getWindowFromInfo(SDL_SysWMinfo info)
{
	return info.info.android.window;
}
#elif defined(HL_IOS) || defined(HL_TVOS)
UIWindow* getWindowFromInfo(SDL_SysWMinfo info)
{
	return info.info.uikit.window;
}
#endif

ui_window* ui_ui_winlog_new (vbyte* title, int w, int h)
{
	buttonId = 0;

    ui_window* newWindow = malloc(sizeof(ui_window));
    memset(newWindow, 0, sizeof(ui_window));

	newWindow->width = w;
	newWindow->height = h;
	newWindow->callb = malloc(sizeof(vclosure) * MAX_BUTTON_ON_MESSAGEBOX);

	SDL_MessageBoxData datas;

	datas.title = hl_to_utf8(title);
	datas.flags = SDL_MESSAGEBOX_ERROR;

	newWindow->parent = currentWindow;
	datas.window = currentWindow->win;
	datas.numbuttons = 0;
	datas.message = "";
	datas.colorScheme = &colorScheme;

	newWindow->datas = datas;

	currentWindow = newWindow;
	return newWindow;
}

void ui_ui_stop_loop ()
{

}

int ui_ui_loop (bool blocking)
{
	int buttonid = 0;
#if defined(HL_MOBILE)
	SDL_ShowMessageBox(&currentWindow->datas, &buttonid);
	if( currentWindow->callb )
	{
		vclosure* callb = &currentWindow->callb[buttonid];
		if(callb)
		{
			hl_dyn_call(callb,NULL,0);
		}
	}
#endif
	return buttonid;
}

int ui_ui_dialog (vbyte* title, vbyte* message, int flags)
{
#if defined(HL_MOBILE)
    Uint32 sdlFlags = (flags & 2) != 0 ? SDL_MESSAGEBOX_ERROR : (flags & 1) != 0 ? SDL_MESSAGEBOX_WARNING : SDL_MESSAGEBOX_INFORMATION;
    return SDL_ShowSimpleMessageBox(sdlFlags, hl_to_utf8(title), hl_to_utf8(message), mainWindow->win);
#endif
	return 0;
}

void ui_ui_init ()
{
    mainWindow = malloc(sizeof(ui_window));
    memset(mainWindow, 0, sizeof(ui_window));
#if defined(HL_ANDROID)
	mainWindow->win = Android_Window;
#elif defined(HL_IOS) || defined(HL_TVOS)
    mainWindow->win = global_sdl_window;
#endif
	currentWindow = mainWindow;
}

void ui_ui_winlog_set_text (ui_window* win, vbyte* text, bool autoscroll)
{
#if defined(HL_MOBILE)
	win->datas.message = hl_to_utf8(text);
#endif
}
