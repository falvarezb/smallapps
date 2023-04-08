import numpy as np


def create_random_file(file_name: str, file_size: int, buffer_size: int):
    """Creates a binary file of arbitrary size with random content

    To improve performance, a buffer is used to write data in larger chunks

    Args:
        file_name (str): file name (relative or full path)
        file_size (int): file size in bytes
        buffer_size (int): buffer size in bytes
    """
    with open(file_name, "wb") as f:
        remaining_bytes = file_size
        while remaining_bytes > 0:
            num_bytes_to_write = min([buffer_size, remaining_bytes])
            f.write(np.random.bytes(num_bytes_to_write))
            remaining_bytes -= num_bytes_to_write


if __name__ == "__main__":
    create_random_file("filetest", 32, 5)
