#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <zlib.h>
#include "sheen/android-package.h"

#define EOCD_SIG 0x06054b50U
#define CEN_SIG  0x02014b50U
#define LOC_SIG  0x04034b50U

struct sheen_android_package_manager {
    sqlite3 *db;
    char root[4096];
};

static uint16_t rd16(const uint8_t *p){return (uint16_t)p[0]|((uint16_t)p[1]<<8);}
static uint32_t rd32(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}

static int read_at(int fd,void *buf,size_t n,off_t off){
    uint8_t *p=buf;size_t got=0;
    while(got<n){
        ssize_t r=pread(fd,p+got,n-got,off+(off_t)got);
        if(r<0){if(errno==EINTR)continue;return errno;}
        if(!r)return EIO;
        got+=(size_t)r;
    }
    return 0;
}

static int find_eocd(int fd,uint64_t file_size,off_t *off_out,uint8_t out[22]){
    uint64_t window=file_size<65557?file_size:65557;
    uint8_t *buf=malloc((size_t)window);
    if(!buf)return ENOMEM;
    off_t base=(off_t)(file_size-window);
    int rc=read_at(fd,buf,(size_t)window,base);
    if(rc){free(buf);return rc;}
    ssize_t found=-1;
    for(ssize_t i=(ssize_t)window-22;i>=0;i--){
        if(rd32(buf+i)==EOCD_SIG){found=i;break;}
    }
    if(found<0){free(buf);return EPROTO;}
    memcpy(out,buf+found,22);
    *off_out=base+found;
    free(buf);
    return 0;
}

static int locate_entry(int fd,off_t central_off,uint32_t central_size,
                        const char *wanted,uint32_t *method,uint32_t *comp_size,
                        uint32_t *uncomp_size,uint32_t *local_off){
    uint8_t h[46];
    uint64_t pos=(uint64_t)central_off,end=pos+central_size;
    size_t wanted_len=strlen(wanted);
    while(pos+46<=end){
        if(read_at(fd,h,sizeof(h),(off_t)pos))return EIO;
        if(rd32(h)!=CEN_SIG)return EPROTO;
        uint16_t name_len=rd16(h+28),extra_len=rd16(h+30),comment_len=rd16(h+32);
        uint64_t next=pos+46+(uint64_t)name_len+extra_len+comment_len;
        if(next>end)return EPROTO;
        char *name=malloc((size_t)name_len+1);if(!name)return ENOMEM;
        int rc=read_at(fd,name,name_len,(off_t)(pos+46));
        if(rc){free(name);return rc;}
        name[name_len]=0;
        if(name_len==wanted_len&&!memcmp(name,wanted,wanted_len)){
            *method=rd16(h+10);*comp_size=rd32(h+20);*uncomp_size=rd32(h+24);*local_off=rd32(h+42);
            free(name);return 0;
        }
        free(name);pos=next;
    }
    return ENOENT;
}

static int extract_entry(int fd,uint32_t local_off,uint32_t method,uint32_t comp_size,
                         uint32_t uncomp_size,uint8_t **out,size_t *out_size){
    uint8_t lh[30];
    int rc=read_at(fd,lh,sizeof(lh),local_off);
    if(rc)return rc;
    if(rd32(lh)!=LOC_SIG)return EPROTO;
    uint16_t name_len=rd16(lh+26),extra_len=rd16(lh+28);
    uint64_t data_off=(uint64_t)local_off+30+(uint64_t)name_len+extra_len;
    uint8_t *compressed=malloc(comp_size?comp_size:1);
    if(!compressed)return ENOMEM;
    rc=read_at(fd,compressed,comp_size,(off_t)data_off);
    if(rc){free(compressed);return rc;}
    uint8_t *plain=malloc(uncomp_size?uncomp_size:1);
    if(!plain){free(compressed);return ENOMEM;}
    if(method==0){
        if(comp_size!=uncomp_size){free(compressed);free(plain);return EPROTO;}
        memcpy(plain,compressed,uncomp_size);
    } else if(method==8){
        z_stream z={0};
        z.next_in=compressed;z.avail_in=comp_size;
        z.next_out=plain;z.avail_out=uncomp_size;
        if(inflateInit2(&z,-MAX_WBITS)!=Z_OK){free(compressed);free(plain);return EPROTO;}
        int zr=inflate(&z,Z_FINISH);inflateEnd(&z);
        if(zr!=Z_STREAM_END||z.total_out!=uncomp_size){free(compressed);free(plain);return EPROTO;}
    } else {
        free(compressed);free(plain);return ENOTSUP;
    }
    free(compressed);*out=plain;*out_size=uncomp_size;return 0;
}

typedef struct {
    const uint8_t *data;
    size_t size;
    const uint8_t *strings;
    uint32_t string_count;
    uint32_t strings_start;
    uint32_t flags;
} axml_ctx;

