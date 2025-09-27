#if !defined(TUI_H)
#define TUI_H

#include "header.h"
#include "termios.h"
#include "unistd.h"

typedef struct
{
  int width;
  int height;
  char *buf;

  int stop;

} tui_ctx_t;

/* terminal commands */
#define clear() printf ("\033[2J")
#define move(y, x) printf ("\033[%d;%dH", (y), (x))
#define hide_cursor() printf ("\033[?25l")
#define show_cursor() printf ("\033[?25h")
#define TEXT_RED(X) printf ("\033[1;31m%s\033[0m", (X))
#define TEXT_GREEN(X) printf ("\033[1;32m%s\033[0m", (X))
#define TEXT_YELLOW(X) printf ("\033[1;33m%s\033[0m", (X))
#define TEXT_BLUE(X) printf ("\033[1;34m%s\033[0m", (X))

#if defined(__cplusplus)
extern "C"
{
#endif // __cplusplus

  void tui_init (); /* enable raw mode */
  tui_ctx_t *tui_new (int w, int h);
  void tui_disp (tui_ctx_t *);
  void tui_destroy (tui_ctx_t *);
  void tui_kill (); /* restore state */

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // TUI_H
