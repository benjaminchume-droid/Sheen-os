#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "sheen/container.h"

typedef int (*probe_fn)(const unsigned char *,size_t);
typedef struct { sheen_container_type type; const char *name; const char *mime; probe_fn match; } probe;
static int has(const unsigned char *b,size_t n,size_t off,const void *p,size_t m){return off+m<=n&&!memcmp(b+off,p,m);}
static int p_ftyp(const unsigned char*b,size_t n){return n>=8&&has(b,n,4,"ftyp",4);}
static int p_ebml(const unsigned char*b,size_t n){static const unsigned char x[]={0x1a,0x45,0xdf,0xa3};return has(b,n,0,x,sizeof(x));}
static int p_ts(const unsigned char*b,size_t n){return n>=376&&b[0]==0x47&&b[188]==0x47&&(n<376||b[376]==0x47);}
static int p_ogg(const unsigned char*b,size_t n){return has(b,n,0,"OggS",4);}
static int p_flv(const unsigned char*b,size_t n){return has(b,n,0,"FLV",3);}
static int p_riff(const unsigned char*b,size_t n){return has(b,n,0,"RIFF",4)&&(has(b,n,8,"AVI ",4)||has(b,n,8,"WAVE",4));}
static int p_ps(const unsigned char*b,size_t n){static const unsigned char x[]={0,0,1,0xba};return has(b,n,0,x,sizeof(x));}
static int p_asf(const unsigned char*b,size_t n){static const unsigned char x[]={0x30,0x26,0xb2,0x75,0x8e,0x66,0xcf,0x11,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c};return has(b,n,0,x,sizeof(x));}
static const probe probes[]={{SHEEN_CONTAINER_ISOBMFF,"isobmff","video/mp4",p_ftyp},{SHEEN_CONTAINER_MATROSKA,"matroska","video/x-matroska",p_ebml},{SHEEN_CONTAINER_MPEG_TS,"mpeg-ts","video/mp2t",p_ts},{SHEEN_CONTAINER_OGG,"ogg","application/ogg",p_ogg},{SHEEN_CONTAINER_FLV,"flv","video/x-flv",p_flv},{SHEEN_CONTAINER_RIFF,"riff","application/octet-stream",p_riff},{SHEEN_CONTAINER_MPEG_PS,"mpeg-ps","video/mpeg",p_ps},{SHEEN_CONTAINER_ASF,"asf","video/x-ms-asf",p_asf}};
const char *sheen_container_type_name(sheen_container_type t){for(size_t i=0;i<sizeof(probes)/sizeof(probes[0]);i++)if(probes[i].type==t)return probes[i].name;return "unknown";}
const char *sheen_container_mime(sheen_container_type t){for(size_t i=0;i<sizeof(probes)/sizeof(probes[0]);i++)if(probes[i].type==t)return probes[i].mime;return "application/octet-stream";}
int sheen_container_inspect_file(const char *path,sheen_container_info *info){
    if(!path||!info)return EINVAL;memset(info,0,sizeof(*info));snprintf(info->source,sizeof(info->source),"%s",path);
    struct stat st;if(stat(path,&st)<0)return errno;info->size_bytes=(uint64_t)st.st_size;info->seekable=S_ISREG(st.st_mode);
    FILE *f=fopen(path,"rb");if(!f)return errno;unsigned char buf[65536];size_t n=fread(buf,1,sizeof(buf),f);int err=ferror(f);fclose(f);if(err)return EIO;info->bytes_inspected=n;
    for(size_t i=0;i<sizeof(probes)/sizeof(probes[0]);i++)if(probes[i].match(buf,n)){info->type=probes[i].type;snprintf(info->name,sizeof(info->name),"%s",probes[i].name);snprintf(info->mime,sizeof(info->mime),"%s",probes[i].mime);return 0;}
    snprintf(info->name,sizeof(info->name),"%s","unknown");snprintf(info->mime,sizeof(info->mime),"%s","application/octet-stream");return 0;
}
