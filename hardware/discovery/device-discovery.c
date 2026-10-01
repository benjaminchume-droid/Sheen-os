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

static void emit_path_device(const char *group, const char *name, const char *path) {
    char real[PATH_MAX]={0}, vendor[256]={0}, device[256]={0}, product[256]={0}, manufacturer[256]={0}, class_code[256]={0};
    ssize_t n=readlink(path,real,sizeof(real)-1); if(n>0) real[n]=0;
    char p[PATH_MAX];
    snprintf(p,sizeof(p),"%s/vendor",path); read_first_line(p,vendor,sizeof(vendor));
    snprintf(p,sizeof(p),"%s/device",path); read_first_line(p,device,sizeof(device));
    snprintf(p,sizeof(p),"%s/idVendor",path); read_first_line(p,vendor,sizeof(vendor));
    snprintf(p,sizeof(p),"%s/idProduct",path); read_first_line(p,device,sizeof(device));
    snprintf(p,sizeof(p),"%s/product",path); read_first_line(p,product,sizeof(product));
    snprintf(p,sizeof(p),"%s/manufacturer",path); read_first_line(p,manufacturer,sizeof(manufacturer));
    snprintf(p,sizeof(p),"%s/class",path); read_first_line(p,class_code,sizeof(class_code));
    printf("{\"device_id\":"); json_escape(real[0] ? real : path);
    printf(",\"class\":"); json_escape(group);
    printf(",\"name\":"); json_escape(name);
    printf(",\"state\":\"present\",\"properties\":{");
    int comma=0;
    if(vendor[0]){printf("\"vendor\":");json_escape(vendor);comma=1;}
    if(device[0]){printf("%s\"device\":",comma?",":"");json_escape(device);comma=1;}
    if(product[0]){printf("%s\"product\":",comma?",":"");json_escape(product);comma=1;}
    if(manufacturer[0]){printf("%s\"manufacturer\":",comma?",":"");json_escape(manufacturer);comma=1;}
    if(class_code[0]){printf("%s\"class_code\":",comma?",":"");json_escape(class_code);comma=1;}
    printf("%s\"sysfs_path\":",comma?",":"");json_escape(path);
    printf("}}\n");
}

static void scan_tree(const char *root, const char *group, int depth) {
    DIR *d=opendir(root); if(!d) return;
    struct dirent *e;
    while((e=readdir(d))) {
        if(!strcmp(e->d_name,".")||!strcmp(e->d_name,"..")) continue;
        char path[PATH_MAX];
        snprintf(path,sizeof(path),"%s/%s",root,e->d_name);
        struct stat st;
        if(stat(path,&st)!=0) continue;
        if(S_ISDIR(st.st_mode)) {
            emit_path_device(group,e->d_name,path);
            if(depth>0) scan_tree(path,group,depth-1);
        }
    }
    closedir(d);
}

int main(void) {
    if(access("/sys/class",R_OK|X_OK)!=0) { fprintf(stderr,"sheen-hw-discover: /sys/class unavailable: %s\\n",strerror(errno)); return 2; }
    for(size_t i=0;class_names[i];++i) { char root[PATH_MAX]; snprintf(root,sizeof(root),"/sys/class/%s",class_names[i]); scan_tree(root,class_names[i],0); }
    scan_tree("/sys/bus/pci/devices","pci",0);
    scan_tree("/sys/bus/usb/devices","usb",0);
    return 0;
}
