
#ifndef RF231_H_
#define RF231_H_


#define MAX_TX_RETRIES    15
#define DEFAULT_CHANNEL   15

#define HAL_DUMMY_READ         (0xAA) //!< Dummy value for the SPI.
#define HAL_TRX_CMD_RW         (0xC0) //!< Register Write (short mode).
#define HAL_TRX_CMD_RR         (0x80) //!< Register Read (short mode).
#define HAL_TRX_CMD_FW         (0x60) //!< Frame Transmit Mode (long mode).
#define HAL_TRX_CMD_FR         (0x20) //!< Frame Receive Mode (long mode).
#define HAL_TRX_CMD_SW         (0x40) //!< SRAM Write.
#define HAL_TRX_CMD_SR         (0x00) //!< SRAM Read.
#define HAL_TRX_CMD_RADDRM     (0x7F) //!< Register Address Mask.

//// Radio Registers

#define TRX_STATUS                0x01
#define CCA_DONE                     7
#define CCA_STATUS                   6

#define TRX_STATUS_MASK           0x1F
#define TRX_P_ON                  0x00
#define TRX_BUSY_RX               0x01
#define TRX_BUSY_TX               0x02
#define TRX_RX_ON                 0x06
#define TRX_TRX_OFF               0x08
#define TRX_PLL_ON                0x09
#define SLEEP_REG                 0x0F
#define BUSY_RX_AACK              0x11
#define BUSY_TX_ARET              0x12
#define RX_AACK_ON                0x16
#define TX_ARET_ON                0x19
#define RX_ON_NOCLK               0x1C
#define RX_AACK_ON_NOCLK          0x1D
#define BUSY_RX_AACK_NOCLK        0x1E
#define TRANSITION_IN_PROGRESS    0x1F

#define TRX_STATE                 0x02
#define TRX_CMD_MASK              0x1F
#define TRAC_STATUS                  5

#define TRX_CMD                      0
// TRX_CMD modes
#define CMD_NOP                      0
#define CMD_TX_START                 2
#define CMD_FORCE_TRX_OFF            3
#define CMD_RX_ON                    6
#define CMD_TRX_OFF                  8
#define CMD_PLL_ON                   9
#define CMD_RX_AACK_ON              22
#define CMD_TX_ARET_ON              25

#define TRX_CTRL_0                0x03
#define TRX_CTRL_1                0x04


#define TRX_CTRL_0                0x03
#define PAD_IO                       6
#define PAD_IO_CLKM                  4
#define CLKM_SHA_SEL                 3

// PAD_IO_CLKM Modes
#define CLKM_2mA                     0
#define CLKM_4mA                     1
#define CLKM_6mA                     2
#define CLKM_8mA                     3

#define CLKM_CTRL                    0
// CLK_M MOdes
#define CLKM_no_clock                0
#define CLKM_1MHz                    1
#define CLKM_2MHz                    2
#define CLKM_4MHz                    3 
#define CLKM_8MHz                    4
#define CLKM_16MHz                   5


#define TRX_CTRL_1                0x04
#define TX_AUTO_CRC_ON               5

#define PHY_TX_PWR                0x05
#define TX_PWR                       0

#define PHY_RSSI                  0x06
#define RX_CRC_VALID                 7
#define RSSI_MASK                 0x1f
#define RSSI                         0
#define RND_VALUE                    5

#define PHY_ED_LEVEL              0x07
#define PHY_CC_CCA                0x08
#define CCA_REQUEST                  7
#define CCA_MODE                     5
#define CHANNEL_MASK              0x1f
#define CHANNEL                      0

#define CCA_THRES                 0x09
#define CCA_CS_THRES                 4
#define CCA_ED_THRES                 0

#define RX_CTRL                   0x0A
#define SFD_VALUE                 0x0B

#define TRX_CTRL_2                0x0c
#define RX_SAFE_MODE                 7
#define OQPSK_DATA_RATE              0

#define IRQ_MASK                  0x0E
#define IRQ_STATUS                0x0F
#define BAT_LOW                      7
#define TRX_UR                       6
#define AMI                          5
#define CCA_ED_DONE                  4
#define TRX_END                      3
#define TRX_START                    2
#define PLL_UNLOCK                   1
#define PLL_LOCK                     0

#define VREG_CTRL                 0x10
#define AVREG_EXT                    7
#define AVDD_OK                      6
#define AVREG_TRIM                   4
#define DVREG_EXT                    3
#define DVDD_OK                      2

#define DVREG_TRIM                   0
// DVREG_TRIM MOdes


#define BATMON                    0x11
#define BATMON_OK                    5
#define BATMON_HR                    4
#define BATMON_VTH                   0

#define XOSC_CTRL                 0x12
#define XTAL_MODE                    4
#define XTAL_TRIM                    0

#define RX_SYN                    0x15
#define RX_PDT_MASK               0x0F 
#define RX_PDT_LEVEL                 0

#define XAH_CTRL_1                0x17
#define AACK_ACK_TIME                2
#define AACK_UPLD_RES_FT             4
#define AACK_UPLD_FLTR_FT            5

#define FTN_CTRL                  0x18
#define FTN_START                    7
#define FTNV                         0

#define PLL_CF                    0x1a
#define PLL_CF_START                 7

#define PLL_DCU                   0x1b
#define PLL_DCU_START                7
#define PLL_DCUW                     0

#define PART_NUM               0x1c
#define VERSION_NUM            0x1d
#define MAN_ID_0               0x1e
#define MAN_ID_1               0x1f

#define SHORT_ADDR_0           0x20
#define SHORT_ADDR_1           0x21

#define PAN_ID_0               0x22
#define PAN_ID_1               0x23

#define IEEE_ADDR_0            0x24
#define IEEE_ADDR_1            0x25
#define IEEE_ADDR_2            0x26
#define IEEE_ADDR_3            0x27
#define IEEE_ADDR_4            0x28
#define IEEE_ADDR_5            0x29
#define IEEE_ADDR_6            0x2a
#define IEEE_ADDR_7            0x2b

#define XAH_CTRL_0             0x2c
#define MAX_FRAME_RETRIES        4
#define MAX_CSMA_RETRIES         1

#define CSMA_SEED_0            0x2D
#define CSMA_SEED_1            0x2E
#define I_AM_COORD                3
#define AACK_DIS_ACK              4
#define AACK_FVN_MODE             6

#define CSMA_BE                0x2f
#define MIN_BE                    0
#define MAX_BE                    4

#define RF230_PN                  2
#define RF231_PN                  3
#define RF212_PN                  7

#endif // RF231_H_ 
