#ifndef SETTINGS_H__
#define SETTINGS_H__

bool settings_write(const String & key, const void * ptr, size_t size);
bool settings_write(const String & key, const String & value);
bool settings_read(const String & key, void *ptr, size_t size);
bool settings_read(const String & key, String & value);

#endif

