///
/// \file 				Led.c
/// \author 			Geoffrey Hunter (www.mbedded.ninja) <gbmhunter@gmail.com>
/// \edited             n/a
/// \created			2017-04-15
/// \last-modified		2018-03-19
/// \brief 				Contains the Led module.
/// \details
///		See README.md in root dir for more info.

#include <stdio.h>


void starting() {
    printf("\"Chuan bi ket noi\"\r\n");
    
    printf("\"Dat QR truoc Camera\"\n");
    
}

void connecting() {
    printf("Beep\n");
    printf("\"Dang ket noi vui long doi.\"\r\n");
    
}

void camera_idle() {
    printf("Camera in idle state.\r\n");
    
}

void wifi_fail(){
	printf("ssid,paswork fail\n");
}
void registering(){
	printf("\"Thiet bi dang duoc tai len\"\n");
    
}
void time_out(){
	printf("Time out\n");
}

void done(){
    printf("Dang ky thanh cong\n");
}
void timeoutQR(){
    printf("Het thoi gian QR code\n");
}
void re_exit(){
    printf("Beep !!! He thong ban !!!\n");
}
void timeoutconnecting(){
    printf("Het thoi gian connecting\n");
}
