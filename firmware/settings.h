#ifndef SETTINGS_H__
#define SETTINGS_H__

enum settings_overwrite_t { SETTINGS_NO_OVERWRITE, SETTINGS_OVERWRITE };
bool settings_write(const String & key, const void * ptr, size_t size, settings_overwrite_t overwrite = SETTINGS_OVERWRITE);
bool settings_write(const String & key, const String & value, settings_overwrite_t overwrite = SETTINGS_OVERWRITE);
bool settings_read(const String & key, void *ptr, size_t size);
bool settings_read(const String & key, String & value);

#endif

