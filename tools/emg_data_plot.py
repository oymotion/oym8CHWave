
#!/usr/bin/python
# -*- coding: UTF-8 -*-

import matplotlib.pyplot as plt
import numpy as np
import struct
import sys


def main(argv):
    if len(argv) != 3:
        print(
            "Usage: python emg_data_plot.py <data bits> <channel count> <input file name>")
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

    sample_count = int(file_size / byte_size / channel_cnt)

    x = np.zeros((channel_cnt, sample_count))

    for i in range(sample_count):
        for j in range(channel_cnt):
            val = 0
            data_bytes = file.read(byte_size)

            for k in  reversed(range(byte_size)):
                val = (val << 8) | data_bytes[k]
            # end for

            x[j, i] = val
            # end if
        # end for
    # end for

    file.close()

    col_count = 2
    row_count = int(channel_cnt / col_count)

    fig, axs = plt.subplots(row_count, col_count)

    for i in range(row_count):
        for j in range(col_count):
            ch_index = i * col_count + j
            # print("i=%d, j=%d, channel=%d" % (i, j, ch_index))
            axs[i, j].plot(x[ch_index, :])
            axs[i, j].set_title('Ch#' + str(ch_index))
        # end for
    # end for

    plt.subplots_adjust(left=None, bottom=None, right=None,
                        top=None, wspace=None, hspace=0.8)
    plt.show()
# end main


if __name__ == "__main__":
    main(sys.argv[1:])
