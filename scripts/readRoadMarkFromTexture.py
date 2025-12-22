from PIL import Image
import numpy as np

def readRoadMarkFromTexture(texture_path, n, m):
   
    road_mark = np.zeros((n, m)) 
    
    roadColors = {
        "road": (74, 69, 65),
        "line": (255, 255, 255),
        "grass": (30, 204, 0)
    }
   
    img = Image.open(texture_path)
    
    scale_x = img.width // n
    scale_y = img.height // m
    
    
    for i in range(n):
        for j in range(m):
            
            is_road = False
            is_grass = False
            road_mark[i, j] = 0
            for dx in range(scale_x):
                for dy in range(scale_y):
                    pixel_y = (i * scale_x + dx)
                    pixel_x = (j * scale_y + dy)
                    
                    if(pixel_x >= img.width or pixel_y >= img.height):
                        continue
                    
                    pixel = img.getpixel((pixel_x, pixel_y))[:3]
                    
                    if pixel == roadColors["road"]:
                        road_mark[i, j] = 1
                        is_road = True
                        
                    elif pixel == roadColors["line"]:
                        road_mark[i, j] = 1
                        is_road = True
                        
                    elif pixel == roadColors["grass"] and not is_road:
                        road_mark[i, j] = 5
                        is_grass = True
                        
                

    return road_mark

def show_road_mark(road_mark):
    import matplotlib.pyplot as plt
    plt.imshow(road_mark, cmap='gray')
    plt.show()