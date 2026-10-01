#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/permissions.h"

struct sheen_permissions { sqlite3 *db; };
const char *sheen_permission_name(sheen_permission p){
    switch(p){
        case SHEEN_PERM_DISPLAY:return "display";
        case SHEEN_PERM_AUDIO:return "audio";
        case SHEEN_PERM_INPUT:return "input";
        case SHEEN_PERM_NETWORK:return "network";
        case SHEEN_PERM_STORAGE:return "storage";
        case SHEEN_PERM_CAMERA:return "camera";
        case SHEEN_PERM_MICROPHONE:return "microphone";
        case SHEEN_PERM_TUNER:return "tuner";
        case SHEEN_PERM_CASTING:return "casting";
        case SHEEN_PERM_POWER:return "power";
    }
    return "unknown";
}
sheen_permissions *sheen_permissions_open(const char *path){
    if(!path)return NULL;
    sheen_permissions *p=calloc(1,sizeof(*p));
    if(!p)return NULL;
    if(sqlite3_open(path,&p->db)!=SQLITE_OK){sqlite3_close(p->db);free(p);return NULL;}
    return p;
}
int sheen_permissions_init(sheen_permissions *p){
    if(!p||!p->db)return EINVAL;
    const char *sql=
        "PRAGMA journal_mode=WAL;"
        "CREATE TABLE IF NOT EXISTS grants("
        "app_id TEXT NOT NULL,"
        "permission INTEGER NOT NULL,"
        "granted INTEGER NOT NULL,"
        "updated_at INTEGER NOT NULL,"
        "PRIMARY KEY(app_id,permission));";
    return sqlite3_exec(p->db,sql,NULL,NULL,NULL)==SQLITE_OK?0:EIO;
}
static int valid(sheen_permissions *p,const char *app,sheen_permission perm){
    if(!p||!app||!app[0]||perm<0||perm>SHEEN_PERM_POWER)return EINVAL;
    return 0;
}
static int set(sheen_permissions *p,const char *app,sheen_permission perm,int grant){
    int rc=valid(p,app,perm);if(rc)return rc;
    sqlite3_stmt *s=NULL;
    const char *sql=
        "INSERT INTO grants(app_id,permission,granted,updated_at)"
        "VALUES(?,?,?,strftime('%s','now'))"
        "ON CONFLICT(app_id,permission) DO UPDATE SET granted=excluded.granted,updated_at=excluded.updated_at;";
    if(sqlite3_prepare_v2(p->db,sql,-1,&s,NULL)!=SQLITE_OK)return EIO;
    sqlite3_bind_text(s,1,app,-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(s,2,perm);
    sqlite3_bind_int(s,3,grant?1:0);
    rc=sqlite3_step(s)==SQLITE_DONE?0:EIO;
    sqlite3_finalize(s);return rc;
}
int sheen_permissions_grant(sheen_permissions *p,const char *app,sheen_permission perm){return set(p,app,perm,1);}
int sheen_permissions_revoke(sheen_permissions *p,const char *app,sheen_permission perm){return set(p,app,perm,0);}
int sheen_permissions_check(sheen_permissions *p,const char *app,sheen_permission perm,int *granted){
    int rc=valid(p,app,perm);if(rc||!granted)return rc?rc:EINVAL;
    sqlite3_stmt *s=NULL;
    if(sqlite3_prepare_v2(p->db,"SELECT granted FROM grants WHERE app_id=? AND permission=?;",-1,&s,NULL)!=SQLITE_OK)return EIO;
    sqlite3_bind_text(s,1,app,-1,SQLITE_TRANSIENT);sqlite3_bind_int(s,2,perm);
    int row=sqlite3_step(s);
    *granted=row==SQLITE_ROW?sqlite3_column_int(s,0):0;
    sqlite3_finalize(s);return 0;
}
void sheen_permissions_close(sheen_permissions *p){if(!p)return;if(p->db)sqlite3_close(p->db);free(p);}