static size_t axml_len(const uint8_t *p,uint32_t *value){
    if(p[0]&0x80U){*value=((uint32_t)(p[0]&0x7fU)<<8)|p[1];return 2;}
    *value=p[0];return 1;
}
static int pool_string(const axml_ctx *ctx,uint32_t idx,char *out,size_t cap){
    if(!ctx||!out||cap<1||idx>=ctx->string_count)return EINVAL;
    const uint8_t *offsets=ctx->strings;
    uint32_t rel=rd32(offsets+4U*idx);
    if((uint64_t)ctx->strings_start+rel>=ctx->size)return EPROTO;
    const uint8_t *p=ctx->data+ctx->strings_start+rel;
    uint32_t utf16_len=0;
    size_t off=axml_len(p,&utf16_len);
    if(ctx->flags&0x100U){
        uint32_t utf8_len=0;
        off+=axml_len(p+off,&utf8_len);
        if((uint64_t)(ctx->strings_start+rel+off+utf8_len)>ctx->size)return EPROTO;
        if(utf8_len>=cap)utf8_len=(uint32_t)cap-1;
        memcpy(out,p+off,utf8_len);
        out[utf8_len]=0;
        return 0;
    }
    size_t chars=utf16_len;
    if(chars>=cap)chars=cap-1;
    for(size_t i=0;i<chars;i++){
        uint16_t u=rd16(p+off+2*i);
        out[i]=(u<128U)?(char)u:'?';
    }
    out[chars]=0;
    return 0;
}

static int axml_find_package(const uint8_t *buf,size_t len,char *package,size_t package_cap,
                             int *has_tv){
    if(len<8)return EPROTO;
    const uint8_t *p=buf;size_t pos=0;
    axml_ctx ctx={0};
    int manifest_seen=0;
    while(pos+8<=len){
        uint16_t type=rd16(p+pos);uint16_t header=rd16(p+pos+2);uint32_t size=rd32(p+pos+4);
        if(size<8||pos+size>len)return EPROTO;
        if(type==0x0001){
            if(size<header||size<28)return EPROTO;
            ctx.data=buf;ctx.string_count=rd32(p+pos+8);ctx.flags=rd32(p+pos+16);
            ctx.strings_start=(uint32_t)(pos+rd32(p+pos+20));
            if(ctx.strings_start>=len)return EPROTO;
            ctx.strings=p+pos+28;
        } else if(type==0x0102 && header>=36){
            const uint8_t *node=p+pos;
            uint32_t name_idx=rd32(node+20);
            char element[256]={0};
            if(!pool_string(&ctx,name_idx,element,sizeof(element))) {
                uint16_t attr_start=rd16(node+24),attr_size=rd16(node+26),attr_count=rd16(node+28);
                const uint8_t *attrs=node+attr_start;
                if((size_t)attr_start+(size_t)attr_size*attr_count>size){pos+=size;continue;}
                for(uint16_t i=0;i<attr_count;i++){
                    const uint8_t *a=attrs+(size_t)i*attr_size;
                    char aname[256]={0},aval[1024]={0};
                    uint32_t ai=rd32(a+4),raw=rd32(a+8);
                    pool_string(&ctx,ai,aname,sizeof(aname));
                    if(raw!=0xffffffffU)pool_string(&ctx,raw,aval,sizeof(aval));
                    uint8_t dtype=a[15];uint32_t data=rd32(a+16);
                    if(dtype==0x03)pool_string(&ctx,data,aval,sizeof(aval));
                    if(!strcmp(element,"manifest")&&!strcmp(aname,"package")&&aval[0]){
                        snprintf(package,package_cap,"%s",aval);manifest_seen=1;
                    }
                    if(!strcmp(element,"uses-feature")&&!strcmp(aname,"name")&&
                       !strcmp(aval,"android.software.leanback"))*has_tv=1;
                }
            }
        }
        pos+=size;
    }
    return manifest_seen?0:ENOENT;
}

