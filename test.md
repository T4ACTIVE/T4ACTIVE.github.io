Object models of Gateway System

- [HOME](index.html)
- [MODELS](models.html)
- [CONTRIBUTE](contribute.html)
- [ABOUT US](aboutUs.html)


#### Gateway System

[VIEW ON GITHUB](https://github.com/T4ACTIVE/T4ACTIVE.github.io/tree/master/Model_List/Gateway_System)

#### LWM2M

#### object version: urn:oma:lwm2m:ext:1022:1

#### descripton

This Object defines the basic settings for a generic gateway.

#### Resource fields

| Resource        | ID | Access Type | Multiple Instances | Mandatory | type   | Description |
| --------------- |--- | ----------- | -----------------  | ----------| -------| ----------- |
| Hostname        | 0  | R,W         | Single             | Mandatory | String | This resource type returns the hostname of the gateway. |
| Timezone        | 1  | R,W         | Single             | Optional  | String | This resource type returns the default timezone. |
| DNS Server List | 2  | R,W         | Single             | Optional  | String | This resource type returns a list of DNS Servers used by this system. |
| NTP Server List | 3  | R,W         | Single             | Optional  | String | This resource type returns a list of NTP Servers. |

#### Example usage

The gateway system object contains the most basic functionalities for a gateway such as its name, timezone and so on. All properties are represented as readable and writable resources using standard LWM2M operations.

#### XML

#### JSON

#### Code

#### Wakaama

[object_system.c](Gateway_System/object_system.c)

#### UCI

#### Description

OpenWRT gateway native data model for the whole gateway

#### Config

#### URL

file://localhost/etc/config/system


#### System

#### Description

The system section contains settings that apply to the most basic operation of the system

| Name     | Type   | Required | Default | Description | 
| -------- | ------ | -------- | --------| ----------  | 
| hostname | string | no       | OpenWrt | The hostname for this system. Avoid points, even if they are within single or double quotes. For example 'my.hostname' will show only the 'my' part. |
| timezone | string | no       | UTC     | The time zone that date and time should be rendered in by default. | 

#### Timeserver

#### Description

The timeserver section contains a list of NTP servers for the system to use

#### List fields

| Name   | Type              | Required | Default | Description | 
| ----   | ----------------  | -------- | --------| ----------  | 
| server | list of hostnames | no       | none    | Defines the pool of NTP servers to poll the time from. If the list is empty, the builtin NTP daemon is not started. |

#### Option fields

| Name          | Type    | Required | Default | Description | 
| -----------   | -----   | -------- | --------| ----------  | 
| enable_server | boolean | no       | 0       | You can put busybox-ntpd in client mode only: by defining at least one host to server and puting enable_server 0 client & server mode: by putting enable_server to 1, (busybox-ntpd listens to UDP 123 by default), server mode only: by not defining any servers in the config and just put enable_server 1 (ntpd will answer with the time of the router) |
