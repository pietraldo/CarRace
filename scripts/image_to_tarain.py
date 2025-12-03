from PIL import Image

def image_to_array(path):
    img = Image.open(path).convert("RGB")   # ensure RGB
    width, height = img.size

    arr = []

    for y in range(height):
        row = []
        for x in range(width):
            r, g, b = img.getpixel((x, y))
            # white pixel check (you can adjust threshold if needed)
            if (r, g, b) == (255, 255, 255):
                row.append(0)
            else:
                row.append(1)
        arr.append(row)

    return arr

