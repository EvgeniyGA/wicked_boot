def test_version(board):
    match = board.wait_for_regex_in_file(r"Bootloader Version:")
    assert match is not None, "Ожидалось 'Bootloader Version:' в выводе"