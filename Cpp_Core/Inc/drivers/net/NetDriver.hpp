/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: Copyright (c) 2024 OpenStickCommunity (gp2040-ce.info)
 */

#ifndef _NET_DRIVER_H_
#define _NET_DRIVER_H_  

#include "gpdriver.hpp"

class NetDriver : public GPDriver {
public:
    void initialize();
    void process(Gamepad * gamepad, uint8_t * outBuffer);
    void initializeAux() {};
    void processAux() {};
    uint16_t get_report(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen);
    void set_report(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize);
    bool vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request);
    const uint16_t * get_descriptor_string_cb(uint8_t index, uint16_t langid);
    const uint8_t * get_descriptor_device_cb();
    const uint8_t * get_hid_descriptor_report_cb(uint8_t itf);
    const uint8_t * get_descriptor_configuration_cb(uint8_t index);
    const uint8_t * get_descriptor_device_qualifier_cb();
    uint16_t GetJoystickMidValue();
    USBListener * get_usb_auth_listener() { return nullptr; }
private:
    // usbd_class_driver_t class_driver;
};



#endif // _NET_DRIVER_H_
