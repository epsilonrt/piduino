SELECT connector_has_pin.pin_id,connector_has_pin.row,connector_has_pin.column,pin.pin_type_id,pin_name.name
FROM connector_has_pin
INNER JOIN pin ON connector_has_pin.pin_id = pin.id
INNER JOIN  pin_has_name ON connector_has_pin.pin_id = pin_has_name.pin_id
INNER JOIN  pin_name ON pin_has_name.pin_name_id = pin_name.id
WHERE connector_has_pin.connector_id=19 AND pin_has_name.pin_mode_id=0