#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/network-tv.h"
static char *trim(char *s){while(*s==' '||*s=='\t'||*s=='\r'||*s=='\n')s++;char *e=s+strlen(s);while(e>s&&(e[-1]==' '||e[-1]=='\t'||e[-1]=='\r'||e[-1]=='\n'))*--e=0;return s;}
static uint64_t hash64(const void *data,size_t n,uint64_t h){const unsigned char *p=data;for(size_t i=0;i<n;i++){h^=p[i];h*=1099511628211ULL;}return h;}
static void make_id(const char *uri,char out[65]){uint64_t h=hash64(uri,strlen(uri),1469598103934665603ULL);snprintf(out,65,"%016llx",(unsigned long long)h);}
int sheen_network_catalog_validate(const sheen_network_channel *c){if(!c||!c->channel_id[0]||!c->name[0]||!c->uri[0])return EINVAL;if(strncmp(c->uri,"http://",7)&&strncmp(c->uri,"udp://",6))return ENOTSUP;return 0;}
int sheen_network_catalog_load(const char *path,sheen_network_catalog *c){if(!path||!c)return EINVAL;memset(c,0,sizeof(*c));FILE *f=fopen(path,"r");if(!f)return errno;char line[8192];while(fgets(line,sizeof(line),f)){char *p=trim(line);if(!*p||*p=='#')continue;char *a=strchr(p,'|');if(!a){fclose(f);return EINVAL;}*a++=0;char *b=strchr(a,'|');if(!b){fclose(f);return EINVAL;}*b++=0;if(c->count>=SHEEN_NETWORK_TV_MAX_CHANNELS){fclose(f);return ENOSPC;}sheen_network_channel *x=&c->channels[c->count];snprintf(x->channel_id,sizeof(x->channel_id),"%s",trim(p));snprintf(x->name,sizeof(x->name),"%s",trim(a));snprintf(x->uri,sizeof(x->uri),"%s",trim(b));x->enabled=1;if(sheen_network_catalog_validate(x)){fclose(f);return EINVAL;}if(strchr(x->name,'\n'))x->name[strcspn(x->name,"\n")]=0;if(!x->channel_id[0])make_id(x->uri,x->channel_id);c->count++;}int rc=ferror(f)?errno:0;fclose(f);return rc;}
int sheen_network_catalog_get(const sheen_network_catalog *c,const char *id,sheen_network_channel *out){if(!c||!id||!out)return EINVAL;for(size_t i=0;i<c->count;i++)if(!strcmp(c->channels[i].channel_id,id)){*out=c->channels[i];return 0;}return ENOENT;}
sheen_live_session *sheen_network_open(const sheen_network_channel *c){if(sheen_network_catalog_validate(c))return NULL;return sheen_live_open(c->uri);}
