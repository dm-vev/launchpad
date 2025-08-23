#include <stdint.h>
#include "elf/elf_symbol.h"
#include "platform.h"

void launchpad_init(void)
{
    _register_symbol("launchpad_platform", (void *)launchpad_platform);
}
