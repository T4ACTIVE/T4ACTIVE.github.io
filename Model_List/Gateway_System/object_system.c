//
//  Copyright (c) 2015, Tampere University of Technology.  
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//  1. Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
//  CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
//  BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//  FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY 
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/*
 * Authors: Bill Silverajan, Hanna Lindholm-Ventola
 * Implements an IPSO Gateway System Application object 
 *
 *                 Multiple
 * Object |  ID  | Instances | Mandatory |
 * System | 1022 |   No    |    No     |
 *
 *  Resources:
 *                  Supported    Multiple
 *  Name     | ID | Operations | Instances | Mandatory |  Type   | Range | Units | Description |
 *  hostname |  0 |    R/W     |           |           | String  |       |       |             |
 *  timezone |  1 |    R/W     |           |           | String  |       |       |             |
 *  DNS      |  2 |    R/W     |           |           | String  |       |       |             |
 *  NTP      |  3 |    R/W     |           |           | String  |       |       |             |
 *    
 */

#include "liblwm2m.h"
#include "lwm2mgateway.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "openwrt_object_utils.h"

#define PRV_TLV_BUFFER_SIZE 512 // Increased from 64 to 512

 /*
  * Multiple instance objects can use userdata to store data that will be shared between the different instances.
  * The lwm2m_object_t object structure - which represent every object of the liblwm2m as seen in the single instance
  * object - contain a chained list called instanceList with the object specific structure prv_instance_t:
  */
 typedef struct _prv_instance_
 {
     /*
      * The first two are mandatories and represent the pointer to the next instance and the ID of this one. The rest
      * is the instance scope user data (uint8_t test in this case)
      */
     struct _prv_instance_ * next;   // matches lwm2m_list_t::next
     uint16_t shortID;               // matches lwm2m_list_t::id
     uint8_t  test;
 } prv_instance_t;


static uint8_t prv_system_get_value(lwm2m_data_t * dataP)
{
  char *line = NULL;
  size_t len = 0;
  uint8_t returncode;
  char buffer[100];
  FILE *fp;
  ssize_t read;
  ipAddrStr *ipAddrList = NULL;
  uint8_t ipaddresses = 0;
  
  dataP->flags = 0;

/******************************************************************
 * Implement below handler for each resource. The handler reads   *
 * data from UCI and then fills lwm2m_data_t structure with        *
 * resource specific data (type, length, pointer to actual        *
 * resource value).                                               *
 *****************************************************************/

  switch (dataP->id) {
  case 0:  // Resource ID 0: hostname
    returncode = calluciandgetline("hostname", "system", "@system[0]", &line, &len);
    //    if (returncode != COAP_205_CONTENT) return returncode;
    if (returncode != COAP_205_CONTENT) {
      len = 5;
      line = malloc(5);
      memcpy(line, "NULL", 4);
    }

    len--;  
    dataP->type = LWM2M_TYPE_RESOURCE;
    dataP->length = len;
    dataP->value = malloc(len);
    memcpy(dataP->value, line, len);
    free(line); 
    return COAP_205_CONTENT;

  case 1: // Resource ID 1: timezone
    returncode = calluciandgetline("timezone", "system", "@system[0]", &line, &len);
    //    if (returncode != COAP_205_CONTENT) return returncode;
    if (returncode != COAP_205_CONTENT) {
      len = 5;
      line = malloc(5);
      memcpy(line, "NULL", 4);
    }
    len--;   
    dataP->flags = 0;
    dataP->type = LWM2M_TYPE_RESOURCE;
    dataP->length = len;
    dataP->value = malloc(len);
    memcpy(dataP->value, line, len);
    free(line); 
    return COAP_205_CONTENT;

  case 2: // Resource ID 2: DNS SERVER LIST
    returncode = calluciandgetlinelist("dns", "system", "@system[0]", &line, &len);
    printf ("Resource 2 \n");
    //    if (returncode != COAP_205_CONTENT) return returncode;
    if (returncode != COAP_205_CONTENT) {
      len = 5;
      line = malloc(5);
      memcpy(line, "NULL", 4);
    }
    len--;   
    dataP->flags = 0;
    dataP->type = LWM2M_TYPE_RESOURCE;
    dataP->length = len;
    dataP->value = malloc(len);
    memcpy(dataP->value, line, len);
    free(line); 
    return COAP_205_CONTENT;

  case 3: // Resource ID 3: NTP SERVER LIST
    returncode = calluciandgetlinelist("server", "system", "ntp", &line, &len);
    //    if (returncode != COAP_205_CONTENT) return returncode;
    if (returncode != COAP_205_CONTENT) {
      len = 5;
      line = malloc(5);
      memcpy(line, "NULL", 4);
    }
    len--;   
    dataP->flags = 0;
    dataP->type = LWM2M_TYPE_RESOURCE;
    dataP->length = len;
    dataP->value = malloc(len);
    memcpy(dataP->value, line, len);
    free(line); 
    return COAP_205_CONTENT;

  }
}   

