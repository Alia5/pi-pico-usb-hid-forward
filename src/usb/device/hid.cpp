#include "hid.hpp"

void send_hid_report(const hid_report& report) {
    // skip if hid is not ready yet
    if (!tud_hid_ready())
        return;

    printf("send_hid_report: %d\n", report.id);
    tud_hid_report(report.id, report.data.data(), report.data.size());
}


// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is
// complete
void hid_task(void) {
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    //   if ( board_millis() - start_ms < interval_ms) return; // not enough time
    //   start_ms += interval_ms;

    //   uint32_t const btn = board_button_read();

    // TODO:

    // Remote wakeup
    if (tud_suspended()) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    } else {
        // Send the 1st of report chain, the rest will be sent by
        // tud_hid_report_complete_cb()
        // TODO:
        // send_hid_report(REPORT_ID_KEYBOARD, false);
    }
}