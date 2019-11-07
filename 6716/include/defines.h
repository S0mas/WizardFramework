#pragma once

#ifdef __linux__
#	include <unistd.h>
#	define Sleep(x) usleep(x * 1000)
#elif _WIN32
#	include <Windows.h>
#endif

#include <time.h>
#include <math.h>

#define BU6716_RES "TCPIP0::192.168.168.135::5025::SOCKET"
#define BU3416_BU6716_RES "TCPIP0::192.168.168.126::INSTR"
#define BU3416_T028_RES "TCPIP0::192.168.168.126::INSTR"
#define BU3416_BU6716_FC 2
#define BU3416_T028_FC 4

#define AUTO_DAC_POSITIVE   0
#define AUTO_DAC_NEGATIVE   1

#define bu6716_FPGA_ID							0x00  // and 0x01   (16 bit register, big endian)
#define bu6716_FPGA_VER							0x02  // and 0x03   (16 bit register, big endian)
#define bu6716_FPGA_SEGSW						0x04
#define bu6716_FPGA_CSR							0x05
#define bu6716_FPGA_CHN_IDENT					0x06
#define bu6716_FPGA_ICP_STS						0x07
#define bu6716_FPGA_EX_LIMIT_STS				0x08
#define bu6716_FPGA_ICP_ERR						0x09
#define bu6716_FPGA_EX_LIMIT_ERR				0x0A
#define bu6716_FPGA_ICP2SCU_ERR					0x0B
#define bu6716_FPGA_EXL2SCU_ERR					0x0C
#define bu6716_FPGA_SEGCONF						0x0D
#define bu6716_FPGA_DACB3						0x0E
#define bu6716_FPGA_DACB2						0x0F
#define bu6716_FPGA_DACB1						0x10
#define bu6716_FPGA_DACMASK						0x11
#define bu6716_FPGA_TEST_RW						0x12
#define bu6716_FPGA_TEST_LEDS					0x13
#define bu6716_FPGA_CSR_DEFAULT                 0x04
#define bu6716_FPGA_TEDS_ACC					0x14
#define bu6716_FPGA_TEDS_DATA					0x15

#define bu6716_DAC_AUTO_GAIN		(10.0/0x7fff)

#define bu6716_TEDS_REG_STATUS  	0xF0
#define bu6716_TEDS_REG_DATA    	0xE1
#define bu6716_TEDS_REG_CFG     	0xC3

#define bu6716_TEDS_CFG_DFL			0xE1 /* TODO */
#define bu6716_TEDS_CMD_DRST		0xF0 /* Device Reset Command */
#define bu6716_TEDS_CMD_SRP			0xE1 /* Set Read Pointer Command */
#define bu6716_TEDS_CMD_WCFG		0xD2 /* Write Configuration Command */
#define bu6716_TEDS_CMD_1WRS		0xB4 /* 1-Wire Reset Command */
#define bu6716_TEDS_CMD_1WSB		0x87 /* 1-Wire Single Bit Command */
#define bu6716_TEDS_CMD_1WWB		0xA5 /* 1-Wire Write Byte Command */
#define bu6716_TEDS_CMD_1WRB		0x96 /* 1-Wire Read Byte Command */
#define bu6716_TEDS_CMD_1WT			0x78 /* 1-Wire Triplet Command */

#define bu6716_TEDS_STATUS_1WB		0x01 /* 1-Wire Busy */
#define bu6716_TEDS_STATUS_PPD		0x02 /* Presence-Pulse Detected */
#define bu6716_TEDS_STATUS_SD		0x04 /* Short Detected */
#define bu6716_TEDS_STATUS_LL		0x08 /* Logic Level */
#define bu6716_TEDS_STATUS_RST		0x10 /* Device Reset */
#define bu6716_TEDS_STATUS_SBR		0x20 /* Single Bit Result */
#define bu6716_TEDS_STATUS_TSB		0x40 /* Triplet Second Bit */
#define bu6716_TEDS_STATUS_DIR		0x80 /* Branch Direction Taken */

#define bu6716_1W_CMD_READ_ROM		0x33 /* Read ROM */
#define bu6716_1W_CMD_MATCH_ROM		0x55 /* Match ROM */
#define bu6716_1W_CMD_SKIP_ROM		0xCC /* Skip ROM */
#define bu6716_1W_CMD_SEARCH_ROM	0xF0 /* Search ROM */

#define bu6716_1W_CMD_WRITE_SPD		0x0F /* Write Scratchpad */
#define bu6716_1W_CMD_READ_SPD		0xAA /* Read Scratchpad */
#define bu6716_1W_CMD_COPY_SPD		0x55 /* Copy Scratchpad */
#define bu6716_1W_CMD_READ_MEM		0xF0 /* Read Memory */
#define bu6716_1W_CMD_WRITE_APR		0x99 /* Write Application Register */
#define bu6716_1W_CMD_READ_STR		0x66 /* Read Status Register */
#define bu6716_1W_CMD_READ_APR		0xC3 /* Read Application Register */
#define bu6716_1W_CMD_CPLK_APR		0x5A /* Copy & Lock Application Register */
#define bu6716_1W_CMD_VALID_KEY		0xA5 /* Validation Key */
