#------------------------------------------------
# DDRAM initialization script for the AT91SAM9M10
#------------------------------------------------

echo Configuring the master clock...\n
# Enable main oscillator
# CKGR_MOR
set *0xFFFFFC20 = 0x00004001
# PMC_SR
while ((*0xFFFFFC68 & 0x1) == 0)
end

echo set plla\n
# Set PLLA to 800MHz
# CKGR_PLLAR
set *0xFFFFFC28 = 0x20C73F03
# PMC_SR
while ((*0xFFFFFC68 & 0x2) == 0)
end
while ((*0xFFFFFC68 & 0x8) == 0)
end

echo select prescaler\n
# Select prescaler
# PMC_MCKR
set *0xFFFFFC30 = 0x00001300
# PMC_SR
while ((*0xFFFFFC68 & 0x8) == 0)
end

echo select master clock\n
# Select master clock
# PMC_MCKR
set *0xFFFFFC30 = 0x00001302
# PMC_SR
while ((*0xFFFFFC68 & 0x8) == 0)
end

echo Master clock ok.\n

echo Configuring the DDRAM controller...\n
echo MICRON DDRAM configuration\n
# 0xFFFFE600 DDR2C Base Address

# Enable DDR2 clock x2 in PMC
# AT91C_BASE_PMC, PMC_SCER, AT91C_PMC_DDR
set *0xFFFFFC00 = 0x04

# -----------------------Step 1------------------- 
# Program the memory device type
# ------------------------------------------------ 
    
# HDDRSDRC2_MDR, AT91C_DDRC2_DBW_16_BITS | 16-bit DDR  
set *0xFFFFE620 = 0x16

# -----------------------Step 2------------------- 
# 1. Program the features of DDR2-SDRAM device into 
#    the Configuration Register.
# 2. Program the features of DDR2-SDRAM device into 
#    the Timing Register HDDRSDRC2_T0PR.    
# 3. Program the features of DDR2-SDRAM device into 
#    the Timing Register HDDRSDRC2_T1PR.
# 4. Program the features of DDR2-SDRAM device into 
#    the Timing Register HDDRSDRC2_T2PR.
# ------------------------------------------------ 

# HDDRSDRC2_CR, AT91C_DDRC2_NC_DDR10_SDR9  |     # 10 column bits (1K) 
#          AT91C_DDRC2_NR_14          |          # 14 row bits    (8K) 
#          AT91C_DDRC2_CAS_3          |          # CAS Latency 3
#          AT91C_DDRC2_DLL_RESET_DISABLED        # DLL not reset
set *0xFFFFE608 = 0x3D

# assume timings for 7.5ns min clock period
# HDDRSDRC2_T0PR, AT91C_DDRC2_TRAS_6       |     #  6 * 7.5 = 45   ns
#            AT91C_DDRC2_TRCD_2            |     #  2 * 7.5 = 15   ns
#            AT91C_DDRC2_TWR_2             |     #  2 * 7.5 = 15   ns
#            AT91C_DDRC2_TRC_8             |     #  8 * 7.5 = 60   ns
#            AT91C_DDRC2_TRP_2             |     #  2 * 7.5 = 15   ns
#            AT91C_DDRC2_TRRD_2            |     #  2 * 7.5 = 15  ns
#            AT91C_DDRC2_TWTR_1            |     #  2 clock cycle
#            AT91C_DDRC2_TMRD_2                  #  2 clock cycles
set *0xFFFFE60C = 0x21228226

# pSDDRC->HDDRSDRC2_T1PR = 0x00000008;
# HDDRSDRC2_T1PR, AT91C_DDRC2_TXP_2  |           #  2 * 7.5 = 15 ns
#                 200 << 16          |           # 200 clock cycles, TXSRD: Exit self refresh delay to Read command
#                 16 << 8            |           # 16 * 7.5 = 120 ns TXSNR: Exit self refresh delay to non read command
#                 AT91C_DDRC2_TRFC_14 << 0       # 14 * 7.5 = 105 ns (must be 105 ns for 512M DDR)
set *0xFFFFE610 = 0x02C8100E

