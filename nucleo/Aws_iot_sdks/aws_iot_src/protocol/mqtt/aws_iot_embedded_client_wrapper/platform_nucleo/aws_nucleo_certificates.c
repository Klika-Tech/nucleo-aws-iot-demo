/**
 ******************************************************************************
 * @file    aws_nucleo_certificates.c
 * @author  Central LAB
 * @version V0.1.0
 * @date    10-March-2016
 * @brief   AWS IoT Device Certificates, Private Key file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


#include "stdint.h"


/**********************************************************************************************
***********************************************************************************************
													Root CA certificate: Never modify
***********************************************************************************************
***********************************************************************************************/

/************************************************************************
Downloaded from https://www.symantec.com/content/en/us/enterprise/verisign/roots/VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem
*************************************************************************/
//This root CA can be used.
const uint8_t rootCA[] = "\
-----BEGIN CERTIFICATE-----\n\
MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\
yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\
ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\
U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\
ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\
aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\
MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\
ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\
biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\
U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\
aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\
nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\
t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\
SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\
BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\
rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\
NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\
BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\
BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\
aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\
MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\
p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\
5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\
WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\
4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\
hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao8WNq\n\
-----END CERTIFICATE-----\n";

/**********************************************************************************************
***********************************************************************************************
													Device Identity Certificates: Modify for your AWS IoT Thing
***********************************************************************************************
***********************************************************************************************/

/****************************************
(somecode)-certificate.pem.crt - Amazon signed PEM sertificate.
*****************************************/

//This Client cert is example. Use own instead.
const uint8_t clientCRT[] = "\
-----BEGIN CERTIFICATE-----\n\
MIIDBjCCAe6gAwIBAgIUVph856omeIxW3UPioq+UrX1DbwowDQYJKoZIhvcNAQEL\
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE3MDUyNTExNTEy\
OVoXDTQ5MTIzMTIzNTk1OVowgZUxCzAJBgNVBAYTAkJZMQ4wDAYDVQQIDAVNaW5z\
azEOMAwGA1UEBwwFTWluc2sxFzAVBgNVBAoMDktsaWthLVRlY2ggTExDMRcwFQYD\
VQQLDA5LbGlrYS1UZWNoIExMQzEMMAoGA1UEAwwDUm5EMSYwJAYJKoZIhvcNAQkB\
FhdtdmF0YWxldUBrbGlrYS10ZWNoLmNvbTBZMBMGByqGSM49AgEGCCqGSM49AwEH\
A0IABCJgOQJmoTBJVPfli9Hm/JVixaxkY5rtlgrYO3hSl633A2hg0P/ue0wXDbF3\
aQ0X57IRFE4k4FEbr3UXjT/IczKjYDBeMB8GA1UdIwQYMBaAFK3YzTUPlYB2Li75\
i/z8rEogr1d6MB0GA1UdDgQWBBT18HXBaXFJuAR/0SwegnxJ+pyJ6TAMBgNVHRMB\
Af8EAjAAMA4GA1UdDwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAb0Ux1aH5\
RLxjrfGqXN6rPVqh8QQRS+AyBfzmaQN8HaPZMkX5WxXLvcn0A3uWlwQxPPkcZ4zf\
51GHtFFQWB4YZ8dx8mUQ0v/j7onHjCJgZ8iDgwOyKMGtnsDZWCakQw+a6cj+NrMZ\
tzhjwCzEEP6ePcbXwErI5OOzLuWns2L/JEr2wWNkokgRuS8ewr/SQ9OLWIWa2rFM\
ahPNTb3y/qBeWdjeJmhI+TOxdqIpsF8roWP25zwo/zkzCHCjXFBrL+0CA4MpxIl9\
x02i7aAhlJ6ys80lDxdeWeeQJXRKkGknP8mcmKn3iEqqJ5s1dQePj2b5d4ldatya\
wsxQBqqZXzIWEw==\n\
-----END CERTIFICATE-----\n";


/**********************************************************************************************
***********************************************************************************************
													Private Key: Modify for your AWS IoT Thing
***********************************************************************************************
***********************************************************************************************/


/********************************************************************8****************************************
nucleo.key.pem - client key generated according to readme.
**************************************************************************************************************/

//This Client Key is example. Use own instead.
const uint8_t clientKey[] ="\
-----BEGIN EC PARAMETERS-----\n\
BggqhkjOPQMBBw==\
-----END EC PARAMETERS-----\n\
-----BEGIN EC PRIVATE KEY-----\n\
MHcCAQEEIHPRfWSC8/k/BsqDWKuP15dXsI9fGwpkTIsLZe6mIrAAoAoGCCqGSM49\
AwEHoUQDQgAEImA5AmahMElU9+WL0eb8lWLFrGRjmu2WCtg7eFKXrfcDaGDQ/+57\
TBcNsXdpDRfnshEUTiTgURuvdReNP9hzMg==\
-----END EC PRIVATE KEY-----\n";
