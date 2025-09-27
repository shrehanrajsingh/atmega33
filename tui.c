#include "tui.h"

struct termios oldt, newt;

void
tui_init ()
{
  tcgetattr (STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr (STDIN_FILENO, TCSANOW, &newt);
}

tui_ctx_t *
tui_new (int w, int h)
{
  tui_ctx_t *t = malloc (sizeof (*t));
  t->width = w;
  t->height = h;
  t->stop = 0;
  t->buf = malloc (t->width * t->height);
  memset (t->buf, 0, t->width * t->height);

  return t;
}

void
tui_disp (tui_ctx_t *ctx)
{
  clear ();
  hide_cursor ();

  move (1, 1);
  printf ("\033[107;1;30m        File                Edit                View "
          "          "
          "     "
          "Help        \033[0m\n\n");

  // code section
  printf ("┌────────────Code");
  for (int i = 0; i < 15; i++)
    printf ("─");
  printf ("┐");

  // inspect section
  printf ("┌───────────────────Inspect");
  for (int i = 0; i < 19; i++)
    printf ("─");
  printf ("┐\n");

  for (int i = 0; i < 19; i++)
    {
      printf ("│");
      for (int j = 0; j < 31; j++)
        putchar (' ');
      printf ("│");

      printf ("│");
      for (int j = 0; j < 45; j++)
        putchar (' ');
      printf ("│\n");
    }

  printf ("└");
  for (int i = 0; i < 31; i++)
    printf ("─");
  printf ("┘");

  printf ("└");
  for (int i = 0; i < 45; i++)
    printf ("─");
  printf ("┘\n");

  printf ("\033[1;32m$>\033[0m\n");

  //   while (!ctx->stop)
  //     ;
}

void
tui_destroy (tui_ctx_t *ctx)
{
  free (ctx->buf);
}

void
tui_kill ()
{
  show_cursor ();
  tcsetattr (STDIN_FILENO, TCSANOW, &oldt);
}