
arm-elf-objcopy -O binary Station_Meteo Station_Meteo.bin
tftp -i 134.158.18.140 put Station_Meteo.bin
