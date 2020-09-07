///
/// \file 				Led.h
/// \author 			Geoffrey Hunter (www.mbedded.ninja) <gbmhunter@gmail.com>
/// \edited             n/a
/// \created			2017-04-15
/// \last-modified		2018-03-19
/// \brief 				Contains the Led module.
/// \details
///		See README.md in root dir for more info.

#ifndef LED_H
#define LED_H
void camera_idle();
void starting();
void connecting();
void wifi_fail();
void registering();
void time_out();
void done();
void timeoutQR();
void re_exit();
void timeoutconnecting();

#endif // #ifndef LED_H