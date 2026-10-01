#ifndef SHEEN_LOG_H
#define SHEEN_LOG_H
typedef enum { SHEEN_LOG_DEBUG, SHEEN_LOG_INFO, SHEEN_LOG_WARN, SHEEN_LOG_ERROR, SHEEN_LOG_FATAL } sheen_log_level;
const char *sheen_log_level_name(sheen_log_level level);
int sheen_log_write_path(const char *path, sheen_log_level level, const char *component, const char *message);
int sheen_log_write_default(sheen_log_level level, const char *component, const char *message);
#endif
