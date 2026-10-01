#include <stdio.h>
#include <string.h>
#include "sheen/media-cast.h"
int main(int argc,char **argv){if(argc!=3){fprintf(stderr,"usage: %s DEVICE_DESCRIPTION MEDIA_URI\n",argv[0]);return 2;}sheen_cast_renderer r;if(sheen_cast_renderer_describe(argv[1],&r)){fprintf(stderr,"renderer description failed\n");return 1;}sheen_cast_media_session s={0};s.renderer=r;if(sheen_cast_media_set_uri(&s,argv[2]))return 1;if(sheen_cast_media_play(&s))return 1;printf("{\"control_url\":\"%s\",\"media_uri\":\"%s\",\"state\":\"%s\"}\n",s.renderer.control_url,s.media_uri,sheen_cast_media_state_name(s.state));return 0;}
