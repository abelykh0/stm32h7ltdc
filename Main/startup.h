#ifndef __STARTUP_H__
#define __STARTUP_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DEMO_COLORS = 0,
    XONIX = 1,
} demo_mode;

void initialize();
void setup();
void loop();

#ifdef __cplusplus
}
#endif

#endif /* __STARTUP_H__ */
