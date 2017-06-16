define reset
    target remote localhost:2331
    monitor reset
    source ./at91sam9m10-ek-ddram.gdb
    load
end