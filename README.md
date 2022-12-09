# Getting Started with WIZ_io_Security-PSA_SDK

This project was made base on STM32CubeU5 TFM Application. The purpose of the project is to provide FW Update, AWS Connection functions via Ethernet.
For more details on how to setup development environment please refer to following guide.

https://www.st.com/resource/en/user_manual/um2851-getting-started-with-stm32cubeu5-tfm-application-stmicroelectronics.pdf
https://github.com/STMicroelectronics/STM32CubeU5

<a name="hardware_requirements"></a>
## Hardware requirements

Sample codes provided in this SDK can be tested using STM B-U585I-IOT02A with combined W5100S, W5500, W6100 Ethernet Shield. (B-U585I-IOT02A + W5100S or W5500 or W6100 Ethernet Shield)

- [**B-U585I-IOT02A**][link-B-U585I-IOT02A]

<p align="center"><img src="https://www.st.com/bin/ecommerce/api/image.PF271412.en.feature-description-include-personalized-no-cpn-large.jpg"></p>

- [**W5100S Ethernet Shield**][link-w5100s_ethernet_shield]

<p align="center"><img src="https://docs.wiznet.io/assets/images/w5100s_ethernet_shield1-cbe9613f8321ce40886355abedf64b69.png"></p>

- [**W5500 Ethernet Shield**][link-w5500_ethernet_shield]

<p align="center"><img src="https://docs.wiznet.io/assets/images/w5500_main_picture2-503b66733a318af37eb0874700a279b8.png"></p>

- [**W6100 Ethernet Shield**][link-w6100_ethernet_shield]

<p align="center"><img src="https://docs.wiznet.io/assets/images/w6100_ethernet_sheild-1f11e290c5fc7d5b42e4b7c6f168a36b.png"></p>

It is necessary to define WIZnet chip that will be used. To do so, edit WIZCHIP in Defined symbols. (Enter corresponding value W5100S:5105, W5500:5500, W6100:6100 in TFM_Appli_NS, TFM_Loader)

<p align="center"><img src="https://github.com/Wiznet/WIZ_io_Security-PSA_SDK/blob/main/Static/Images/WIZCHIP_Select.png"></p>

<a name="aws_iot_sdk_example"></a>
## AWS IoT SDK example

Clone the sample code and update Submodules. It is also necessary to apply Patch.
Patch is located in WIZ_io_Security-PSA_SDK\Patches directory.
Create a thing in [**AWS_IoT_Core**][link-aws-iot-core]. Then copy Root CA, Client Certification and Private Key to WIZ_io_Security-PSA_SDK\Examples\sample_certificates.h

```cpp
uint8_t aws_iot_root_ca[] =
"-----BEGIN CERTIFICATE-----\r\n"
"..."
"-----END CERTIFICATE-----\r\n";

uint8_t aws_iot_client_cert[] =
"-----BEGIN CERTIFICATE-----\r\n"
"..."
"-----END CERTIFICATE-----\r\n";

uint8_t aws_iot_private_key[] =
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"..."
"-----END RSA PRIVATE KEY-----\r\n";
```

Update the Domain, Topic and Client ID in WIZ_io_Security-PSA_SDK\Examples\aws_iot_sample.c file.

```cpp
#define MQTT_DOMAIN "aqzlwsttrwzrm-ats.iot.ap-northeast-2.amazonaws.com"
#define MQTT_PUB_TOPIC "$aws/things/my_aws_iot_thing/shadow/update"
#define MQTT_SUB_TOPIC "$aws/things/my_aws_iot_thing/shadow/update/accepted"
 
#define MQTT_USERNAME NULL
#define MQTT_PASSWORD NULL
#define MQTT_CLIENT_ID "my_aws_iot_thing"
```

Then Build the project in following order: TFM_SBSFU_Boot, TFM_Appli, TFM_Loader. For more information on this part please follow Section 10. Installation procedure (page 36) of the following guide.
https://www.st.com/resource/en/user_manual/um2851-getting-started-with-stm32cubeu5-tfm-application-stmicroelectronics.pdf

