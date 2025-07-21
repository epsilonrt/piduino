UPDATE pin_number
SET
  chip_num = 0,
  chip_offset = selected_items.system_num - 352
FROM
  (
    SELECT
      pin_number.pin_id AS item_id,
      system_num
    FROM
      pin_number
    INNER JOIN  
      soc_has_pin ON pin_number.pin_id = soc_has_pin.pin_id 
    INNER JOIN  
      pin ON pin.id = pin_number.pin_id 
    WHERE 
      soc_id=3
      AND 
      pin.pin_type_id = 0
      AND
      system_num >= 352
  ) AS selected_items
WHERE
  pin_number.pin_id  = selected_items.item_id;