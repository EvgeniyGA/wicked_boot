def test_version(board):
    match = board.wait_for_regex_in_file(r"Firmware version:")
    assert match is not None, "Ожидалось 'Firmware version:' в выводе"