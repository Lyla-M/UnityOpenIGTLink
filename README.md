# UnityOpenIGTLink
Unity Plugin for streaming data to/from PLUS ultrasound simulator 

**Plugins** folder contains .dll for Unity editor and .so for android devices

**UnityIGT.cs**: Unity code for communicating with the Plugin
 
**UnitySU_Moderate1.xml**: a sample configuration file for PLUS

Source code for compiling the library: 

* [UnityIGTLinkSend.cpp](https://github.com/Lyla-M/UnityOpenIGTLink/blob/master/UnityIGTLinkSend.cpp) send poses of probe, needle and gelblock to PLUS
* [UnityIGTLinkReceive.cpp](https://github.com/Lyla-M/UnityOpenIGTLink/blob/master/UnityIGTLinkReceive.cpp) receive simulated ultrasound image  
 
