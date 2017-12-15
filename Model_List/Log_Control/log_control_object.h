
#ifndef LOG_CONTROL_OBJECT_H
#define LOG_CONTROL_OBJECT_H

#include <anjay/anjay.h>

const anjay_dm_object_def_t **log_control_object_create(void);
void delete_log_control_object(const anjay_dm_object_def_t **obj);

#endif /* TEST_OBJECT_H */
