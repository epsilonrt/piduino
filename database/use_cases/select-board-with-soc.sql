SELECT board.id,board.name,board.gpio_id
FROM board
INNER JOIN board_model ON board.board_model_id = board_model.id
WHERE board_model.soc_id = 6