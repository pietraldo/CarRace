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

def read_image(filename):
    """
    Read an image from a file and return the Image object.
    """
    img = Image.open(filename)
    return img    

def generate_texture(road_mark):
    height = len(road_mark)         # number of rows
    width = len(road_mark[0])       # number of columns

    img = create_image(width, height, default_color=(50, 250, 50))
    img_asphalt = read_image("./asphalt_texture.jpg")
    img_grass = read_image("./grass_texture.png")

    for y in range(height):         # row index
        for x in range(width):      # column index
            if road_mark[y][x] == 1:
                color = img_asphalt.getpixel((x % img_asphalt.width, y % img_asphalt.height))
                set_pixel(img, x, y, color)
                # if(x-2 >=0 and road_mark[y][x-2] == 0):
                #     set_pixel(img, x-2, y, (255, 255, 255))
                # if(x+2 < width and road_mark[y][x+2] == 0):
                #     set_pixel(img, x+2, y, (255, 255, 255))
                # if(y-2 >=0 and road_mark[y-2][x] == 0):
                #     set_pixel(img, x, y-2, (255, 255, 255))
                # if(y+2 < height and road_mark[y+2][x] == 0):
                #     set_pixel(img, x, y+2, (255, 255, 255))
            else:
                color = img_grass.getpixel((x % img_grass.width, y % img_grass.height))
                set_pixel(img, x, y, color)
    save_image(img, "road_texture.png")