ST SE_STSAFEA_API library
https://github.com/STMicroelectronics/STM32CubeU5/blob/main/Middlewares/ST/SE_STSAFEA_API/readme.txt

ST STSAFE_A1xx library
https://github.com/STMicroelectronics/STM32CubeU5/blob/main/Middlewares/ST/STSAFE_A1xx/readme.txt


Make the device initialization by executing following script WIZ_io_Security-PSA_SDK\Projects\B-U585I-IOT02A\Applications\TFM\TFM_SBSFU_Boot\EWARM\regression.bat
<p align="center"><img src="https://github.com/Wiznet/WIZ_io_Security-PSA_SDK/blob/main/Static/Images/regression.bat_img.png"></p>

Write binary files by executing script WIZ_io_Security-PSA_SDK\Projects\B-U585I-IOT02A\Applications\TFM\TFM_SBSFU_Boot\EWARM\TFM_UPDATE.bat.
<p align="center"><img src="https://github.com/Wiznet/WIZ_io_Security-PSA_SDK/blob/main/Static/Images/TFM_UPDATE.bat_img.png"></p>

Open preferred terminal (COM Port 115200), remove the JP3 Jumper and reconnect.
Press 6 and select WIZCHIP AWS Key Provisining from the menu.
<p align="center"><img src="https://github.com/Wiznet/WIZ_io_Security-PSA_SDK/blob/main/Static/Images/Main_Menu_img.png"></p>

Once message "Key Provisioning Done. 0" appears, press 7 to complete connection to AWS.
<p align="center"><img src="https://github.com/Wiznet/WIZ_io_Security-PSA_SDK/blob/main/Static/Images/Key_Provisioning_Done.png"></p>
<p align="center"><img src="https://github.com/Wiznet/WIZ_io_Security-PSA_SDK/blob/main/Static/Images/Running_AWS.png"></p>

<a name="AWS IoT SDK example Demo Video"></a>
## AWS IoT SDK example Demo Video
https://youtu.be/jFAEf4FO9ps


<a name="TCP Server Firmware Update"></a>
## TCP Server Firmware Update

Select item 3 "New Fw Image" from Main Menu -> Item 2 "TCP Server" -> preferred update menu. (In this guide we selected item 3 "Download NonSecure App Image").
<p align="center"><img src="https://github.com/Wiznet/WIZ_io_Security-PSA_SDK/blob/main/Static/Images/Select_TCP_FW_Update.png"></p>


Open another TCP Client Tool (Teraterm) and connect to IP assigned by DHCP; default port is 8000.
Transmit the WIZ_io_Security-PSA_SDK\Projects\B-U585I-IOT02A\Applications\TFM\TFM_Appli\Binary\tfm_ns_app_sign.bin file via File Transfer. (in case of Teraterm check the box "Binary").
<p align="center"><img src="https://github.com/Wiznet/WIZ_io_Security-PSA_SDK/blob/main/Static/Images/Select_Bin_File.png"></p>

Once transmit is complete, 2 seconds later success message will appear. Then select item 1 "Reset to trigger Installation" from New Fw Image menu.
<p align="center"><img src="https://github.com/Wiznet/WIZ_io_Security-PSA_SDK/blob/main/Static/Images/Reset_to_Trigger.png"></p>

The "TCP Server Firmware Update" function is also included in TFM_Loader project.

<!--
Link
-->

[link-aws-iot-core]: https://aws.amazon.com/iot-core/?nc1=h_ls
[link-B-U585I-IOT02A]: https://www.st.com/en/evaluation-tools/b-u585i-iot02a.html
[link-w5100s_ethernet_shield]: https://docs.wiznet.io/Product/Open-Source-Hardware/w5100s_mkr_ethernet_shield
[link-w5500_ethernet_shield]: https://docs.wiznet.io/Product/Open-Source-Hardware/w5500_ethernet_shield
[link-w6100_ethernet_shield]: https://docs.wiznet.io/Product/Open-Source-Hardware/w6100_mkr_ethernet_shield
