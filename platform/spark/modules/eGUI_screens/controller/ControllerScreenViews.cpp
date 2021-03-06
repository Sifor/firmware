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

#include "ControllerScreenViews.h"
#include "ConnectivityDisplay.h"
#include "TempControl.h"
#include "controller_screen.h"

bool set_background_color(const D4D_OBJECT* pThis, D4D_COLOR bg)
{
    D4D_COLOR existing = pThis->clrScheme->bckg;    
    pThis->clrScheme->bckg = bg;
    pThis->clrScheme->bckgDis = bg;
    pThis->clrScheme->bckgCapture = bg;
    pThis->clrScheme->bckgFocus = bg;
    if (existing!=bg)
        D4D_InvalidateObject(pThis, D4D_TRUE);
    return existing!=bg;
}

const char* ControllerStatePresenter::state_name[] {
    "IDLE",
    "OFF",
    "DOOR OPEN",
    "HEATING",
    "COOLING",
    "COOL IN...",
    "PEAK DET.",
    "COOLING.",
    "HEATING."
};

D4D_COLOR ControllerStatePresenter::state_color[] = {
	IDLE_COLOR,                     // 0
	STATE_OFF_COLOR,				// 1
	DOOR_OPEN_COLOR,				// 2 used by the Display only
	HEATING_COLOR,					// 3
	COOLING_COLOR,					// 4
	WAITING_TO_COOL_COLOR,			// 5
	WAITING_FOR_PEAK_DETECT_COLOR,	// 6
	COOLING_MIN_TIME_COLOR,			// 7
};



const D4D_OBJECT* stateDisplay[] = { &scrController_state, &scrController_time };
const D4D_OBJECT* beerTempDisplay[] = { &scrController_beertemp, &scrController_beersv, &scrController_beer };
const D4D_OBJECT* fridgeTempDisplay[] = { &scrController_fridgetemp, &scrController_fridgesv, &scrController_fridge};
const D4D_OBJECT* loggingDisplay[] = { &scrController_log1temp, &scrController_log2temp, &scrController_log3temp, &scrController_logging};

ControllerStateView stateView(stateDisplay);
ControllerStatePresenter statePresenter(stateView);

TemperatureProcessView beerTempView(beerTempDisplay);
TemperatureProcessPresenter beerTempPresenter(beerTempView, BEER_BG_COLOR);

TemperatureProcessView fridgeTempView(fridgeTempDisplay);
TemperatureProcessPresenter fridgeTempPresenter(fridgeTempView, FRIDGE_BG_COLOR);

TemperatureLoggingView loggingTempView(loggingDisplay);
TemperatureLoggingPresenter loggingTempPresenter(loggingTempView, LOG_BG_COLOR);

ControllerModeView modeView(&scrController_mode);
ControllerModePresenter modePresenter(modeView);

ControllerTimeView timeView(&scrController_time);
ControllerTimePresenter timePresenter(timeView);

TextView tempFormatView(&scrController_lbl_tempunit);
ControllerTemperatureFormatPresenter tempFormatPresenter(tempFormatView);


void asString(char* buf, temp_t t, unsigned num_decimals, unsigned max_len)
{
    if (t.isDisabledOrInvalid()) {
        strcpy(buf, "--.-");
    }
    else
        t.toTempString(buf, num_decimals, max_len, tempControl.cc.tempFormat, true);
}

const char* ltrim(const char* s) {
    for (;*s==' ';s++);
    return s;
}

uint16_t fetch_time(states state)
{
    tcduration_t time = 0;
    tcduration_t sinceIdleTime = tempControl.timeSinceIdle();
    if(state==IDLE){
        time = min(tempControl.timeSinceCooling(), tempControl.timeSinceHeating());
    }
    else if(state==COOLING || state==HEATING){
        time = sinceIdleTime;
    }
    return time;
}

void ControllerScreen_Update()
{
    tempFormatPresenter.update();
    states state = states(tempControl.getState());
    statePresenter.setState(state);
    modePresenter.update(tempControl.getMode());
    timePresenter.update();
    
    beerTempPresenter.update(tempControl.getBeerTemp(), tempControl.getBeerSetting());
    fridgeTempPresenter.update(tempControl.getFridgeTemp(), tempControl.getFridgeSetting());
    loggingTempPresenter.update(tempControl.getLog1Temp(), tempControl.getLog2Temp(), tempControl.getLog3Temp());
    usbPresenter.update();
    wifiPresenter.update();
}

void TemperatureProcessPresenter::update(temp_t current, temp_t setpoint, bool has_setpoint)
{
    char current_str[MAX_TEMP_LEN];
    char setpoint_str[MAX_TEMP_LEN];

    asString(current_str, current, 1, MAX_TEMP_LEN);
    asString(setpoint_str, setpoint, 1, MAX_TEMP_LEN);        
    view_.setBgColor(bg_col);
    view_.update(ltrim(current_str), has_setpoint ? ltrim(setpoint_str) : NULL);
}          

void TemperatureLoggingPresenter::update(temp_t log1, temp_t log2, temp_t log3)
{
    char log1_str[MAX_TEMP_LEN];
    char log2_str[MAX_TEMP_LEN];
    char log3_str[MAX_TEMP_LEN];


    asString(log1_str, log1, 1, MAX_TEMP_LEN);
    asString(log2_str, log2, 1, MAX_TEMP_LEN);
    asString(log3_str, log3, 1, MAX_TEMP_LEN);
    view_.setBgColor(bg_col);
    view_.update(ltrim(log1_str), ltrim(log2_str), ltrim(log3_str));
}

const char* ControllerTemperatureFormatPresenter::formatText()
{
    switch (tempControl.cc.tempFormat) {
        case 'C': return TEMP_FORMAT_C_TEXT;
        case 'F': return TEMP_FORMAT_F_TEXT;
        default:
            return "";
    }
}

void ScrController_OnInit()
{
    wifiView.setTarget(&scrController_wifi_state);
    usbView.setTarget(&scrController_usb_state);
}

void ScrController_OnMain()
{
    ControllerScreen_Update();
}


void ScrController_OnActivate()
{    
}

void ScrController_OnDeactivate()
{
}

Byte ScrController_OnObjectMsg(D4D_MESSAGE* pMsg)
{
    D4D_UNUSED(pMsg);
    return 0;
}

