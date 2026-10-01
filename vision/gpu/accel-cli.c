#include <stdio.h>
#include "sheen/vision-accel.h"
int main(void){
    sheen_vision_accel_inventory i;
    if(sheen_vision_accel_probe(&i)) return 1;
    printf("{\"count\":%zu,\"devices\":[",i.count);
    for(size_t n=0;n<i.count;n++){
        if(n) putchar(',');
        printf("{\"device\":\"%s\",\"accessible\":%s,\"prime_cap\":%llu,\"addfb2_modifiers\":%llu,\"dumb_buffers\":%llu,\"syncobj\":%llu}",
               i.devices[n].device,i.devices[n].accessible?"true":"false",
               (unsigned long long)i.devices[n].prime_cap,
               (unsigned long long)i.devices[n].addfb2_modifiers,
               (unsigned long long)i.devices[n].dumb_buffers,
               (unsigned long long)i.devices[n].syncobj);
    }
    puts("]}");
    return 0;
}
