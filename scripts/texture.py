from PIL import Image

def create_image(width, height, default_color=(0, 0, 0)):
    """
    Create a new image of size width x height.
    default_color is an RGB tuple, e.g. (255, 0, 0) for red.
    """
    img = Image.new("RGB", (width, height), default_color)
    return img

def set_pixel(img, x, y, color):
    """
    Set the pixel at (x, y) to the given RGB color.
    """
    img.putpixel((x, y), color)

def save_image(img, filename):
    """
    Save the image to a file.
    """
    img.save(filename)
    
def generate_texture(road_mark):
    height = len(road_mark)         # number of rows
    width = len(road_mark[0])       # number of columns

    img = create_image(width, height, default_color=(50, 250, 50))

    for y in range(height):         # row index
        for x in range(width):      # column index
            if road_mark[y][x] == 1:
                set_pixel(img, x, y, (40, 40, 40))

    save_image(img, "road_texture.png")