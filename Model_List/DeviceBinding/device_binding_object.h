
#ifndef DEVICE_BINDING_OBJECT_H
#define DEVICE_BINDING_OBJECT_H

#include <anjay/anjay.h>

const anjay_dm_object_def_t **device_binding_object_create(void);
void delete_device_binding_object(const anjay_dm_object_def_t **obj);

#endif /* TEST_OBJECT_H */
