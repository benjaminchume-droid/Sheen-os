#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "sheen/broadcast.h"
static int copy_demux(const sheen_demux_result *d,const sheen_broadcast_source *s,sheen_broadcast_multiplex *out){memset(out,0,sizeof(*out));out->source=*s;if(d->program_count>16)return ENOSPC;for(size_t i=0;i<d->program_count;i++){const sheen_demux_program *p=&d->programs[i];sheen_broadcast_program *o=&out->programs[out->program_count++];o->program_number=p->program_number;o->pmt_pid=p->pmt_pid;o->pcr_pid=p->pcr_pid;o->stream_count=p->stream_count;if(o->stream_count>SHEEN_DEMUX_MAX_STREAMS)o->stream_count=SHEEN_DEMUX_MAX_STREAMS;memcpy(o->streams,p->streams,o->stream_count*sizeof(o->streams[0]));o->service_id=p->program_number;}return 0;}
int sheen_broadcast_from_ts(const char *path,const sheen_broadcast_source *source,sheen_broadcast_multiplex *out){if(!path||!source||!out)return EINVAL;sheen_demux_result d;int rc=sheen_demux_mpegts_file(path,&d);if(rc)return rc;return copy_demux(&d,source,out);}
const char *sheen_broadcast_type_name(sheen_broadcast_type t){switch(t){case SHEEN_BROADCAST_TERRESTRIAL:return "terrestrial";case SHEEN_BROADCAST_CABLE:return "cable";case SHEEN_BROADCAST_SATELLITE:return "satellite";case SHEEN_BROADCAST_ATSC:return "atsc";case SHEEN_BROADCAST_ISDB:return "isdb";case SHEEN_BROADCAST_NETWORK:return "network";}return "unknown";}
