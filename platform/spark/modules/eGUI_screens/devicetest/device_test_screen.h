/*
 * Copyright 2015 BrewPi / Elco Jacobs, Matthew McGowan.
 *
 * This file is part of BrewPi.
 *
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "Board.h"

#ifdef	__cplusplus
extern "C" {
#endif

void SetActuatorButtonState(const D4D_OBJECT* pThis, D4D_BOOL state, uint8_t idx);


D4D_EXTERN_SCREEN(screen_devicetest);
D4D_EXTERN_OBJECT(scrDeviceTest_devices00);
D4D_EXTERN_OBJECT(scrDeviceTest_devices01);
D4D_EXTERN_OBJECT(scrDeviceTest_devices10);
D4D_EXTERN_OBJECT(scrDeviceTest_devices11);
D4D_EXTERN_OBJECT(scrDeviceTest_devices20);
D4D_EXTERN_OBJECT(scrDeviceTest_devices21);

D4D_EXTERN_OBJECT(scrDeviceTest_actuator0);
D4D_EXTERN_OBJECT(scrDeviceTest_actuator1);
D4D_EXTERN_OBJECT(scrDeviceTest_actuator2);
D4D_EXTERN_OBJECT(scrDeviceTest_actuator3);
#if MAX_ACTUATOR_COUNT > 4
D4D_EXTERN_OBJECT(scrDeviceTest_actuator4);
#endif

D4D_EXTERN_OBJECT(scrDeviceTest_wifi_state);
D4D_EXTERN_OBJECT(scrDeviceTest_usb_state);

void ScreenDeviceTest_OnInit_c();
void ScreenDeviceTest_OnInit();
void ScreenDeviceTest_OnMain();
void ScreenDeviceTest_OnActivate();
void ScreenDeviceTest_OnDeactivate();
Byte ScreenDeviceTest_OnObjectMsg(D4D_MESSAGE* pMsg);


#ifdef	__cplusplus
}
#endif

