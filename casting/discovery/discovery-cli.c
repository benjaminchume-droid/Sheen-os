#include <stdio.h>
#include <stdlib.h>
#include "sheen/discovery.h"
int main(int argc,char **argv){uint32_t ms=1500;if(argc==2)ms=(uint32_t)strtoul(argv[1],NULL,10);sheen_cast_discovery_result r;int rc=sheen_cast_discover(ms,&r);if(rc){fprintf(stderr,"discover failed: %d\n",rc);return 1;}printf("{\"count\":%zu,\"devices\":[",r.count);for(size_t i=0;i<r.count;i++){if(i)putchar(',');printf("{\"device_id\":\"%s\",\"name\":\"%s\",\"address\":\"%s\",\"port\":%u,\"capabilities\":\"%s\"}",r.devices[i].device_id,r.devices[i].name,r.devices[i].address,r.devices[i].port,r.devices[i].capabilities);}puts("]}");return 0;}