static int apk_inspect_internal(const char *apk,sheen_android_package_info *info){
    memset(info,0,sizeof(*info));
    snprintf(info->source_path,sizeof(info->source_path),"%s",apk);
    struct stat st;if(stat(apk,&st))return errno;
    info->size_bytes=(uint64_t)st.st_size;
    int fd=open(apk,O_RDONLY|O_CLOEXEC);if(fd<0)return errno;
    uint8_t eocd[22];off_t eoff=0;int rc=find_eocd(fd,info->size_bytes,&eoff,eocd);(void)eoff;
    if(rc){close(fd);return rc;}
    uint16_t disk=rd16(eocd+4),cd_disk=rd16(eocd+6);
    uint16_t count_disk=rd16(eocd+8),count=rd16(eocd+10);
    uint32_t cd_size=rd32(eocd+12),cd_off=rd32(eocd+16);
    if(disk||cd_disk||count_disk!=count){close(fd);return EPROTO;}
    uint32_t method=0,comp=0,uncomp=0,local=0;
    rc=locate_entry(fd,cd_off,cd_size,"AndroidManifest.xml",&method,&comp,&uncomp,&local);
    if(rc){close(fd);return rc;}
    if(uncomp>16*1024*1024){close(fd);return EFBIG;}
    uint8_t *manifest=NULL;size_t manifest_size=0;
    rc=extract_entry(fd,local,method,comp,uncomp,&manifest,&manifest_size);
    if(rc){close(fd);return rc;}
    info->has_manifest=1;
    int tv=0;rc=axml_find_package(manifest,manifest_size,info->package_id,sizeof(info->package_id),&tv);
    info->has_tv_feature=(uint8_t)tv;
    free(manifest);
    if(rc){close(fd);return rc;}
    rc=locate_entry(fd,cd_off,cd_size,"classes.dex",&method,&comp,&uncomp,&local);
    if(!rc)info->has_dex=1;
    close(fd);
    return info->has_dex?0:ENOENT;
}

