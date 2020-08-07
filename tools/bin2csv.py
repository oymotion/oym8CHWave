
#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys


def main(argv):
    if len(argv) != 3:
        print("Usage: python bin2csv.py <data bits> <channel count> <input file name>")
        sys.exit()
    # endif

    data_bits = int(argv[0])

    if (data_bits == 12):
        byte_size = 2
    elif (data_bits == 8):
        byte_size = 1
    else:
        print("data bits should be 8 or 12")
        sys.exit()
    # end if

    channel_cnt = int(argv[1])
    input_file = argv[2]

    file = open(input_file, 'rb')
    file.seek(0, 2)
    file_size = file.tell()
    file.seek(0, 0)

    loop_count = int(file_size / byte_size)

    for i in range(loop_count):
        val = 0
        data_bytes = file.read(byte_size)

        for j in range(len(data_bytes)):
            val = (val << 8) | data_bytes[j]
        # end for

        if ((i % channel_cnt) == channel_cnt - 1):
            if (i == loop_count - 1):
                print("%5d" % val, end='\n')
            else:
                print("%5d," % val, end='\n')
            # end if
        else:
            print("%5d," % val, end='')
        # end if
    # end for

    file.close()
# end main


if __name__ == "__main__":
    main(sys.argv[1:])
