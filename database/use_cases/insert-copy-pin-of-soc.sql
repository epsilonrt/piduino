INSERT INTO 
		soc_has_pin 
	SELECT 
			6,pin.id
		FROM
		  pin_number
		INNER JOIN  
		  soc_has_pin ON pin_number.pin_id = soc_has_pin.pin_id 
		INNER JOIN  
		  pin ON pin.id = pin_number.pin_id 
		WHERE 
		  soc_id=2 
		  AND 
		  pin.pin_type_id = 0;