int sheen_android_package_inspect(const char *apk_path,sheen_android_package_info *info){
    if(!apk_path||!info)return EINVAL;
    return apk_inspect_internal(apk_path,info);
}
static int mkdir_p(const char *path){
    char b[4096];size_t n=strlen(path);if(n>=sizeof(b))return ENAMETOOLONG;memcpy(b,path,n+1);
    for(size_t i=1;i<n;i++)if(b[i]=='/'){b[i]=0;if(mkdir(b,0755)&&errno!=EEXIST)return errno;b[i]='/';}
    if(mkdir(b,0755)&&errno!=EEXIST)return errno;return 0;
}
static int copy_atomic(const char *src,const char *dst){
    char tmp[4096];int n=snprintf(tmp,sizeof(tmp),"%s.tmp.%ld",dst,(long)getpid());if(n<0||(size_t)n>=sizeof(tmp))return ENAMETOOLONG;
    int in=open(src,O_RDONLY|O_CLOEXEC);if(in<0)return errno;
    int out=open(tmp,O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC,0644);if(out<0){int e=errno;close(in);return e;}
    uint8_t buf[1024*1024];int rc=0;for(;;){ssize_t r=read(in,buf,sizeof(buf));if(r<0){if(errno==EINTR)continue;rc=errno;break;}if(!r)break;size_t w=0;while(w<(size_t)r){ssize_t x=write(out,buf+w,(size_t)r-w);if(x<0){if(errno==EINTR)continue;rc=errno;break;}w+=(size_t)x;}if(rc)break;}
    if(!rc&&fsync(out)<0)rc=errno;close(in);if(close(out)<0&&!rc)rc=errno;
    if(!rc&&rename(tmp,dst)<0)rc=errno;
    if(rc)unlink(tmp);return rc;
}
sheen_android_package_manager *sheen_android_package_open(const char *db,const char *root){
    if(!db||!root)return NULL;
    sheen_android_package_manager *m=calloc(1,sizeof(*m));if(!m)return NULL;
    snprintf(m->root,sizeof(m->root),"%s",root);
    if(mkdir_p(root)!=0){free(m);return NULL;}
    if(sqlite3_open(db,&m->db)!=SQLITE_OK){sqlite3_close(m->db);free(m);return NULL;}
    return m;
}
int sheen_android_package_init(sheen_android_package_manager *m){
    if(!m||!m->db)return EINVAL;
    const char *sql="PRAGMA journal_mode=WAL;CREATE TABLE IF NOT EXISTS packages(package_id TEXT PRIMARY KEY,version_code INTEGER NOT NULL,source_path TEXT NOT NULL,installed_path TEXT NOT NULL UNIQUE,size_bytes INTEGER NOT NULL,has_manifest INTEGER NOT NULL,has_dex INTEGER NOT NULL,has_tv_feature INTEGER NOT NULL);";
    return sqlite3_exec(m->db,sql,NULL,NULL,NULL)==SQLITE_OK?0:EIO;
}
static int valid_package_id(const char *id){
    if(!id||!*id||strlen(id)>=256||id[0]=='/'||id[0]=='\\')return 0;
    for(const char *p=id;*p;p++){
        if(*p=='/'||*p=='\\'||*p==' '||*p=='\t'||*p=='\r'||*p=='\n')return 0;
    }
    return 1;
}
int sheen_android_package_install(sheen_android_package_manager *m,const char *apk,sheen_android_package_info *info){
    if(!m||!apk||!info)return EINVAL;int rc=apk_inspect_internal(apk,info);if(rc)return rc;
    if(!valid_package_id(info->package_id))return EINVAL;
    char dir[4096],dst[4096];snprintf(dir,sizeof(dir),"%s/%s",m->root,info->package_id);if(mkdir_p(dir))return errno;snprintf(dst,sizeof(dst),"%s/base.apk",dir);
    rc=copy_atomic(apk,dst);if(rc)return rc;
    snprintf(info->installed_path,sizeof(info->installed_path),"%s",dst);
    sqlite3_stmt *s=NULL;const char *sql="INSERT INTO packages(package_id,version_code,source_path,installed_path,size_bytes,has_manifest,has_dex,has_tv_feature)VALUES(?,?,?,?,?,?,?,?) ON CONFLICT(package_id)DO UPDATE SET source_path=excluded.source_path,installed_path=excluded.installed_path,size_bytes=excluded.size_bytes,has_manifest=excluded.has_manifest,has_dex=excluded.has_dex,has_tv_feature=excluded.has_tv_feature;";
    if(sqlite3_prepare_v2(m->db,sql,-1,&s,NULL)!=SQLITE_OK)return EIO;
    sqlite3_bind_text(s,1,info->package_id,-1,SQLITE_TRANSIENT);sqlite3_bind_int64(s,2,(sqlite3_int64)info->version_code);sqlite3_bind_text(s,3,info->source_path,-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,4,info->installed_path,-1,SQLITE_TRANSIENT);sqlite3_bind_int64(s,5,(sqlite3_int64)info->size_bytes);sqlite3_bind_int(s,6,info->has_manifest);sqlite3_bind_int(s,7,info->has_dex);sqlite3_bind_int(s,8,info->has_tv_feature);
    rc=sqlite3_step(s)==SQLITE_DONE?0:EIO;sqlite3_finalize(s);return rc;
}
int sheen_android_package_remove(sheen_android_package_manager *m,const char *id){
    if(!m||!id)return EINVAL;char path[4096];snprintf(path,sizeof(path),"%s/%s/base.apk",m->root,id);unlink(path);
    sqlite3_stmt *s=NULL;if(sqlite3_prepare_v2(m->db,"DELETE FROM packages WHERE package_id=?;",-1,&s,NULL)!=SQLITE_OK)return EIO;sqlite3_bind_text(s,1,id,-1,SQLITE_TRANSIENT);int rc=sqlite3_step(s)==SQLITE_DONE?0:EIO;int changed=sqlite3_changes(m->db);sqlite3_finalize(s);return rc?(rc):(changed?0:ENOENT);
}
int sheen_android_package_get(sheen_android_package_manager *m,const char *id,sheen_android_package_info *i){
    if(!m||!id||!i)return EINVAL;sqlite3_stmt *s=NULL;if(sqlite3_prepare_v2(m->db,"SELECT package_id,version_code,source_path,installed_path,size_bytes,has_manifest,has_dex,has_tv_feature FROM packages WHERE package_id=?;",-1,&s,NULL)!=SQLITE_OK)return EIO;sqlite3_bind_text(s,1,id,-1,SQLITE_TRANSIENT);int rc=sqlite3_step(s);if(rc!=SQLITE_ROW){sqlite3_finalize(s);return ENOENT;}memset(i,0,sizeof(*i));snprintf(i->package_id,sizeof(i->package_id),"%s",(const char*)sqlite3_column_text(s,0));i->version_code=(uint64_t)sqlite3_column_int64(s,1);snprintf(i->source_path,sizeof(i->source_path),"%s",(const char*)sqlite3_column_text(s,2));snprintf(i->installed_path,sizeof(i->installed_path),"%s",(const char*)sqlite3_column_text(s,3));i->size_bytes=(uint64_t)sqlite3_column_int64(s,4);i->has_manifest=(uint8_t)sqlite3_column_int(s,5);i->has_dex=(uint8_t)sqlite3_column_int(s,6);i->has_tv_feature=(uint8_t)sqlite3_column_int(s,7);sqlite3_finalize(s);return 0;
}
int sheen_android_package_count(sheen_android_package_manager *m,uint64_t *count){
    if(!m||!count)return EINVAL;sqlite3_stmt *s=NULL;if(sqlite3_prepare_v2(m->db,"SELECT COUNT(*) FROM packages;",-1,&s,NULL)!=SQLITE_OK)return EIO;int rc=sqlite3_step(s);if(rc==SQLITE_ROW){*count=(uint64_t)sqlite3_column_int64(s,0);rc=0;}else rc=EIO;sqlite3_finalize(s);return rc;
}
void sheen_android_package_close(sheen_android_package_manager *m){if(!m)return;if(m->db)sqlite3_close(m->db);free(m);}
