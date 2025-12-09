import matplotlib.pyplot as plt
import numpy as np
from scipy.ndimage import gaussian_filter1d
from scipy.ndimage import uniform_filter


def is_neightbor_terrain(road_mark, i, j):
    n = len(road_mark)
    m = len(road_mark[0])
    for di in [-1, 0, 1]:
        for dj in [-1, 0, 1]:
            if di == 0 and dj == 0:
                continue
            ni = i + di
            nj = j + dj
            if 0 <= ni < n and 0 <= nj < m:
                if road_mark[ni][nj] == 0:
                    return True
    return False

def found_closest_edge(road_mark, i, j, number_edge):
    n = len(road_mark)
    m = len(road_mark[0])
    min_dist = float('inf')
    closest_i = -1
    closest_j = -1
    
    
    distante_limit = 10
    for di in range(-distante_limit, distante_limit + 1):
        for dj in range(-distante_limit, distante_limit + 1):
            ni = i + di
            nj = j + dj
            if 0 <= ni < n and 0 <= nj < m:
                if road_mark[ni][nj] == number_edge:
                    dist = abs(di) + abs(dj)
                    if dist < min_dist:
                        min_dist = dist
                        closest_i = ni
                        closest_j = nj
    return closest_i, closest_j, min_dist

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
    smoothed = uniform_filter(heights, size=sigma)
    return smoothed
    #return gaussian_filter1d(heights, sigma=sigma)

def apply_smooth_heights(road_mark, heights, sigma=5):
    road_mark = np.array(road_mark)
    heights = np.array(heights)
    
    heights_copy = heights.copy()
    n,m  = heights.shape
    for i in range(n):
        for j in range(m):
            if road_mark[i][j] == 0:
                ni, nj, dist = found_closest_edge(road_mark, i, j, 1)
                heights_copy[i][j] = heights[ni][nj]
                
    
    smoothed_heights = smooth_heights(heights_copy, sigma=sigma)
    smoothed_heights = smooth_heights(smoothed_heights, sigma=sigma)

    # copy original terrain
    result = heights.copy()

    # replace only road cells with smoothed values
    result[road_mark >= 1] = smoothed_heights[road_mark >= 1]
    #result = smoothed_heights

    return result


import matplotlib.pyplot as plt

def plot_heights(heights):
    print("heights size:", len(heights))
    
    plt.plot(heights, marker='o', linestyle='')
    plt.title("Height Plot")
    plt.xlabel("Index")
    plt.ylabel("Height")
    plt.show()
