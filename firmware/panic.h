#ifndef PANIC_H_
#define PANIC_H_

void __attribute__((noreturn)) do_panic(int code, const String & message);

#endif
