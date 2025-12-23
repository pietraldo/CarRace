from PIL import Image
import numpy as np
from math import ceil
from math import floor

import numpy as np
from PIL import Image
from math import ceil, floor

def readRoadMarkFromTexture(texture_path, n, m):

    road_mark = np.zeros((n, m))

    roadColors = {
        "road": (74, 69, 65),
        "line": (255, 255, 255),
        "grass": (30, 204, 0)
    }

    img = Image.open(texture_path).convert("RGB")

    scale_x = img.width / (m - 1)
    scale_y = img.height / (n - 1)

    array_count = [0] * img.width

    for i in range(m):       # x cell
        for j in range(n):   # y cell

            low_x = ceil(scale_x * i)
            high_x = floor(scale_x * (i + 1))

            low_y = ceil(scale_y * j)
            high_y = floor(scale_y * (j + 1))

            is_road = False

            for dy in range(low_y, high_y):
                for dx in range(low_x, high_x):

                    if dx >= img.width or dy >= img.height:
                        continue

                    array_count[dx] += 1

                    pixel = img.getpixel((dx, dy))

                    if pixel == roadColors["road"] or pixel == roadColors["line"]:
                        road_mark[j, i] = 1
                        is_road = True

                    elif pixel == roadColors["grass"] and not is_road:
                        road_mark[j, i] = 5

    

    return road_mark


def show_road_mark(road_mark):
    import matplotlib.pyplot as plt
    plt.imshow(road_mark, cmap='gray')
    plt.show()