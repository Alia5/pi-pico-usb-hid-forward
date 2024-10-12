#include "tusb.h"
#include "../usb_descriptors.h"
#include <span>

struct hid_report {
    uint8_t id; // TODO;
    std::span<const uint8_t> data;
};

void send_hid_report(const hid_report& report);
void hid_task(void);
