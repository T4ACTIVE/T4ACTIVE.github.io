//
//  Copyright (c) 2016, Tampere University of Technology.  
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
 * Implements an object for fixed interface
 *
 *                            Multiple
 *  Object           |  ID  | Instances | Mandatory |
 *  Fixed Interface  | 1024 |    Yes    |    No     |
 *
 *  Resources:
 *                          Supported    Multiple
 *  Name            | ID | Operations | Instances | Mandatory |  Type   | Range | Units | Description |
 *  ifname          |  0 |    R/W     |           |           | String  |       |       |             |
 *  mac address     |  1 |    R/W     |           |           | String  |       |       |             |
 *  status          |  2 |    R/W     |           |           | String  |up/down|       |             |
 *  IP address list |  3 |    R/W     |           |           | List    |       |       |             |
 *  netmask         |  4 |    R/W     |           |           | String  |       |       |             |
 *  proto           |  5 |    R/W     |           |           | String  |       |       |             |
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
     char *ifname;   
 } prv_instance_t;

 /* static void prv_output_buffer(uint8_t * buffer, */
 /* 			       int length) */
 /* { */
 /*     int i; */
 /*     uint8_t array[16]; */

 /*     i = 0; */
 /*     while (i < length) */
 /*     { */
 /* 	 int j; */
 /* 	 fprintf(stderr, "  "); */

 /* 	 memcpy(array, buffer+i, 16); */

 /* 	 for (j = 0 ; j < 16 && i+j < length; j++) */
 /* 	 { */
 /* 	     fprintf(stderr, "%02X ", array[j]); */
 /* 	 } */
 /* 	 while (j < 16) */
 /* 	 { */
 /* 	     fprintf(stderr, "   "); */
 /* 	     j++; */
 /* 	 } */
 /* 	 fprintf(stderr, "  "); */
 /* 	 for (j = 0 ; j < 16 && i+j < length; j++) */
 /* 	 { */
 /* 	     if (isprint(array[j])) */
 /* 		 fprintf(stderr, "%c ", array[j]); */
 /* 	     else */
 /* 		 fprintf(stderr, ". "); */
 /* 	 } */
 /* 	 fprintf(stderr, "\n"); */

 /* 	 i += 16; */
 /*     } */
 /* } */

static uint8_t prv_netif_get_value(char *ifname, lwm2m_data_t * dataP)
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
  case 0: // Resource ID 0 = ifname
    returncode = calluciandgetline("ifname", "network", ifname, &line, &len);
    /* if (returncode != COAP_205_CONTENT) return returncode; */
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
    break;

  case 1: // Resource ID 1 = MAC address
    returncode = calluciandgetline("macaddr", "network", ifname, &line, &len);
    /* if (returncode != COAP_205_CONTENT) return returncode; */
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
    break;

  case 2: // Resource ID 2 = status of the interface
    returncode = calluciandgetline("ifname", "network", ifname, &line, &len);
    if (returncode == COAP_205_CONTENT) { 
      line[len-1] = '\0'; 
      strcpy(buffer,"ifconfig "); 
      strcat(buffer, line); 
      strcat(buffer, " |grep UP > output.txt"); 
      systemCmd(buffer); 
      fp = fopen("output.txt","r"); 

      if (fp == NULL) return COAP_500_INTERNAL_SERVER_ERROR; 
      read = getline(&line, &len, fp);
      fclose(fp);
      if (read == 0 || read == -1) { 
	dataP->length = 4;
	dataP->value = malloc(4);
	memcpy(dataP->value,"Down",4);
      } else { 
	dataP->length = 2;
	dataP->value = malloc(2);
	memcpy(dataP->value,"Up",2);
      }
    } else { 
      /* return returncode; */
      len = 5;
      line = malloc(5);
      memcpy(line, "NULL", 4);
      len--;
      dataP->length = len;
      dataP->value = malloc(len);
      memcpy(dataP->value, line, len);
    }
    
    dataP->flags = 0;
    dataP->type = LWM2M_TYPE_RESOURCE;
    break;

  case 3: // Resource ID 3 = IP address list
    ipAddrList = getIPAddrList("ipaddr", "network", ifname, &ipaddresses);
    
    /* if (ipAddrList == NULL) return COAP_404_NOT_FOUND; */
    
    /* packAddressesToTLV(ipAddrList, &ipaddresses, dataP); */

    if (ipAddrList != NULL) {
      packAddressesToTLV(ipAddrList, &ipaddresses, dataP);
    } else { 
      len = 5;
      line = malloc(5);
      memcpy(line, "NULL", 4);
      len--;
      dataP->length = len;
      dataP->value = malloc(len);
      memcpy(dataP->value, line, len);
      dataP->flags = 0;
      dataP->type = LWM2M_TYPE_RESOURCE;
    }
    

    break;

 case 4: // Resource ID 4 = netmask
    returncode = calluciandgetline("netmask", "network", ifname, &line, &len);
    /* if (returncode != COAP_205_CONTENT) return returncode; */
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
    break;

 case 5: // Resource ID 5 = proto
    returncode = calluciandgetline("proto", "network", ifname, &line, &len);
    /* if (returncode != COAP_205_CONTENT) return returncode; */
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
    break;


  }
  if (line != NULL)
    free(line);
  return COAP_205_CONTENT; // Default return code

}   


