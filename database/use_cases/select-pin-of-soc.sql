SELECT
    pin_number.pin_id AS item_id,
    pin_type_id,
    system_num,
    chip_num,
    chip_offset
  FROM
    pin_number
  INNER JOIN  
    soc_has_pin ON pin_number.pin_id = soc_has_pin.pin_id 
  INNER JOIN  
    pin ON pin.id = pin_number.pin_id 
  WHERE 
    soc_id=6 
    AND 
    pin.pin_type_id = 0
  ORDER BY 
    item_id;