/******************************************************************
 * prv_system_read(): this is called by lwm2mgateway.c to obtain values   *
 * for a specific resource (numDataP indicating resource number)  *
 * or for all resources of this object (numDataP indicating 0).   *
 * This function prepares response data structure and then calls  *
 * prv_system_get_value multiple times to get data for each resource.    *
 *****************************************************************/

static uint8_t prv_system_read(uint16_t instanceId,
                        int * numDataP,
                        lwm2m_data_t ** dataArrayP,
                        lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;
    int i;
    uint8_t returncode = COAP_205_CONTENT;

    fprintf(stdout,"prv_system_read, instanceID: %i, numDataP: %i\n",instanceId, *numDataP);

    targetP = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    if (*numDataP == 0)
      {
	fprintf(stdout,"Server is asking for the whole object\n");

        *dataArrayP = lwm2m_data_new(4); // *** MODIFY: Number of TLV resources ***

        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = 4; // *** MODIFY: number of resources ***

       	for (i = 0; i < 4; i++) {  // *** MODIFY: i < number of resources ***
	  (*dataArrayP)[i].id = i;
	  printf("Setting ID field for resource number %d\n",i);
	}
    }
    else {
      fprintf(stdout,"Server is asking just for resource: %i\n",(*dataArrayP)->id);
    }

    i = 0;
    do {

      printf("Setting content for resource %d\n",((*dataArrayP)+i)->id);
      returncode = prv_system_get_value((*dataArrayP)+i);
      if (returncode != COAP_205_CONTENT && *numDataP == 1) {

	*numDataP = 0; 
	fprintf(stdout,"Error situation, returning value: %i\n",returncode);
	return returncode; 
      }
      i++;
    } while (i < *numDataP); 
 
    fprintf(stdout,"All content collected.\nReturning value: COAP_205_CONTENT\n");

    return COAP_205_CONTENT;
}

/******************************************************************
 * prv_system_write: Handlers for writing each resource. Handlers        *
 * construct UCI commands for storing new settings and then       *
 * execute those.                                                 *
 *****************************************************************/

static uint8_t prv_system_write(uint16_t instanceId,
                         int numData,
                         lwm2m_data_t * dataArray,
                         lwm2m_object_t * objectP)
{
  prv_instance_t * targetP;
  int64_t value;
  char *buffer;
  uint8_t returncode = COAP_204_CHANGED;
  char *line = NULL;
  size_t len = 0;
  
  buffer = malloc(100);
  memset(buffer, 0, 100);
  
  targetP = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
  if (NULL == targetP) return COAP_404_NOT_FOUND;
  
  if (numData != 1) return COAP_404_NOT_FOUND;
  
  switch (dataArray->id) {
  case 0:
    strcpy(buffer,"uci set system.@system[0].hostname=");
    memcpy(buffer + strlen(buffer), dataArray->value, dataArray->length);
    systemCmd(buffer);
    systemCmd("uci commit");
    break;
  case 1:
    strcpy(buffer,"uci set system.@system[0].timezone=");
    memcpy(buffer + strlen(buffer), dataArray->value, dataArray->length);
    systemCmd(buffer);
    systemCmd("uci commit");
    break;
  case 2:
    returncode = calluciwriteaddresses("dns", "system", "@system[0]", 
				       dataArray->value, dataArray->length);
    break;
  case 3:
    returncode = calluciwriteaddresses("server", "system", "ntp", 
				       dataArray->value, dataArray->length);
    break;
  default:
    printf("Write to unknown resource\n");
    returncode = COAP_404_NOT_FOUND;
  }

  free(buffer);
  return returncode;
}

/******************************************************************
 * prv_system_delete(): This function would be used to delete object     *
 * instances. Unmodified & not tested.                            *
 *****************************************************************/

static uint8_t prv_system_delete(uint16_t id,
                          lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;

    objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&targetP);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    free(targetP);

    return COAP_202_DELETED;
}

