#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/android-permissions.h"

struct sheen_android_permissions { sqlite3 *db; };

typedef struct { const char *name; uint64_t capability; } perm_map;

static const perm_map maps[] = {
    {"android.permission.INTERNET",SHEEN_ANDROID_CAP_NETWORK},
    {"android.permission.ACCESS_NETWORK_STATE",SHEEN_ANDROID_CAP_NETWORK},
    {"android.permission.CAMERA",SHEEN_ANDROID_CAP_CAMERA},
    {"android.permission.RECORD_AUDIO",SHEEN_ANDROID_CAP_MICROPHONE},
    {"android.permission.ACCESS_COARSE_LOCATION",SHEEN_ANDROID_CAP_LOCATION},
    {"android.permission.ACCESS_FINE_LOCATION",SHEEN_ANDROID_CAP_LOCATION},
    {"android.permission.READ_EXTERNAL_STORAGE",SHEEN_ANDROID_CAP_STORAGE_READ},
    {"android.permission.WRITE_EXTERNAL_STORAGE",SHEEN_ANDROID_CAP_STORAGE_WRITE},
    {"android.permission.BLUETOOTH",SHEEN_ANDROID_CAP_BLUETOOTH},
    {"android.permission.BLUETOOTH_CONNECT",SHEEN_ANDROID_CAP_BLUETOOTH},
    {"android.permission.BLUETOOTH_SCAN",SHEEN_ANDROID_CAP_BLUETOOTH},
    {"android.permission.POST_NOTIFICATIONS",SHEEN_ANDROID_CAP_NOTIFICATIONS},
    {"android.permission.WAKE_LOCK",SHEEN_ANDROID_CAP_WAKELOCK},
    {"android.permission.VIBRATE",SHEEN_ANDROID_CAP_VIBRATE}
};

uint64_t sheen_android_permission_capability(const char *permission) {
    if(!permission)return 0;
    for(size_t i=0;i<sizeof(maps)/sizeof(maps[0]);i++)
        if(!strcmp(maps[i].name,permission))return maps[i].capability;
    return 0;
}

sheen_android_permissions *sheen_android_permissions_open(const char *path) {
    if(!path)return NULL;
    sheen_android_permissions *p=calloc(1,sizeof(*p));
    if(!p)return NULL;
    if(sqlite3_open(path,&p->db)!=SQLITE_OK){sqlite3_close(p->db);free(p);return NULL;}
    return p;
}

int sheen_android_permissions_init(sheen_android_permissions *p) {
    if(!p||!p->db)return EINVAL;
    const char *sql =
        "PRAGMA journal_mode=WAL;"
        "CREATE TABLE IF NOT EXISTS grants("
        "package_id TEXT NOT NULL,"
        "permission TEXT NOT NULL,"
        "granted INTEGER NOT NULL,"
        "updated_at INTEGER NOT NULL,"
        "PRIMARY KEY(package_id,permission));";
    return sqlite3_exec(p->db,sql,NULL,NULL,NULL)==SQLITE_OK?0:EIO;
}

static int set_grant(sheen_android_permissions *p,const char *pkg,const char *perm,int granted) {
    if(!p||!pkg||!perm||!pkg[0]||!perm[0])return EINVAL;
    if(!sheen_android_permission_capability(perm))return ENOTSUP;
    const char *sql =
        "INSERT INTO grants(package_id,permission,granted,updated_at) VALUES(?,?,?,strftime('%s','now')) "
        "ON CONFLICT(package_id,permission) DO UPDATE SET granted=excluded.granted,updated_at=excluded.updated_at;";
    sqlite3_stmt *s=NULL;
    if(sqlite3_prepare_v2(p->db,sql,-1,&s,NULL)!=SQLITE_OK)return EIO;
    sqlite3_bind_text(s,1,pkg,-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(s,2,perm,-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(s,3,granted?1:0);
    int rc=sqlite3_step(s)==SQLITE_DONE?0:EIO;
    sqlite3_finalize(s);
    return rc;
}

int sheen_android_permission_grant(sheen_android_permissions *p,const char *pkg,const char *perm){return set_grant(p,pkg,perm,1);}
int sheen_android_permission_revoke(sheen_android_permissions *p,const char *pkg,const char *perm){return set_grant(p,pkg,perm,0);}

int sheen_android_permission_check(sheen_android_permissions *p,const char *pkg,const char *perm,int *granted) {
    if(!p||!pkg||!perm||!granted)return EINVAL;
    if(!sheen_android_permission_capability(perm))return ENOTSUP;
    sqlite3_stmt *s=NULL;
    if(sqlite3_prepare_v2(p->db,"SELECT granted FROM grants WHERE package_id=? AND permission=?;",-1,&s,NULL)!=SQLITE_OK)return EIO;
    sqlite3_bind_text(s,1,pkg,-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(s,2,perm,-1,SQLITE_TRANSIENT);
    int rc=sqlite3_step(s);
    *granted=(rc==SQLITE_ROW)?sqlite3_column_int(s,0):0;
    sqlite3_finalize(s);
    return 0;
}

void sheen_android_permissions_close(sheen_android_permissions *p) {
    if(!p)return;
    if(p->db)sqlite3_close(p->db);
    free(p);
}
