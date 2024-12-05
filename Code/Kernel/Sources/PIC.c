#include "../Includes/PIC.h"

void pic_init(void) {
//    uint8_t mask1 = inPort(PIC1_DATA);
//    uint8_t mask2 = inPort(PIC2_DATA);

    outPort(PIC1_COMMAND, 0x11);
    outPort(PIC2_COMMAND, 0x11);

    outPort(PIC1_DATA, 0x20);
    outPort(PIC2_DATA, 0x28);

    outPort(PIC1_DATA, 0x04);
    outPort(PIC2_DATA, 0x02);

    outPort(PIC1_DATA, 0x01);
    outPort(PIC2_DATA, 0x01);

    // Enable keyboard and PIT timer only
    outPort(PIC1_DATA, 0xFC);
    outPort(PIC2_DATA, 0xFF);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outPort(PIC2_COMMAND, PIC_EOI);
    }
    outPort(PIC1_COMMAND, PIC_EOI);
}