#!/usr/bin/env python3

from bs4 import BeautifulSoup
import requests
import re


addressing_modes = {
    "imp": "implicit",
    "imm": "immediate_addressing",
    "zp": "zero_page_addressing",
    "zpx": "zero_page_x_indexed",
    "zpy": "zero_page_y_indexed",
    "izx": "indexed_indirect",
    "izy": "indirect_indexed",
    "abs": "absolute_addressing",
    "abx": "absolute_x_indexed",
    "aby": "absolute_y_indexed",
    "ind": "indirect_addressing",
    "rel": "relative_addressing",
}


def fetch_instruction_set():
    """Function to fetch opcodes from website and save in instruction_set.html file."""
    response = requests.get("http://www.oxyron.de/html/opcodes02.html")
    with open("instruction_set.html", "w") as f:
        f.write(response.text)


def get_init(instruction):
    """Function returns struct initalizer for perticular instruction."""

    name = instruction[0]
    addressing = "imp"
    if len(instruction) == 3:
        addressing = instruction[1]

    addressing = addressing_modes[addressing]
    init = "{" + f'"{name}", &Cpu::{name}, &Cpu::{addressing}' + "}"
    return init


def main():
    # fetch_instruction_set()
    with open("instruction_set.html", "r") as f:
        page = BeautifulSoup(f.read(), "lxml")

    table = page.find("table")
    entries = 0
    rows = table.find_all("tr")[1:]

    instructions = []
    for row in rows:
        cols = row.find_all("td")[1:]
        for cell in cols:
            instructions.append(cell.font.decode_contents())

    tags = re.compile(r"</?[\w]+/?>")
    instructions = map(lambda x: tags.sub(" ", x).strip().split(" "), instructions)
    instructions = map(get_init, instructions)

    with open("instruction_mapping.txt", "w") as f:
        f.write(",\n".join(instructions))


if __name__ == "__main__":
    main()