# HDDRSDRC2_T2PR, AT91C_DDRC2_TRTP_2   |         #  2 * 7.5 = 15 ns
#                 AT91C_DDRC2_TRPA_0   |         #  0 * 7.5 = 0 ns
#                 AT91C_DDRC2_TXARDS_7 |         #  7 clock cycles
#                 AT91C_DDRC2_TXARD_2            #  2 clock cycles
set *0xFFFFE614 = 0x2072

# -----------------------Step 3------------------- 
# An NOP command is issued to the DDR2-SDRAM to 
# enable clock.
# ------------------------------------------------ 
set *0xFFFFE600 = 0x1
set *0x70000000 = 0

# A minimum pause of 200 us is provided to precede any signal toggle.

set $i = 0
while $i != 13300
  set $i += 1
end

# Now clocks which drive DDR2-SDRAM device are enabled

# -----------------------Step 4------------------- 
# An NOP command is issued to the DDR2-SDRAM 
# ------------------------------------------------ 
set *0xFFFFE600 = 0x1
set *0x70000000 = 0
# Now CKE is driven high.
set $i = 0
while $i != 100
  set $i += 1
end

# -----------------------Step 5------------------- 
# An all banks precharge command is issued to the 
# DDR2-SDRAM.
# ------------------------------------------------ 
# HDDRSDRC2_MR, AT91C_DDRC2_MODE_PRCGALL_CMD  
set *0xFFFFE600 = 0x2
set *0x70000000 = 0
set $i = 0
while $i != 100
  set $i += 1
end

# -----------------------Step 6------------------- 
# An Extended Mode Register set (EMRS2) cycle is 
# issued to chose between commercialor high 
# temperature operations
# ------------------------------------------------ 

# HDDRSDRC2_MR, AT91C_DDRC2_MODE_EXT_LMR_CMD  
set *0xFFFFE600 = 0x5
set *0x74000000 = 0
set $i = 0
while $i != 100
  set $i += 1
end


# -----------------------Step 7------------------- 
# An Extended Mode Register set (EMRS3) cycle is 
# issued to set all registers to 0.
# ------------------------------------------------
# HDDRSDRC2_MR, AT91C_DDRC2_MODE_EXT_LMR_CMD  
set *0xFFFFE600 = 0x5
set *0x76000000 = 0
set $i = 0
while $i != 100
  set $i += 1
end

# -----------------------Step 8------------------- 
# An Extended Mode Register set (EMRS1) cycle is 
# issued to enable DLL.
# ------------------------------------------------
# HDDRSDRC2_MR, AT91C_DDRC2_MODE_EXT_LMR_CMD  
set *0xFFFFE600 = 0x5
set *0x72000000 = 0
# An additional 200 cycles of clock are required for locking DLL
set $i = 0
while $i != 10000
  set $i += 1
end


# -----------------------Step 9------------------- 
# Program DLL field into the Configuration Register.
# -------------------------------------------------

# HDDRSDRC2_CR, cr | AT91C_DDRC2_DLL_RESET_ENABLED
set *0xFFFFE608 |= 0xBD

# -----------------------Step 10------------------- 
# A Mode Register set (MRS) cycle is issued to reset
# DLL.
# -------------------------------------------------
# HDDRSDRC2_MR, AT91C_DDRC2_MODE_LMR_CMD
set *0xFFFFE600 = 0x3
set *0x70000000 = 0
set $i = 0
while $i != 100
  set $i += 1
end

# -----------------------Step 11------------------- 
# An all banks precharge command is issued to the 
# DDR2-SDRAM.
# -------------------------------------------------
# HDDRSDRC2_MR, AT91C_DDRC2_MODE_PRCGALL_CMD
set *0xFFFFE600 = 0x2
set *0x70000000 = 0
set $i = 0
while $i != 100
  set $i += 1
