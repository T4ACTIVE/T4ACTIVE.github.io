
# HII ACTIVE Task 4 [Semantic Meta Model Repository](https://t4active.github.io)

## Welcome to Semantic Meta Model Repository
This repository can be seen as a platform for presenting and sharing multiple data models for IoT device management, such as LWM2M, BLE models as well as other non-standard or proprietary models. Moreover, it helps enterprises (organizations) to manage data models for objects and demonstrate them publicly in more appealing way.

### Features

* Browsing diverse data models in a more interactive way
* Searching models, which is implemented by using List.js (a JavaScript library). For more information see [List.js](http://listjs.com/)
* Downloading XML or JSON files containing the data models for use by management servers (Currently supports _**Chrome**_ and _**Firefox**_)
* Allowing new data models to be contributed by pull requests

### To Contribute

We suggest contributing new data models by using Github pull requests. When contributors submit their new data models, it should be placed in a new folder under the existing Model_List folder in the repository. For example the structure for a new LWM2M object is as below:
- `Model_List/`
  - `NewManagedObject/`
    - `NameOfModel.xml`: XML definition of the LWM2M object is _**the minimum requirement**_, e.g. [Gateway_System.xml of Gateway System Object](Model_List/Gateway_System/Gateway_System.xml)
    - `NameOfModel.json`: JSON file of data model, e.g. [Gateway_System.json of Gateway System Object](Model_List/Gateway_System/Gateway_System.json)
    - Optional example usages, messages and interaction diagrams, if needed.
    - `Example.c`/`example.java`: Other available code snippet, or files of data model, for example, code for Wakaama, e.g. [object_system.c of Gateway System Object](Model_List/Gateway_System/object_system.c), or code for Leshan
    - `README.md`: Description of data model and it is optional

Additional object models for other architectures (such as BLE data models, OCF data models or other proprietary models) can also be provided in the same folder.
