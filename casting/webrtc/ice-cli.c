#include <signal.h>
#include <stdio.h>
#include "sheen/webrtc.h"
int main(void){
    sheen_webrtc_ice *i=sheen_webrtc_ice_open(19654);
    if(!i)return 1;
    puts("ready");
    fflush(stdout);
    for(;;) {
        int r=sheen_webrtc_ice_process(i);
        if(r) break;
    }
    sheen_webrtc_ice_close(i);
    return 0;
}
