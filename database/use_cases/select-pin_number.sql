SELECT soc_has_pin.pin_id,pin_name.name,pin.pin_type_id,pin_number.logical_num, soc_has_pin.mcu_num,soc_has_pin.system_num,soc_has_pin.chip_num,soc_has_pin.chip_offset 
FROM soc_has_pin 
INNER JOIN  pin_number ON pin_number.pin_id = soc_has_pin.pin_id 
INNER JOIN  pin ON  soc_has_pin.pin_id = pin.id
INNER JOIN  pin_has_name ON soc_has_pin.pin_id = pin_has_name.pin_id
INNER JOIN  pin_name ON pin_has_name.pin_name_id = pin_name.id
WHERE soc_has_pin.soc_id=6 AND  pin_has_name.pin_mode_id = 0
 