#ifndef SHEEN_EPG_H
#define SHEEN_EPG_H
#include <stddef.h>
#include <stdint.h>
#define SHEEN_EPG_MAX_EVENTS 8192
typedef struct { char channel_id[256]; int64_t start_ms; int64_t end_ms; char title[512]; char description[2048]; } sheen_epg_event;
typedef struct { size_t event_count; sheen_epg_event events[SHEEN_EPG_MAX_EVENTS]; } sheen_epg_document;
int sheen_epg_parse_xmltv(const char *path,sheen_epg_document *document);
int sheen_epg_find(const sheen_epg_document *document,const char *channel_id,int64_t position_ms,size_t *first,size_t *count);
#endif
