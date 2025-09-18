#include <log/log.h>

void SDL_Main();

int main() {
#ifdef DEBUG
  // output all logs to stderr
  log_set_level(0);
#else
  // output only error and fatal if release
  log_set_level(4);
#endif
  SDL_Main();
  return 0;
}
