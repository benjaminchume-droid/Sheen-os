#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/drm.h>
#include <linux/drm_mode.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
static void esc(const char *s){putchar(34);for(;s&&*s;s++){if(*s==34||*s==92){putchar(92);putchar(*s);}else if((unsigned char)*s<32)putchar(32);else putchar(*s);}putchar(34);}
static const char *conn_name(unsigned int t){switch(t){case DRM_MODE_CONNECTOR_VGA:return "VGA";case DRM_MODE_CONNECTOR_DVII:return "DVI-I";case DRM_MODE_CONNECTOR_DVID:return "DVI-D";case DRM_MODE_CONNECTOR_DVIA:return "DVI-A";case DRM_MODE_CONNECTOR_Composite:return "Composite";case DRM_MODE_CONNECTOR_SVIDEO:return "SVIDEO";case DRM_MODE_CONNECTOR_LVDS:return "LVDS";case DRM_MODE_CONNECTOR_Component:return "Component";case DRM_MODE_CONNECTOR_9PinDIN:return "DIN";case DRM_MODE_CONNECTOR_DisplayPort:return "DisplayPort";case DRM_MODE_CONNECTOR_HDMIA:return "HDMI-A";case DRM_MODE_CONNECTOR_HDMIB:return "HDMI-B";case DRM_MODE_CONNECTOR_TV:return "TV";case DRM_MODE_CONNECTOR_eDP:return "eDP";case DRM_MODE_CONNECTOR_VIRTUAL:return "Virtual";case DRM_MODE_CONNECTOR_DSI:return "DSI";case DRM_MODE_CONNECTOR_DPI:return "DPI";case DRM_MODE_CONNECTOR_WRITEBACK:return "Writeback";default:return "Unknown";}}
static void dump_connector(int fd,unsigned int cid,int *first_connector){
    struct drm_mode_get_connector c={0};c.connector_id=cid;
    if(ioctl(fd,DRM_IOCTL_MODE_GETCONNECTOR,&c)<0)return;
    struct drm_mode_modeinfo *modes=c.count_modes?calloc(c.count_modes,sizeof(*modes)):NULL;
    uint32_t *encoders=c.count_encoders?calloc(c.count_encoders,sizeof(*encoders)):NULL;
    c.modes_ptr=(uintptr_t)modes;c.encoders_ptr=(uintptr_t)encoders;
    if(ioctl(fd,DRM_IOCTL_MODE_GETCONNECTOR,&c)<0){free(modes);free(encoders);return;}
    if(!*first_connector) putchar(',');
    *first_connector=0;
    printf("{\"id\":%u,\"type\":",c.connector_type);esc(conn_name(c.connector_type));
    printf(",\"type_id\":%u,\"connection\":%u,\"mm_width\":%u,\"mm_height\":%u,\"modes\":[",c.connector_type_id,c.connection,c.mm_width,c.mm_height);
    for(uint32_t i=0;i<c.count_modes;i++){if(i)putchar(',');printf("{\"name\":");esc(modes[i].name);printf(",\"width\":%u,\"height\":%u,\"refresh\":%u}",modes[i].hdisplay,modes[i].vdisplay,modes[i].vrefresh);}
    printf("]}");free(modes);free(encoders);
}
static int dump_card(const char *path,const char *name,int *first_card){
    int fd=open(path,O_RDWR|O_CLOEXEC);if(fd<0){return -1;}
    struct drm_mode_card_res r={0};if(ioctl(fd,DRM_IOCTL_MODE_GETRESOURCES,&r)<0){close(fd);return -1;}
    uint32_t *fbs=r.count_fbs?calloc(r.count_fbs,sizeof(*fbs)):NULL;
    uint32_t *crtcs=r.count_crtcs?calloc(r.count_crtcs,sizeof(*crtcs)):NULL;
    uint32_t *connectors=r.count_connectors?calloc(r.count_connectors,sizeof(*connectors)):NULL;
    uint32_t *encoders=r.count_encoders?calloc(r.count_encoders,sizeof(*encoders)):NULL;
    r.fb_id_ptr=(uintptr_t)fbs;r.crtc_id_ptr=(uintptr_t)crtcs;r.connector_id_ptr=(uintptr_t)connectors;r.encoder_id_ptr=(uintptr_t)encoders;
    if(ioctl(fd,DRM_IOCTL_MODE_GETRESOURCES,&r)<0){free(fbs);free(crtcs);free(connectors);free(encoders);close(fd);return -1;}
    if(!*first_card) putchar(',');
    *first_card=0;
    printf("{\"path\":");esc(path);printf(",\"name\":");esc(name);printf(",\"resource_counts\":{\"framebuffers\":%u,\"crtcs\":%u,\"connectors\":%u,\"encoders\":%u},\"connectors\":[",r.count_fbs,r.count_crtcs,r.count_connectors,r.count_encoders);
    int first_connector=1;for(uint32_t i=0;i<r.count_connectors;i++)dump_connector(fd,connectors[i],&first_connector);
    printf("]}");free(fbs);free(crtcs);free(connectors);free(encoders);close(fd);return 0;
}
int main(void){
    DIR *d=opendir("/sys/class/drm");if(!d){printf("{\"available\":false,\"cards\":[]}\n");return 0;}
    printf("{\"available\":true,\"cards\":[");struct dirent *e;int first_card=1;int found=0;
    while((e=readdir(d))){if(strncmp(e->d_name,"card",4)!=0)continue;const char *p=e->d_name+4;if(!*p)continue;int digits=1;for(const char *q=p;*q;q++)if(*q<'0'||*q>'9'){digits=0;break;}if(!digits)continue;char path[128];int n=snprintf(path,sizeof(path),"/dev/dri/%s",e->d_name);if(n<0||(size_t)n>=sizeof(path))continue;if(dump_card(path,e->d_name,&first_card)==0)found=1;}
    closedir(d);printf("],\"accessible_cards\":%s}\n",found?"true":"false");return 0;
}
