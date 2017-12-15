
#ifndef EMMI_OBJECT_H
#define EMMI_OBJECT_H

#include <anjay/anjay.h>

const anjay_dm_object_def_t **emm_integrator_object_create(void);
void delete_emm_integrator_object(const anjay_dm_object_def_t **obj);

#endif /* TEST_OBJECT_H */
