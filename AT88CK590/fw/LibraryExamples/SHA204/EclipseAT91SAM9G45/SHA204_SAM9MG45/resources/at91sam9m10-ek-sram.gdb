#------------------------------------------------
# MCK initialization script for the AT91SAM9M10
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

