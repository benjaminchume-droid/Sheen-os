#ifndef SHEEN_SUBTITLE_H
#define SHEEN_SUBTITLE_H
#include <stddef.h>
#include <stdint.h>
#define SHEEN_SUBTITLE_MAX_CUES 4096
#define SHEEN_SUBTITLE_TEXT_MAX 4096
typedef enum { SHEEN_SUBTITLE_UNKNOWN=0, SHEEN_SUBTITLE_SRT, SHEEN_SUBTITLE_WEBVTT } sheen_subtitle_format;
typedef struct { uint64_t start_ms; uint64_t end_ms; char text[SHEEN_SUBTITLE_TEXT_MAX]; } sheen_subtitle_cue;
typedef struct { sheen_subtitle_format format; size_t cue_count; sheen_subtitle_cue cues[SHEEN_SUBTITLE_MAX_CUES]; } sheen_subtitle_document;
int sheen_subtitle_parse_file(const char *path,sheen_subtitle_document *document);
int sheen_subtitle_find_active(const sheen_subtitle_document *document,uint64_t position_ms,size_t *first,size_t *count);
const char *sheen_subtitle_format_name(sheen_subtitle_format format);
#endif
