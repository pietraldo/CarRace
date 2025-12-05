import matplotlib.pyplot as plt
import numpy as np
from scipy.ndimage import gaussian_filter1d


def get_heights(road_mark, flattened, edge_number):
    n = len(road_mark)
    m = len(road_mark[0])
    
    road_mark_copy = [row[:] for row in road_mark]
    heights=[]
    
    start_i =0
    start_j =0
    
    brak_loop = False
    for i in range(n):
        for j in range(m):
            if road_mark[i][j] == edge_number:
                start_i = i
                start_j = j
                brak_loop = True
                break
        if brak_loop:
            break

    indexes = []
    i=start_i
    j=start_j 
    found_next = True
    while found_next:
        
        road_mark[i][j] = 1
        indexes.append((i,j))  
        heights.append(flattened[i][j])
        found_next = False
        
        for di in [-1, 0, 1]:
            for dj in [-1, 0, 1]:
                
                ni = i + di
                nj = j + dj
                if 0 <= ni < n and 0 <= nj < m:
                    if road_mark[ni][nj] == edge_number:
                        i = ni
                        j = nj
                        found_next = True
                        break
            if found_next:
                break
    return (heights, indexes)

def smooth_heights(heights, sigma=50):
    return gaussian_filter1d(heights, sigma=sigma)

def apply_smooth_heights(road_mark, flattened):
    (heights, indexes) = get_heights(road_mark, flattened,3)
    smoothed_heights = smooth_heights(heights, sigma=50)
    
    plot_heights(smoothed_heights)
    
    for idx, (i, j) in enumerate(indexes):
        flattened[i][j] = smoothed_heights[idx]
        road_mark[i][j] = 4  
    
    (heights, indexes) = get_heights(road_mark, flattened,2)
    smoothed_heights = smooth_heights(heights, sigma=50)
    for idx, (i, j) in enumerate(indexes):
        flattened[i][j] = smoothed_heights[idx]
        road_mark[i][j] = 5  
    
    return flattened

import matplotlib.pyplot as plt

def plot_heights(heights):
    print("heights size:", len(heights))
    
    plt.plot(heights, marker='o', linestyle='')
    plt.title("Height Plot")
    plt.xlabel("Index")
    plt.ylabel("Height")
    plt.show()
