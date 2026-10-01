#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static const char *class_names[] = {"block","drm","net","sound","tty","input","backlight","graphics",NULL};

static void json_escape(const char *s) {
    putchar(34);
    for (; *s; ++s) {
        if (*s == 34 || *s == 92) { putchar(92); putchar(*s); }
        else if ((unsigned char)*s < 32) putchar(32);
        else putchar(*s);
    }
    putchar(34);
}

static int read_first_line(const char *path, char *buf, size_t n) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    if (!fgets(buf, n, f)) { fclose(f); return -1; }
    fclose(f);
    buf[strcspn(buf, "\n")] = 0;
    return 0;
}

static void emit_device(const char *class_name, const char *name) {
    char devpath[PATH_MAX], uevent[PATH_MAX], subsystem[PATH_MAX], vendor[PATH_MAX], model[PATH_MAX], driver[PATH_MAX];
    char link[PATH_MAX];
    snprintf(devpath,sizeof(devpath),"/sys/class/%s/%s",class_name,name);
    snprintf(uevent,sizeof(uevent),"%s/uevent",devpath);
    snprintf(subsystem,sizeof(subsystem),"%s/subsystem",devpath);
    snprintf(vendor,sizeof(vendor),"%s/device/vendor",devpath);
    snprintf(model,sizeof(model),"%s/device/model",devpath);
    snprintf(driver,sizeof(driver),"%s/device/driver",devpath);
    char real[PATH_MAX]={0}, value[256]={0};
    ssize_t n=readlink(devpath,real,sizeof(real)-1);
    if(n>0){real[n]=0;}
    char subsys_real[PATH_MAX]={0}, drv_real[PATH_MAX]={0};
    n=readlink(subsystem,subsys_real,sizeof(subsys_real)-1); if(n>0) subsys_real[n]=0;
    n=readlink(driver,drv_real,sizeof(drv_real)-1); if(n>0) drv_real[n]=0;
    const char *id = real[0] ? real : devpath;
    printf("{\\\"device_id\\\":"); json_escape(id);
    printf(",\\\"class\\\":"); json_escape(class_name);
    printf(",\\\"name\\\":"); json_escape(name);
    printf(",\\\"state\\\":\\\"present\\\"");
    printf(",\\\"properties\\\":{");
    int comma=0;
    if (read_first_line(vendor,value,sizeof(value))==0) { printf("%s\\\"vendor\\\":",comma?",":""); json_escape(value); comma=1; }
    if (read_first_line(model,value,sizeof(value))==0) { printf("%s\\\"model\\\":",comma?",":""); json_escape(value); comma=1; }
    if (read_first_line(uevent,value,sizeof(value))==0) { printf("%s\\\"uevent\\\":",comma?",":""); json_escape(value); comma=1; }
    if (real[0]) { printf("%s\\\"sysfs_path\\\":",comma?",":""); json_escape(real); comma=1; }
    if (subsys_real[0]) { printf("%s\\\"subsystem\\\":",comma?",":""); json_escape(subsys_real); comma=1; }
    if (drv_real[0]) { printf("%s\\\"driver\\\":",comma?",":""); json_escape(drv_real); comma=1; }
    snprintf(link,sizeof(link),"%s/device",devpath);
    n=readlink(link,real,sizeof(real)-1);
    if(n>0){real[n]=0; printf("%s\\\"device_path\\\":",comma?",":""); json_escape(real);}
    printf("}}\\n");
}

static void scan_class(const char *name) {
    char root[PATH_MAX]; snprintf(root,sizeof(root),"/sys/class/%s",name);
    DIR *d=opendir(root); if(!d) return;
    struct dirent *e;
    while((e=readdir(d))) {
        if(!strcmp(e->d_name,".") || !strcmp(e->d_name,"..")) continue;
        emit_device(name,e->d_name);
    }
    closedir(d);
}

int main(void) {
    if(access("/sys/class",R_OK|X_OK)!=0) { fprintf(stderr,"sheen-hw-discover: /sys/class unavailable: %s\\n",strerror(errno)); return 2; }
    for(size_t i=0;class_names[i];++i) {
        char root[PATH_MAX]; snprintf(root,sizeof(root),"/sys/class/%s",class_names[i]);
        DIR *d=opendir(root); if(!d) continue;
        struct dirent *e;
        while((e=readdir(d))) {
            if(!strcmp(e->d_name,".")||!strcmp(e->d_name,"..")) continue;
            if(!first){}
            emit_device(class_names[i],e->d_name);
            first=0;
        }
        closedir(d);
    }
    return 0;
}
