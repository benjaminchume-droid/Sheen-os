#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sheen/cast-devices.h"
struct sheen_cast_devices { sqlite3 *db; };
const char *sheen_cast_device_state_name(sheen_cast_device_state s){
    switch(s){
        case SHEEN_CAST_DEVICE_PRESENT:return "present";
        case SHEEN_CAST_DEVICE_STALE:return "stale";
        case SHEEN_CAST_DEVICE_REMOVED:return "removed";
    }
    return "unknown";
}
sheen_cast_devices *sheen_cast_devices_open(const char *path){
    if(!path)return NULL;
    sheen_cast_devices *d=calloc(1,sizeof(*d));
    if(!d)return NULL;
    if(sqlite3_open(path,&d->db)!=SQLITE_OK){sqlite3_close(d->db);free(d);return NULL;}
    return d;
}
int sheen_cast_devices_init(sheen_cast_devices *d){
    if(!d||!d->db)return EINVAL;
    const char *sql=
        "PRAGMA journal_mode=WAL;"
        "CREATE TABLE IF NOT EXISTS devices("
        "device_id TEXT PRIMARY KEY,"
        "name TEXT NOT NULL,"
        "address TEXT NOT NULL,"
        "port INTEGER NOT NULL,"
        "capabilities TEXT NOT NULL,"
        "last_seen_ms INTEGER NOT NULL,"
        "state INTEGER NOT NULL);"
        "CREATE INDEX IF NOT EXISTS devices_state_idx ON devices(state,last_seen_ms);";
    return sqlite3_exec(d->db,sql,NULL,NULL,NULL)==SQLITE_OK?0:EIO;
}
int sheen_cast_devices_upsert(sheen_cast_devices *d,const sheen_cast_managed_device *x){
    if(!d||!x||!x->device_id[0])return EINVAL;
    const char *sql=
        "INSERT INTO devices(device_id,name,address,port,capabilities,last_seen_ms,state)"
        "VALUES(?,?,?,?,?,?,?) "
        "ON CONFLICT(device_id) DO UPDATE SET "
        "name=excluded.name,address=excluded.address,port=excluded.port,"
        "capabilities=excluded.capabilities,last_seen_ms=excluded.last_seen_ms,state=excluded.state;";
    sqlite3_stmt *s=NULL;
    if(sqlite3_prepare_v2(d->db,sql,-1,&s,NULL)!=SQLITE_OK)return EIO;
    sqlite3_bind_text(s,1,x->device_id,-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(s,2,x->name,-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(s,3,x->address,-1,SQLITE_TRANSIENT);
    sqlite3_bind_int(s,4,x->port);
    sqlite3_bind_text(s,5,x->capabilities,-1,SQLITE_TRANSIENT);
    sqlite3_bind_int64(s,6,(sqlite3_int64)x->last_seen_ms);
    sqlite3_bind_int(s,7,x->state);
    int rc=sqlite3_step(s)==SQLITE_DONE?0:EIO;
    sqlite3_finalize(s);
    return rc;
}
int sheen_cast_devices_mark_missing(sheen_cast_devices *d,const char *id){
    if(!d||!id)return EINVAL;
    sqlite3_stmt *s=NULL;
    if(sqlite3_prepare_v2(d->db,"UPDATE devices SET state=? WHERE device_id=?;",-1,&s,NULL)!=SQLITE_OK)return EIO;
    sqlite3_bind_int(s,1,SHEEN_CAST_DEVICE_REMOVED);
    sqlite3_bind_text(s,2,id,-1,SQLITE_TRANSIENT);
    int rc=sqlite3_step(s)==SQLITE_DONE?0:EIO;
    int changed=sqlite3_changes(d->db);
    sqlite3_finalize(s);
    return rc?(rc):(changed?0:ENOENT);
}
int sheen_cast_devices_get(sheen_cast_devices *d,const char *id,sheen_cast_managed_device *x){
    if(!d||!id||!x)return EINVAL;
    sqlite3_stmt *s=NULL;
    if(sqlite3_prepare_v2(d->db,
        "SELECT device_id,name,address,port,capabilities,last_seen_ms,state FROM devices WHERE device_id=?;",
        -1,&s,NULL)!=SQLITE_OK)return EIO;
    sqlite3_bind_text(s,1,id,-1,SQLITE_TRANSIENT);
    int rc=sqlite3_step(s);
    if(rc!=SQLITE_ROW){sqlite3_finalize(s);return ENOENT;}
    memset(x,0,sizeof(*x));
    snprintf(x->device_id,sizeof(x->device_id),"%s",(const char*)sqlite3_column_text(s,0));
    snprintf(x->name,sizeof(x->name),"%s",(const char*)sqlite3_column_text(s,1));
    snprintf(x->address,sizeof(x->address),"%s",(const char*)sqlite3_column_text(s,2));
    x->port=(uint16_t)sqlite3_column_int(s,3);
    snprintf(x->capabilities,sizeof(x->capabilities),"%s",(const char*)sqlite3_column_text(s,4));
    x->last_seen_ms=(uint64_t)sqlite3_column_int64(s,5);
    x->state=(sheen_cast_device_state)sqlite3_column_int(s,6);
    sqlite3_finalize(s);
    return 0;
}
int sheen_cast_devices_count(sheen_cast_devices *d,uint64_t *count){
    if(!d||!count)return EINVAL;
    sqlite3_stmt *s=NULL;
    if(sqlite3_prepare_v2(d->db,"SELECT COUNT(*) FROM devices WHERE state!=?;",-1,&s,NULL)!=SQLITE_OK)return EIO;
    sqlite3_bind_int(s,1,SHEEN_CAST_DEVICE_REMOVED);
    int rc=sqlite3_step(s);
    if(rc==SQLITE_ROW){*count=(uint64_t)sqlite3_column_int64(s,0);rc=0;}else rc=EIO;
    sqlite3_finalize(s);
    return rc;
}
void sheen_cast_devices_close(sheen_cast_devices *d){
    if(!d)return;
    if(d->db)sqlite3_close(d->db);
    free(d);
}
