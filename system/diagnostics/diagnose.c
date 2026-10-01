#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
static void print_file_value(const char *key,const char *path){char b[8192];FILE *f=fopen(path,"r");if(!f)return;if(!fgets(b,sizeof(b),f)){fclose(f);return;}fclose(f);b[strcspn(b,"\\n")]=0;printf("\"%s\":\"",key);for(char *p=b;*p;p++){if(*p==34||*p==92)putchar(92);putchar(*p);}putchar(34);}
static int exists(const char *p){return access(p,F_OK)==0;}
int main(void){
printf("{");int comma=0;
if(exists("/proc/sys/kernel/osrelease")){print_file_value("kernel_release","/proc/sys/kernel/osrelease");comma=1;}
if(exists("/proc/cmdline")){if(comma)putchar(',');print_file_value("cmdline","/proc/cmdline");comma=1;}
if(exists("/etc/os-release")){if(comma)putchar(',');print_file_value("os_release","/etc/os-release");comma=1;}
if(comma)putchar(',');printf("\"sysfs_present\":%s",exists("/sys")?"true":"false");comma=1;
if(comma)putchar(',');printf("\"hardware_inventory_present\":%s",exists("/run/sheen/hardware/devices.jsonl")?"true":"false");
printf("}\\n");return 0;}
