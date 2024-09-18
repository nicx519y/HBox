#include "cpp_main.hpp"
#include "bsp/board_api.h"
#include "tusb.h"
#include "net_init.h"

int cpp_main(void) 
{

    board_init();
    board_led_write(false);

    // tusb_init();
    tud_init(BOARD_TUD_RHPORT);
    net_init();

    while(1) {
        tud_task();
        service_traffic();
    }

    return 0;
}