
# HII ACTIVE Task 4 Semantic Meta Model Repository [(link)](https://t4active.github.io)

This is a work-in-progress activity of HII ACTIVE Task 4 to undertake the development of a Semantic Meta Model repository, which can allow IoT nodes to be better managed than current practices. This current version of the repository is primarily used to provide a demo of how that can be done. 

Well-understood, open and standardised data models are an essential cornerstone for any IoT device and gateway management system. In addition to such standard data models, many IoT devices and gateways need additional management support for semantic interoperability and properly exposing their capabilities. 

The aim of this semantic meta model repository is to serve as an additional component for IoT management services, by providing a facility for managed nodes to supply data models, object notations as well as running code. A list of example data models can be found [here](Model_List/). 

Features of this repository include:
 
* Version support. As new endpoints are developed, often the data model needs to undergo revisions and refinements. Supporting existing data models while simultaneously allowing new versions to exist is an important step in data model evolution
* Support for multiple data models. An endpoint can often exhibit its properties using multiple data models that need to be mapped with each other. A simple example would be a BLE endpoint which natively exposes its data model as a BLE service, but also supports management using LWM2M via a gateway device or proxy. This allows a management service to understand how to properly map one data model to another
* Ability to share object notations and code. While standard data models can be found from several SDOs, it often becomes advantageous, where possible, to provide platform-specific code implementations of the object being managed
* Support for proprietary or non-standard models. As more and more data models begin to be developed in IoT, situations would arise in which while access methods and the structure of the data may conform to standards, the actual description of the object may be unknown to the management service. By providing a suitable mechanism during object registration, the meta model repository can serve as a trusted reference point for management servers to retrieve various kinds of data models of an endpoint, as well as understand the relationships should the endpoint contain multiple data models.


Currently the repository is aimed primarily at supporting LWM2M and certain non-IP and proprietary object models. However, the architecture of the repository is aimed at flexibly encompassing future IoT management systems such as CoMI, RESTCONF as well as systems conforming to the OCF specifications.
 

Contributors are encouraged to first design their domain-specific data models using online data models supplied by the relevant SDOs, if possible. These definitions, as well as files containing implementation code as well as html code, can then be submitted to the Meta Model Repository as a pull request.
