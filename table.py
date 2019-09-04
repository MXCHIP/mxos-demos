"""Simple example usage of terminaltables and column_max_width().
Just prints sample text and exits.
"""

from __future__ import print_function

from textwrap import wrap

from terminaltables import SingleTable


def main():
    """Main function."""
    table_data = [
        ['NAME', 'ADDRESS', "LENGTH"],
        ['BOOTLOADER', '0x10000000', "0x5000"],
        ['INFO', '0x10005000', "0x1000"],
        ['KV', '0x10006000', "0x4000"],
        ['APPLICATION', '0x1000A000', "0xF6000"],
        ['OTA_TEMP', '0x10100000', "0xA4000"],
    ]
    table = SingleTable(table_data)

    print(table.table)


if __name__ == '__main__':
    main()
