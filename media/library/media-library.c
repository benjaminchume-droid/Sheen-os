#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "sheen/media-library.h"
#include "sheen/container.h"
struct sheen_media_library { sqlite3 *db; };

static uint64_t fnv1a(const void *data,size_t n,uint64_t seed){const unsigned char *p=data;uint64_t h=seed;for(size_t i=0;i<n;i++){h^=p[i];h*=1099511628211ULL;}return h;}
static void make_id(const char *source,uint64_t size,int64_t mtime,char out[65]){
    uint64_t a=1469598103934665603ULL,b=1099511628211ULL;
    a=fnv1a(source,strlen(source),a);a=fnv1a(&size,sizeof(size),a);a=fnv1a(&mtime,sizeof(mtime),a);
    b=fnv1a(source,strlen(source),b);b=fnv1a(&mtime,sizeof(mtime),b);b=fnv1a(&size,sizeof(size),b);
    snprintf(out,65,"%016llx%016llx",(unsigned long long)a,(unsigned long long)b);
}
static int bind_item(sqlite3_stmt *s,const sheen_media_item *i){sqlite3_bind_text(s,1,i->id,-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,2,i->source,-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,3,i->container,-1,SQLITE_TRANSIENT);sqlite3_bind_int64(s,4,(sqlite3_int64)i->size_bytes);sqlite3_bind_int64(s,5,(sqlite3_int64)i->mtime_ns);return 0;}
sheen_media_library *sheen_media_library_open(const char *path){if(!path)return NULL;sheen_media_library *l=calloc(1,sizeof(*l));if(!l)return NULL;if(sqlite3_open(path,&l->db)!=SQLITE_OK){sqlite3_close(l->db);free(l);return NULL;}return l;}
void sheen_media_library_close(sheen_media_library *l){if(!l)return;if(l->db)sqlite3_close(l->db);free(l);}
int sheen_media_library_init(sheen_media_library *l){if(!l||!l->db)return EINVAL;const char *sql="PRAGMA journal_mode=WAL; CREATE TABLE IF NOT EXISTS media(id TEXT PRIMARY KEY,source TEXT UNIQUE NOT NULL,container TEXT NOT NULL,size_bytes INTEGER NOT NULL,mtime_ns INTEGER NOT NULL,added_at INTEGER NOT NULL); CREATE INDEX IF NOT EXISTS media_source_idx ON media(source);";char *err=NULL;int rc=sqlite3_exec(l->db,sql,NULL,NULL,&err);sqlite3_free(err);return rc==SQLITE_OK?0:EIO;}
int sheen_media_library_upsert(sheen_media_library *l,const sheen_media_item *i){if(!l||!l->db||!i)return EINVAL;const char *sql="INSERT INTO media(id,source,container,size_bytes,mtime_ns,added_at) VALUES(?,?,?,?,?,strftime('%s','now')) ON CONFLICT(source) DO UPDATE SET id=excluded.id,container=excluded.container,size_bytes=excluded.size_bytes,mtime_ns=excluded.mtime_ns;";sqlite3_stmt *s=NULL;if(sqlite3_prepare_v2(l->db,sql,-1,&s,NULL)!=SQLITE_OK)return EIO;bind_item(s,i);int rc=sqlite3_step(s)==SQLITE_DONE?0:EIO;sqlite3_finalize(s);return rc;}
int sheen_media_library_scan_file(sheen_media_library *l,const char *path){if(!l||!path)return EINVAL;struct stat st;if(stat(path,&st)!=0)return errno;if(!S_ISREG(st.st_mode))return ENOTSUP;sheen_container_info info;int rc=sheen_container_inspect_file(path,&info);if(rc)return rc;if(info.type==SHEEN_CONTAINER_UNKNOWN)return ENOTSUP;sheen_media_item item={0};snprintf(item.source,sizeof(item.source),"%s",path);snprintf(item.container,sizeof(item.container),"%s",info.name);item.size_bytes=(uint64_t)st.st_size;item.mtime_ns=(int64_t)st.st_mtime*1000000000LL;make_id(item.source,item.size_bytes,(uint64_t)item.mtime_ns,item.id);return sheen_media_library_upsert(l,&item);}
static int scan_dir(sheen_media_library *l,const char *root){DIR *d=opendir(root);if(!d)return errno;struct dirent *e;int rc=0;while((e=readdir(d))){if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;char path[4096];int n=snprintf(path,sizeof(path),"%s/%s",root,e->d_name);if(n<0||(size_t)n>=sizeof(path))continue;struct stat st;if(stat(path,&st)!=0)continue;if(S_ISDIR(st.st_mode)){int x=scan_dir(l,path);if(x&&rc==0)rc=x;}else if(S_ISREG(st.st_mode)){int x=sheen_media_library_scan_file(l,path);if(x&&x!=ENOTSUP&&rc==0)rc=x;}}closedir(d);return rc;}
int sheen_media_library_scan_tree(sheen_media_library *l,const char *root){if(!l||!root)return EINVAL;return scan_dir(l,root);}
int sheen_media_library_count(sheen_media_library *l,uint64_t *count){if(!l||!count)return EINVAL;sqlite3_stmt *s=NULL;if(sqlite3_prepare_v2(l->db,"SELECT COUNT(*) FROM media;",-1,&s,NULL)!=SQLITE_OK)return EIO;int rc=sqlite3_step(s)==SQLITE_ROW?0:EIO;if(!rc)*count=(uint64_t)sqlite3_column_int64(s,0);sqlite3_finalize(s);return rc;}
int sheen_media_library_get_source(sheen_media_library *l,const char *id,sheen_media_item *i){if(!l||!id||!i)return EINVAL;sqlite3_stmt *s=NULL;if(sqlite3_prepare_v2(l->db,"SELECT id,source,container,size_bytes,mtime_ns FROM media WHERE id=?;",-1,&s,NULL)!=SQLITE_OK)return EIO;sqlite3_bind_text(s,1,id,-1,SQLITE_TRANSIENT);int rc=sqlite3_step(s);if(rc==SQLITE_ROW){snprintf(i->id,sizeof(i->id),"%s",(const char*)sqlite3_column_text(s,0));snprintf(i->source,sizeof(i->source),"%s",(const char*)sqlite3_column_text(s,1));snprintf(i->container,sizeof(i->container),"%s",(const char*)sqlite3_column_text(s,2));i->size_bytes=(uint64_t)sqlite3_column_int64(s,3);i->mtime_ns=(int64_t)sqlite3_column_int64(s,4);rc=0;}else rc=ENOENT;sqlite3_finalize(s);return rc;}
