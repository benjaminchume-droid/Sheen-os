#include <stdio.h>
#include <stdlib.h>
#include "sheen/wfd.h"
int main(void) {
    sheen_wfd_config c={.rtsp_port=19554,.rtp_port=19555,.width=1920,.height=1080,.fps=30};
    sheen_wfd_server *s=sheen_wfd_start(&c);
    if(!s) return 1;
    printf("%d\n",sheen_wfd_port(s));
    fflush(stdout);
    getchar();
    sheen_wfd_stop(s);
    return 0;
}
