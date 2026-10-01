#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "sheen/config.h"
#define MAX_ENTRIES 1024
typedef struct { char section[SHEEN_CONFIG_SECTION_MAX]; char key[SHEEN_CONFIG_KEY_MAX]; char value[SHEEN_CONFIG_VALUE_MAX]; } entry;
struct sheen_config { entry entries[MAX_ENTRIES]; size_t count; };

static char *trim(char *s){while(*s==' '||*s=='\t'||*s=='\r'||*s=='\n')s++;char *e=s+strlen(s);while(e>s&&(e[-1]==' '||e[-1]=='\t'||e[-1]=='\r'||e[-1]=='\n'))*--e=0;return s;}
static entry *find_entry(sheen_config *c,const char *section,const char *key){for(size_t i=0;i<c->count;i++)if(!strcmp(c->entries[i].section,section)&&!strcmp(c->entries[i].key,key))return &c->entries[i];return NULL;}
sheen_config *sheen_config_create(void){return calloc(1,sizeof(sheen_config));}
void sheen_config_destroy(sheen_config *c){free(c);}
int sheen_config_load(sheen_config *c,const char *path){
    if(!c||!path)return EINVAL; FILE *f=fopen(path,"r"); if(!f)return errno;
    c->count=0; char section[SHEEN_CONFIG_SECTION_MAX]="",line[2048];
    while(fgets(line,sizeof(line),f)){char *p=trim(line);if(!*p||*p=='#'||*p==';')continue;
        if(*p=='['&&p[strlen(p)-1]==']'){p[strlen(p)-1]=0;snprintf(section,sizeof(section),"%s",trim(p+1));continue;}
        char *eq=strchr(p,'=');if(!eq)continue;*eq=0;char *key=trim(p);char *value=trim(eq+1);if(!*key||c->count>=MAX_ENTRIES)continue;
        entry *e=&c->entries[c->count++];snprintf(e->section,sizeof(e->section),"%s",section);snprintf(e->key,sizeof(e->key),"%s",key);snprintf(e->value,sizeof(e->value),"%s",value);
    }
    int rc=ferror(f)?errno:0;fclose(f);return rc;
}
const char *sheen_config_get(const sheen_config *c,const char *section,const char *key){if(!c)return NULL;for(size_t i=0;i<c->count;i++)if(!strcmp(c->entries[i].section,section)&&!strcmp(c->entries[i].key,key))return c->entries[i].value;return NULL;}
int sheen_config_set(sheen_config *c,const char *section,const char *key,const char *value){if(!c||!section||!key||!value||!section[0]||!key[0]||strlen(section)>=SHEEN_CONFIG_SECTION_MAX||strlen(key)>=SHEEN_CONFIG_KEY_MAX||strlen(value)>=SHEEN_CONFIG_VALUE_MAX)return EINVAL;entry *e=find_entry(c,section,key);if(!e){if(c->count>=MAX_ENTRIES)return ENOSPC;e=&c->entries[c->count++];snprintf(e->section,sizeof(e->section),"%s",section);snprintf(e->key,sizeof(e->key),"%s",key);}snprintf(e->value,sizeof(e->value),"%s",value);return 0;}
int sheen_config_save_atomic(const sheen_config *c,const char *path){
    if(!c||!path)return EINVAL;char tmp[4096];int n=snprintf(tmp,sizeof(tmp),"%s.tmp.%ld",path,(long)getpid());if(n<0||(size_t)n>=sizeof(tmp))return ENAMETOOLONG;
    char dir[4096];snprintf(dir,sizeof(dir),"%s",path);char *slash=strrchr(dir,'/');if(slash){*slash=0;char *p=dir;if(*p==0)p="/";for(char *q=p+1;*q;q++)if(*q=='/'){*q=0;mkdir(p,0755);*q='/';}mkdir(p,0755);}
    int fd=open(tmp,O_WRONLY|O_CREAT|O_TRUNC|O_CLOEXEC,0644);if(fd<0)return errno;FILE *f=fdopen(fd,"w");if(!f){close(fd);unlink(tmp);return errno;}
    const char *last="";for(size_t i=0;i<c->count;i++){entry const *e=&c->entries[i];if(strcmp(last,e->section)){fprintf(f,"[%s]\\n",e->section);last=e->section;}fprintf(f,"%s=%s\\n",e->key,e->value);}
    fflush(f);fsync(fileno(f));if(fclose(f)!=0){unlink(tmp);return errno;}if(rename(tmp,path)<0){int e=errno;unlink(tmp);return e;}return 0;
}
int sheen_config_dump(const sheen_config *c,int fd){if(!c)return EINVAL;const char *last="";char buf[1400];for(size_t i=0;i<c->count;i++){entry const *e=&c->entries[i];if(strcmp(last,e->section)){dprintf(fd,"[%s]\\n",e->section);last=e->section;}dprintf(fd,"%s=%s\\n",e->key,e->value);}return 0;}
