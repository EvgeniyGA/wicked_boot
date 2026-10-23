def test_formatting_successful(board):
    match = board.wait_for_regex_in_file(r"Bootloader Version")
    assert match is not None, "Ожидалось 'Bootloader Version' в выводе"

def test_mounted_successful(board):
    match = board.wait_for_regex_in_file(r"Swap type: none")
    assert match is not None, "Сообщение Swap type: none не найдено"






