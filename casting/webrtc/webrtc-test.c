#include <stdio.h>
#include <string.h>
#include "sheen/webrtc.h"
int main(void){
    const char *sdp =
        "v=0\r\n"
        "a=ice-ufrag:abc123\r\n"
        "a=ice-pwd:secret456\r\n"
        "m=video 9 RTP/AVP 96\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=candidate:1 1 UDP 2130706431 192.0.2.10 5000 typ host\r\n";
    sheen_webrtc_offer o;
    if(sheen_webrtc_parse_sdp(sdp,&o)) return 1;
    if(strcmp(o.ice_ufrag,"abc123")||strcmp(o.ice_pwd,"secret456")) return 2;
    if(!o.has_video||o.codec_count!=1||o.codecs[0].payload_type!=96) return 3;
    if(strcmp(o.codecs[0].encoding,"H264")||o.candidates[0].port!=5000) return 4;
    puts("sdp ok");
    return 0;
}