/******************************************************************
 * prv_system_create(): This function would be used to create new object *
 * instances. Unmodified & not tested.                            *
 *****************************************************************/

static uint8_t prv_system_create(uint16_t instanceId,
                          int numData,
                          lwm2m_data_t * dataArray,
                          lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;
    uint8_t result;


    targetP = (prv_instance_t *)malloc(sizeof(prv_instance_t));
    if (NULL == targetP) return COAP_500_INTERNAL_SERVER_ERROR;
    memset(targetP, 0, sizeof(prv_instance_t));

    targetP->shortID = instanceId;
    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, targetP);

    result = prv_system_write(instanceId, numData, dataArray, objectP);

    if (result != COAP_204_CHANGED)
    {
        (void)prv_system_delete(instanceId, objectP);
    }
    else
    {
        result = COAP_201_CREATED;
    }

    return result;
}

/******************************************************************
 * prv_system_exec(): This function is used for execute commands.        *
 * This was just quickly tested.                                  *
 *****************************************************************/

static uint8_t prv_system_exec(uint16_t instanceId,
                        uint16_t resourceId,
                        char * buffer,
                        int length,
                        lwm2m_object_t * objectP)
{

    if (NULL == lwm2m_list_find(objectP->instanceList, instanceId)) return COAP_404_NOT_FOUND;

    switch (resourceId)
    {
    case 1:
        return COAP_405_METHOD_NOT_ALLOWED;
    case 2:
	if (length > 0) {
	  char system_command_str[100];
	  memset(system_command_str, 0, 100);

	  strcpy(system_command_str,"uci -c etc/config set wireless.wlan0.txpower=");

	  memcpy(system_command_str + strlen(system_command_str), buffer, length);
	  systemCmd(system_command_str);

	  systemCmd("uci -c etc/config commit wireless");
	}
        return COAP_204_CHANGED;
    default:
        return COAP_404_NOT_FOUND;
    }
}

static void prv_system_close(lwm2m_object_t * object)
{
    LWM2M_LIST_FREE(object->instanceList);
    if (object->userData != NULL)
    {
        lwm2m_free(object->userData);
        object->userData = NULL;
    }
}

void display_system_object(lwm2m_object_t * object)
{
#ifdef WITH_LOGS
    fprintf(stdout, "  /%u: Test object, instances:\r\n", object->objID);
    /* prv_instance_t * instance = (prv_instance_t *)object->instanceList; */
    /* while (instance != NULL) */
    /* { */
    /*     fprintf(stdout, "    /%u/%u: shortId: %u, test: %u\r\n", */
    /*             object->objID, instance->shortID, */
    /*             instance->shortID, instance->test); */
    /*     instance = (prv_instance_t *)instance->next; */
    /* } */
#endif
}

/******************************************************************
 * get_object_system(): This function is called by lwm2mgateway.c  * 
 * at start. The function creates and initializes object          *
 * instances.                                                     *
 *****************************************************************/

lwm2m_object_t * get_object_system()  
{
    lwm2m_object_t * objSystem;

    objSystem = (lwm2m_object_t *)malloc(sizeof(lwm2m_object_t));

    if (NULL != objSystem)
    {
        int i;
        prv_instance_t * targetP;

        memset(objSystem, 0, sizeof(lwm2m_object_t));

        objSystem->objID = 1022; // *** MODIFY: the object ID ***

	/* Create object instances - specify just one for System Object */
        for (i=0 ; i < 1 ; i++)   {
            targetP = (prv_instance_t *)malloc(sizeof(prv_instance_t));
            if (NULL == targetP) return NULL;
            memset(targetP, 0, sizeof(prv_instance_t));
            targetP->shortID = i;
            targetP->test = 20 + i;
            objSystem->instanceList = LWM2M_LIST_ADD(objSystem->instanceList, targetP);
        }

        /*
         * From a single instance object, two more functions are available.
         * - The first one (createFunc) create a new instance and filled it with the provided informations. If an ID is
         *   provided a check is done for verifying its availability, or a new one is generated.
         * - The other one (deleteFunc) delete an instance by removing it from the instance list (and freeing the memory
         *   allocated to it)
         */
        objSystem->readFunc = prv_system_read;
        objSystem->writeFunc = prv_system_write;
        objSystem->createFunc = prv_system_create;
        objSystem->deleteFunc = prv_system_delete;
        objSystem->executeFunc = prv_system_exec;
	objSystem->closeFunc = prv_system_close;

    }

    return objSystem;
}
