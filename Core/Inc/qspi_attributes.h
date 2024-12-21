#ifndef QSPI_ATTRIBUTES_H
#define QSPI_ATTRIBUTES_H

#define QSPI_CODE __attribute__((section(".text.qspi")))
#define QSPI_RODATA __attribute__((section(".rodata.qspi")))

#endif // QSPI_ATTRIBUTES_H 