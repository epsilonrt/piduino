SELECT pin_number.logical_num,soc_has_pin.mcu_num,soc_has_pin.system_num,soc_has_pin.chip_num,soc_has_pin.chip_offset 
FROM soc_has_pin 
INNER JOIN  pin_number ON pin_number.pin_id = soc_has_pin.pin_id 
WHERE soc_has_pin.soc_id=3 AND soc_has_pin.pin_id=44