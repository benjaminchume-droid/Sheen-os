#include <errno.h>
#include <fcntl.h>
#include <linux/dvb/frontend.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "sheen/tuner.h"
struct sheen_tuner { int fd; char device[256]; };
const char *sheen_tuner_delivery_name(sheen_tuner_delivery d){switch(d){case SHEEN_TUNER_DVBT:return "dvbt";case SHEEN_TUNER_DVBT2:return "dvbt2";case SHEEN_TUNER_DVBC:return "dvbc";case SHEEN_TUNER_DVBS:return "dvbs";case SHEEN_TUNER_DVBS2:return "dvbs2";case SHEEN_TUNER_ATSC:return "atsc";case SHEEN_TUNER_ISDBT:return "isdbt";}return "unknown";}
static int delivery_system(sheen_tuner_delivery d,enum fe_delivery_system *out){switch(d){case SHEEN_TUNER_DVBT:*out=SYS_DVBT;return 0;case SHEEN_TUNER_DVBT2:*out=SYS_DVBT2;return 0;case SHEEN_TUNER_DVBC:*out=SYS_DVBC_ANNEX_A;return 0;case SHEEN_TUNER_DVBS:*out=SYS_DVBS;return 0;case SHEEN_TUNER_DVBS2:*out=SYS_DVBS2;return 0;case SHEEN_TUNER_ATSC:*out=SYS_ATSC;return 0;case SHEEN_TUNER_ISDBT:*out=SYS_ISDBT;return 0;}return EINVAL;}
sheen_tuner *sheen_tuner_open(const char *device){if(!device)return NULL;int fd=open(device,O_RDWR|O_NONBLOCK|O_CLOEXEC);if(fd<0)return NULL;sheen_tuner *t=calloc(1,sizeof(*t));if(!t){close(fd);return NULL;}t->fd=fd;snprintf(t->device,sizeof(t->device),"%s",device);return t;}
int sheen_tuner_info_get(sheen_tuner *t,sheen_tuner_info *i){if(!t||!i)return EINVAL;struct dvb_frontend_info x;if(ioctl(t->fd,FE_GET_INFO,&x)<0)return errno;memset(i,0,sizeof(*i));snprintf(i->name,sizeof(i->name),"%s",x.name);snprintf(i->type,sizeof(i->type),"%s","dvb-frontend");i->min_frequency=x.frequency_min;i->max_frequency=x.frequency_max;i->frequency_step=x.frequency_stepsize;i->capabilities=x.caps;return 0;}
int sheen_tuner_tune(sheen_tuner *t,const sheen_tuner_tune *r){if(!t||!r||!r->frequency_hz)return EINVAL;enum fe_delivery_system sys;if(delivery_system(r->delivery,&sys))return EINVAL;struct dtv_property p[4];memset(p,0,sizeof(p));p[0].cmd=DTV_CLEAR;p[1].cmd=DTV_DELIVERY_SYSTEM;p[1].u.data=sys;p[2].cmd=DTV_FREQUENCY;p[2].u.data=r->frequency_hz;unsigned n=3;if(r->bandwidth_hz){p[n].cmd=DTV_BANDWIDTH_HZ;p[n].u.data=r->bandwidth_hz;n++;}struct dtv_properties props={.num=n,.props=p};if(ioctl(t->fd,FE_SET_PROPERTY,&props)<0)return errno;struct dtv_property tune={.cmd=DTV_TUNE};struct dtv_properties seq={.num=1,.props=&tune};return ioctl(t->fd,FE_SET_PROPERTY,&seq)<0?errno:0;}
int sheen_tuner_lock_status(sheen_tuner *t,uint32_t *status){if(!t||!status)return EINVAL;fe_status_t s=0;if(ioctl(t->fd,FE_READ_STATUS,&s)<0)return errno;*status=(uint32_t)s;return 0;}
void sheen_tuner_close(sheen_tuner *t){if(!t)return;if(t->fd>=0)close(t->fd);free(t);}
