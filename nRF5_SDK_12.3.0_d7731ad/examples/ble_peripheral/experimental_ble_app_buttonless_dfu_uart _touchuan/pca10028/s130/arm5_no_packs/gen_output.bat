nrfutil settings generate --family NRF51 --application .\_build\nrf51422_xxac.hex --application-version 1 --bootloader-version 1 --bl-settings-version 1 bootloader_setting.hex
mergehex --merge s130_nrf51_2.0.1_softdevice.hex bootloader.hex .\_build\nrf51422_xxac.hex --output output3to1.hex
mergehex --merge output3to1.hex bootloader_setting.hex --output output.hex
