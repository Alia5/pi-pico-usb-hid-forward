#include "tusb.h"
#include "pio_usb.h"
#include "../device/hid.hpp"

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report,
                      uint16_t desc_len) {
    (void)desc_report;
    (void)desc_len;

    // Interface protocol (hid_interface_protocol_enum_t)
    const char* protocol_str[] = {"None", "Keyboard", "Mouse"};
    uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);

    char tempbuf[256];
    int count = sprintf(tempbuf, "[%04x:%04x][%u] HID Interface%u, Protocol = %s\r\n",
                        vid, pid, dev_addr, instance, protocol_str[itf_protocol]);

    tud_cdc_write(tempbuf, count);
    tud_cdc_write_flush();

    // Receive report from boot keyboard & mouse only
    // tuh_hid_report_received_cb() will be invoked when report is available
    if (itf_protocol == HID_ITF_PROTOCOL_KEYBOARD ||
        itf_protocol == HID_ITF_PROTOCOL_MOUSE) {
        if (!tuh_hid_receive_report(dev_addr, instance)) {
            tud_cdc_write_str("Error: cannot request report\r\n");
        }
    }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    char tempbuf[256];
    int count =
        sprintf(tempbuf, "[%u] HID Interface%u is unmounted\r\n", dev_addr, instance);
    tud_cdc_write(tempbuf, count);
    tud_cdc_write_flush();
}


// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report,
                                uint16_t len) {
    uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

    // TODO: check report_id(protocol) and send according HID report

    switch (itf_protocol) {
        case REPORT_ID_MOUSE:
            // connected mouse may not have same descriptor as our device mouse, length
            // may differ... simply padding can work....
            if (len < sizeof(hid_mouse_report_t)) {
                tud_cdc_write_str("Error: received report with incorrect length; trying "
                                  "simple padding...\r\n");
                std::array<uint8_t, sizeof(hid_mouse_report_t)> report_padded{0};
                memcpy(report_padded.data(), report, len);
                send_hid_report({itf_protocol, {report_padded}});
            }
            break;

        default:
            send_hid_report({itf_protocol, {report, len}});
            break;
    }

    // continue to request to receive report
    if (!tuh_hid_receive_report(dev_addr, instance)) {
        tud_cdc_write_str("Error: cannot request report\r\n");
    }
}