/******************************************************************
 * prv_netif_read(): this is called by lwm2mclient.c to obtain values   *
 * for a specific resource (numDataP indicating resource number)  *
 * or for all resources of this object (numDataP indicating 0).   *
 * This function prepares response data structure and then calls  *
 * prv_netif_get_value multiple times to get data for each resource.    *
 *****************************************************************/

static uint8_t prv_netif_read(uint16_t instanceId,
                        int * numDataP,
                        lwm2m_data_t ** dataArrayP,
                        lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;
    int i;
    uint8_t returncode = COAP_205_CONTENT;

    fprintf(stdout,"prv_netif_read, instanceID: %i, numDataP: %i\n",instanceId, *numDataP);

    targetP = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;
    if (*numDataP == 0)
      {
	fprintf(stdout,"Server is asking for the whole object\n");

        *dataArrayP = lwm2m_data_new(6); // *** MODIFY: number of TLV resources. ***

        if (*dataArrayP == NULL) {
	  fprintf(stdout,"prv_netif_read returning COAP_500_INTERNAL_SERVER_ERROR");
	  return COAP_500_INTERNAL_SERVER_ERROR;
	}
        *numDataP = 6; // *** MODIFY: number of TLV resources *** 

       	for (i = 0; i < 6; i++) { // *** MODIFY: i < number of resources ***
 
	  (*dataArrayP)[i].id = i;
	}
    }
    else {
      fprintf(stdout,"Server is asking just for resource: %i\n",(*dataArrayP)->id);
    }

    i = 0;
    do {
      returncode = prv_netif_get_value(targetP->ifname, (*dataArrayP)+i);
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
 * prv_netif_write: Handlers for writing each resource. Handlers        *
 * construct UCI commands for storing new settings and then       *
 * execute those.                                                 *
 *****************************************************************/


static uint8_t prv_netif_write(uint16_t instanceId,
                         int numData,
                         lwm2m_data_t * dataArray,
                         lwm2m_object_t * objectP)
{
  prv_instance_t * targetP;
  int64_t value;
  char buffer[100];
  uint8_t returncode = COAP_204_CHANGED;
  char *line = NULL;
  size_t len = 0;
  
  memset(buffer, 0, 100);
  
  targetP = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
  if (NULL == targetP || numData != 1) return COAP_404_NOT_FOUND;
  
  switch (dataArray->id) {

  case 0:
    strcpy(buffer,"uci set network.");
    strcat(buffer,targetP->ifname);
    strcat(buffer,".ifname=");
    memcpy(buffer + strlen(buffer), dataArray->value, dataArray->length);
    break;

  case 1:
    strcpy(buffer,"uci set network.");
    strcat(buffer,targetP->ifname);
    strcat(buffer,".macaddr=");
    memcpy(buffer + strlen(buffer), dataArray->value, dataArray->length);
    break;

  case 2:
    returncode = calluciandgetline("ifname", "network", 
				   targetP->ifname, &line, &len);
    if (returncode == COAP_205_CONTENT) {  
      line[len-1] = 0; 
      strcpy(buffer,"ifconfig ");
      strcat(buffer, line);
      
      if (dataArray->length == 2 &&
	  strncmp(dataArray->value, "Up", 2) == 0) {
	strcat(buffer, " up");
      } 
      else if (dataArray->length == 4 &&
	       strncmp(dataArray->value, "Down", 4) == 0) {
	strcat(buffer, " down");
      }
      else return COAP_404_NOT_FOUND;
    } else {
      return COAP_404_NOT_FOUND;
    }    
    break;

  case 3:
    return calluciwriteaddresses("ipaddr", "network", 
				 targetP->ifname, dataArray->value, 
				 dataArray->length);
    break;

 case 4:
    strcpy(buffer,"uci set network.");
    strcat(buffer,targetP->ifname);
    strcat(buffer,".netmask=");
    memcpy(buffer + strlen(buffer), dataArray->value, dataArray->length);
    break;

  case 5:
    strcpy(buffer,"uci set network.");
    strcat(buffer,targetP->ifname);
    strcat(buffer,".proto=");
    memcpy(buffer + strlen(buffer), dataArray->value, dataArray->length);
    break;

  default:
    printf("Write to unknown resource\n");
    return COAP_404_NOT_FOUND;
  }

  systemCmd(buffer);
  systemCmd("uci commit");
  return COAP_204_CHANGED; 
}


/******************************************************************
 * prv_netif_delete(): This function would be used to delete object     *
 * instances. Unmodified & not tested.                            *
 *****************************************************************/

static uint8_t prv_netif_delete(uint16_t id,
                          lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;

    objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&targetP);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    free(targetP);

    return COAP_202_DELETED;
}

/******************************************************************
 * prv_netif_create(): This function would be used to create new object *
 * instances. Unmodified & not tested.                            *
 *****************************************************************/


static uint8_t prv_netif_create(uint16_t instanceId,
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

    result = prv_netif_write(instanceId, numData, dataArray, objectP);

    if (result != COAP_204_CHANGED)
    {
        (void)prv_netif_delete(instanceId, objectP);
    }
    else
    {
        result = COAP_201_CREATED;
    }

    return result;
}

/******************************************************************
 * prv_netif_exec(): This function is used for execute commands.        *
 * This was just quickly tested.                                  *
 *****************************************************************/

static uint8_t prv_netif_exec(uint16_t instanceId,
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

static void prv_netif_close(lwm2m_object_t * object)
{
    LWM2M_LIST_FREE(object->instanceList);
    if (object->userData != NULL)
    {
        lwm2m_free(object->userData);
        object->userData = NULL;
    }
}

void display_netif_object(lwm2m_object_t * object)
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
 * get_object_system(): This function is called by lwm2mclient.c  * 
 * at start. The function creates and initializes object          *
 * instances.                                                     *
 *****************************************************************/

lwm2m_object_t * get_object_netif()  // *** MODIFY ***
{
  lwm2m_object_t * objNetIf; 
    char ifname[100];
    char *linePtr = NULL;
    FILE *fp;
    ssize_t read;
    ssize_t len;
    uint16_t icount = 0;


    objNetIf = (lwm2m_object_t *)malloc(sizeof(lwm2m_object_t));

    if (NULL != objNetIf)
    {
        int i;
        prv_instance_t * targetP;

        memset(objNetIf, 0, sizeof(lwm2m_object_t));

        objNetIf->objID = 1024; // *** MODIFY: the object ID ***

	// Scan for interfaces currently configured
	systemCmd("uci show network | grep interface > output.txt");
	fp = fopen("output.txt","r");
	if (fp != NULL) {
	  do {
	    read = getline(&linePtr, &len, fp);
	    if (read > 0) {
	      char* pos = linePtr;
	      
	      // Advance pointer until dot or end of read data
	      while (*pos != '.' && pos <= linePtr+read) pos++;

	      if (*pos == '.') { // Was dot found?
		pos++; // Advance pointer over dot

		// Advance pointer until equal sign or end of read data
		// copy characters while seeking equal sign
		int i = 0;
		while (*pos != '=' && pos <= linePtr+read) {
		  ifname[i] = *pos;
		  i++;
		  pos++;
		}
		ifname[i] = '\0'; // Terminate string

		// Check if we found equal sign, if so, we have ifname
		if (*pos == '=') {
		  printf("Found network interface: >%s<, creating object instance %i\n", ifname, icount);
		  targetP = (prv_instance_t *)malloc(sizeof(prv_instance_t));
		  if (NULL == targetP) return NULL;
		  memset(targetP, 0, sizeof(prv_instance_t));
		  targetP->shortID = icount;
		  icount++;
		  char *tempPtr = malloc(strlen(ifname)+1);
		  strcpy(tempPtr, ifname);
		  targetP->ifname = tempPtr;
		  objNetIf->instanceList = LWM2M_LIST_ADD(objNetIf->instanceList, targetP);
		}	       
	      }
	    }
	    free(linePtr);
	    linePtr = NULL;
	  } while (read > 0);
	}

	/* Create four object instances - mostly for testing purposes */
	/*        for (i=0 ; i < 4 ; i++) 
        {
            targetP = (prv_instance_t *)malloc(sizeof(prv_instance_t));
            if (NULL == targetP) return NULL;
            memset(targetP, 0, sizeof(prv_instance_t));
            targetP->shortID = i;
	    if (i == 0) targetP->ifname = "loopback";
	    if (i == 1) targetP->ifname = "lan";
	    if (i == 2) targetP->ifname = "wan";
	    if (i == 3) targetP->ifname = "wan6";
            objNetIf->instanceList = LWM2M_LIST_ADD(objNetIf->instanceList, targetP);
	    }*/
        /*
         * From a single instance object, two more functions are available.
         * - The first one (createFunc) create a new instance and filled it with the provided informations. If an ID is
         *   provided a check is done for verifying his disponibility, or a new one is generated.
         * - The other one (deleteFunc) delete an instance by removing it from the instance list (and freeing the memory
         *   allocated to it)
         */
        objNetIf->readFunc = prv_netif_read;
        objNetIf->writeFunc = prv_netif_write;
        objNetIf->createFunc = prv_netif_create;
        objNetIf->deleteFunc = prv_netif_delete;
        objNetIf->executeFunc = prv_netif_exec;
	/* objNetIf->closeFunc = prv_netif_close; */

    }

    return objNetIf;
}