end

# -----------------------Step 12------------------- 
# Two auto-refresh (CBR) cycles are provided. 
# Program the auto refresh command (CBR) into the 
# Mode Register.
# -------------------------------------------------
# HDDRSDRC2_MR, AT91C_DDRC2_MODE_RFSH_CMD
set *0xFFFFE600 = 0x4
set *0x70000000 = 0
set $i = 0
while $i != 100
  set $i += 1
end

# Set 2nd CBR
# HDDRSDRC2_MR, AT91C_DDRC2_MODE_RFSH_CMD
set *0xFFFFE600 = 0x4
set *0x70000000 = 0
set $i = 0
while $i != 100
  set $i += 1
end

# -----------------------Step 13------------------- 
# Program DLL field into the Configuration Register
# to low(Disable DLL reset).
# -------------------------------------------------
# HDDRSDRC2_CR, cr & (~AT91C_DDRC2_DLL_RESET_ENABLED)  
set *0xFFFFE608 &= 0xFFFFFF7F

# -----------------------Step 14------------------- 
# A Mode Register set (MRS) cycle is issued to 
# program the parameters of the DDR2-SDRAM devices
# -------------------------------------------------
# HDDRSDRC2_MR, AT91C_DDRC2_MODE_LMR_CMD
set *0xFFFFE600 = 0x3
set *0x70000000 = 0
set $i = 0
while $i != 100
  set $i += 1
end

# -----------------------Step 15------------------- 
# Program OCD field into the Configuration Register
# to high (OCD calibration default)
# -------------------------------------------------
set *0xFFFFE608  |= (0x07 << 12)

# -----------------------Step 16------------------- 
# An Extended Mode Register set (EMRS1) cycle is 
# issued to OCD default value.
# -------------------------------------------------
# HDDRSDRC2_MR, AT91C_DDRC2_MODE_EXT_LMR_CMD
set *0xFFFFE600 = 0x5
set *0x72000000 = 0
set $i = 0
while $i != 100
  set $i += 1
end

# -----------------------Step 17------------------- 
# Program OCD field into the Configuration Register 
# to low (OCD calibration mode exit).
# -------------------------------------------------
set *0xFFFFE608  &= 0xFFFF8FFF

# -----------------------Step 18------------------- 
# An Extended Mode Register set (EMRS1) cycle is 
# issued to enable OCD exit.
# -------------------------------------------------
# HDDRSDRC2_MR, AT91C_DDRC2_MODE_EXT_LMR_CMD
set *0xFFFFE600 = 0x5
set *0x76000000 = 0
set $i = 0
while $i != 100
  set $i += 1
end

# -----------------------Step 19,20------------------- 
# A mode Normal command is provided. Program the 
# Normal mode into Mode Register.
# -------------------------------------------------
# HDDRSDRC2_MR, AT91C_DDRC2_MODE_NORMAL_CMD
set *0xFFFFE600 = 0x0
set *0x70000000 = 0
set $i = 0
while $i != 100
  set $i += 1
end

# -----------------------Step 21------------------- 
# Write the refresh rate into the count field in the 
# Refresh Timer register. The DDR2-SDRAM device requires a
# refresh every 15.625 us or 7.81 us. With a 100 
# MHz frequency, the refresh timer count register must to 
# be set with (15.625 /100 MHz) = 1562 i.e. 0x061A or 
# (7.81 /100MHz) = 781 i.e. 0x030d
# -------------------------------------------------
# HDDRSDRC2_RTR, 0x00000411
set *0xFFFFE604 = 0x00000411

# Read optimization" shall be un-selected on this revision.
set *0xFFFFE62C = 0x04

# OK now we are ready to work on the DDRSDR


echo DDRAM configuration ok.\n

