# skrypt z chata gpt
from typing import Tuple, Optional
import numpy as np


def _next_pow2_plus_one(n: int) -> int:
    """Return the next size of form (2^k)+1 >= n."""
    k = 0
    while (1 << k) + 1 < n:
        k += 1
    return (1 << k) + 1


def _diamond_square(size: int, roughness: float, seed: Optional[int] = None) -> np.ndarray:
    """
    Diamond-square algorithm on a (size x size) grid where size == 2^k + 1.
    roughness controls the amplitude decay of the random offset (typical range 0.3..1.2).
    """
    if seed is not None:
        rng = np.random.RandomState(seed)
    else:
        rng = np.random.RandomState()

    # Start with zeros
    grid = np.zeros((size, size), dtype=float)

    # Initialize corners with small random values
    grid[0, 0] = rng.rand()
    grid[0, -1] = rng.rand()
    grid[-1, 0] = rng.rand()
    grid[-1, -1] = rng.rand()

    step_size = size - 1
    scale = 1.0

    while step_size > 1:
        half = step_size // 2

        # Diamond step
        for x in range(0, size - 1, step_size):
            for y in range(0, size - 1, step_size):
                a = grid[x, y]
                b = grid[x + step_size, y]
                c = grid[x, y + step_size]
                d = grid[x + step_size, y + step_size]
                avg = (a + b + c + d) / 4.0
                offset = (rng.rand() - 0.5) * 2 * scale
                grid[x + half, y + half] = avg + offset

        # Square step
        for x in range(0, size, half):
            for y in range((x + half) % step_size, size, step_size):
                s = []
                if x - half >= 0:
                    s.append(grid[x - half, y])
                if x + half < size:
                    s.append(grid[x + half, y])
                if y - half >= 0:
                    s.append(grid[x, y - half])
                if y + half < size:
                    s.append(grid[x, y + half])
                avg = np.mean(s)
                offset = (rng.rand() - 0.5) * 2 * scale
                grid[x, y] = avg + offset

        # reduce step
        step_size //= 2
        scale *= roughness

    return grid


def _gaussian_smoothing(grid: np.ndarray, sigma: float) -> np.ndarray:
    """
    Apply a separable Gaussian blur to `grid` with standard deviation `sigma`.
    Implemented via convolution with a small kernel created from sigma.
    If sigma <= 0, returns the original grid (no smoothing).
    """
    if sigma <= 0:
        return grid

    # Kernel radius: cover at least 3 sigma on each side
    radius = max(1, int(3 * sigma))
    x = np.arange(-radius, radius + 1)
    kernel = np.exp(-(x ** 2) / (2 * sigma * sigma))
    kernel = kernel / kernel.sum()

    # Convolve rows then columns (separable)
    # pad with reflect to avoid boundary artifacts
    temp = np.apply_along_axis(lambda m: np.convolve(m, kernel, mode='same'), axis=1, arr=grid)
    smoothed = np.apply_along_axis(lambda m: np.convolve(m, kernel, mode='same'), axis=0, arr=temp)
    return smoothed

def generate_terrain(n: int,
                     m: int,
                     roughness: float = 0.7,
                     seed: Optional[int] = None,
                     smooth_sigma: float = 0.0,
                     normalize: bool = True) -> np.ndarray:
    """
    Generate an n x m terrain heightmap.

    Parameters
    ----------
    n, m : int
        Desired output size (n x m).
    roughness : float
        Controls how rough the terrain is. Lower -> smoother large-scale features.
    seed : Optional[int]
        RNG seed for reproducibility.
    smooth_sigma : float
        Optional Gaussian smoothing sigma in grid units.
    normalize : bool
        If True, the returned grid will be normalized to [0, 1].
    """

    if n <= 0 or m <= 0:
        raise ValueError("n and m must be positive")

    # Diamond-square needs a square (2^k + 1) grid; pick size big enough for both dims
    size = _next_pow2_plus_one(max(n, m))

    grid = _diamond_square(size, roughness, seed=seed)

    if smooth_sigma > 0:
        grid = _gaussian_smoothing(grid, smooth_sigma)

    # Crop to n × m
    cropped = grid[:n, :m]

    if normalize:
        minv = cropped.min()
        maxv = cropped.max()
        if maxv > minv:
            cropped = (cropped - minv) / (maxv - minv)
        else:
            cropped[:] = 0.0

    return cropped


def flatten(heightmap, road_mark):
    """
    Flatten the terrain heightmap where the road_mark indicates road cells.
    road_mark: 2D array of same size as heightmap, with 1 for road cells and 0 for non-road.
    """
    flattened = heightmap.copy()
    n, m = heightmap.shape
    print(n)
    print(m)
    for i in range(n):
        for j in range(m):
            if road_mark[i][j] == 1:
                flattened[i][j] = 0.5  # Set road cells to height 0
    return flattened

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

def flatten_own_function(heightmap, road_mark):
    flattened = heightmap.copy()
    n, m = heightmap.shape


    for i in range(n):
        for j in range(m):
            if road_mark[i][j] == 1:
                if(is_neightbor_terrain(road_mark, i, j)):
                    road_mark[i][j] = 2  # mark edge cells
    mark_left_edge(road_mark)
    
    # road_mark = np.array(road_mark, dtype=int)
    # np.savetxt("road_mark_edges.txt", road_mark, fmt='%d')
    
    
    for i in range(n):
        for j in range(m):
            if road_mark[i][j] == 1:
                closest_i, closest_j = found_closest_edge(road_mark, i, j)
                if closest_i != -1 and closest_j != -1:
                    flattened[i][j] = flattened[closest_i][closest_j]
    
    for i in range(n):
        for j in range(m):
            if road_mark[i][j] >= 1:
                road_mark[i][j] = 1  # reset to road cells
    
    return flattened

def found_closest_edge(road_mark, i, j):
    n = len(road_mark)
    m = len(road_mark[0])
    min_dist = float('inf')
    closest_i = -1
    closest_j = -1
    
    
    distante_limit = 20
    for di in range(-distante_limit, distante_limit + 1):
        for dj in range(-distante_limit, distante_limit + 1):
            ni = i + di
            nj = j + dj
            if 0 <= ni < n and 0 <= nj < m:
                if road_mark[ni][nj] == 3:
                    dist = abs(di) + abs(dj)
                    if dist < min_dist:
                        min_dist = dist
                        closest_i = ni
                        closest_j = nj
    return closest_i, closest_j

def mark_left_edge(road_mark):
    n = len(road_mark)
    m = len(road_mark[0])
    
    start_i =0
    start_j =0
    
    brak_loop = False
    for i in range(n):
        for j in range(m):
            if road_mark[i][j] == 2:
                start_i = i
                start_j = j
                brak_loop = True
                break
        if brak_loop:
            break
    
    edges_found=[]
    edges_found.append((start_i, start_j))
    while len(edges_found) > 0:
        i, j = edges_found.pop(0)
        road_mark[i][j] = 3  # mark left edge
        for di in [-1, 0, 1]:
            for dj in [-1, 0, 1]:
                
                ni = i + di
                nj = j + dj
                if 0 <= ni < n and 0 <= nj < m:
                    if road_mark[ni][nj] == 2:
                        road_mark[ni][nj] = 3  # mark left edge
                        edges_found.append((ni, nj))
    
    return road_mark

import numpy as np

def flatten_heightmap(heightmap, road_mark, radius=3):
    # Convert lists to numpy arrays if needed
    heightmap = np.array(heightmap, dtype=float)
    road_mark = np.array(road_mark, dtype=int)

    road_mark2 = road_mark.copy()
    flattened = heightmap.copy()
    n, m = flattened.shape

    for i in range(n):
        for j in range(m):
            if road_mark[i, j] == 1:

                # ------ 1) Compute average height in radius ------
                total = 0.0
                count = 0

                for di in range(-radius, radius + 1):
                    for dj in range(-radius, radius + 1):
                        ni = i + di
                        nj = j + dj
                        if 0 <= ni < n and 0 <= nj < m:
                            total += flattened[ni, nj]
                            count += 1

                if count == 0:
                    continue

                avg_height = total / count

                # ------ 2) Apply the averaged height ------
                for di in range(-radius, radius + 1):
                    for dj in range(-radius, radius + 1):
                        ni = i + di
                        nj = j + dj
                        if 0 <= ni < n and 0 <= nj < m:
                            if(road_mark[ni, nj] == 1):
                                flattened[ni, nj] = avg_height
                                road_mark[ni, nj] = 0  # mark processed
    
    #this does not work
    for num in range(10):  
        for i in range(n):
            for j in range(m):
                if road_mark2[i, j] == 1:
                    sum = 0.0
                    count = 0
                    if(road_mark2[i-1, j] == 0 and i-1 >= 0):
                        sum += flattened[i-1, j]
                        count += 1
                    if(road_mark2[i+1, j] == 0 and i+1 < n):
                        sum += flattened[i+1, j]
                        count += 1
                    if(road_mark2[i, j-1] == 0 and j-1 >= 0):
                        sum += flattened[i, j-1]
                        count += 1
                    if(road_mark2[i, j+1] == 0 and j+1 < m):
                        sum += flattened[i, j+1]
                        count += 1
                    if count > 0:
                        flattened[i, j] = sum / count
                        
                        
                    
                    
                    
    return flattened

import numpy as np

def smooth_road(heightmap, road_mark, iterations=30):
    heightmap = np.array(heightmap, dtype=float)
    road_mark = np.array(road_mark, dtype=int)

    n, m = heightmap.shape
    smoothed = heightmap.copy()

    for _ in range(iterations):
        new_h = smoothed.copy()

        for i in range(n):
            for j in range(m):
                if road_mark[i, j] == 1:

                    total = 0.0
                    count = 0

                    # 8-neighbor smoothing works best
                    for di in (-1, 0, 1):
                        for dj in (-1, 0, 1):
                            if di == 0 and dj == 0:
                                continue
                            ni = i + di
                            nj = j + dj
                            if 0 <= ni < n and 0 <= nj < m:
                                total += smoothed[ni, nj]
                                count += 1

                    if count > 0:
                        new_h[i, j] = total / count   # average of surrounding terrain

        smoothed = new_h

    return smoothed




# Small convenience CLI when run directly
def main2():
    import argparse
    try:
        import matplotlib.pyplot as plt
    except Exception:
        plt = None

    parser = argparse.ArgumentParser(description='Generate an n x n terrain heightmap (diamond-square).')
    parser.add_argument('n', type=int, help='size (n) of the output grid (n x n)')
    parser.add_argument('--roughness', type=float, default=0.7, help='roughness (0.3..1.2 default 0.7)')
    parser.add_argument('--seed', type=int, default=None, help='random seed (int)')
    parser.add_argument('--smooth', type=float, default=0.0, help='gaussian smoothing sigma (0 = none)')
    parser.add_argument('--outfile', type=str, default=None, help='save heights as plain text (.txt)')
    parser.add_argument('--show', action='store_true', help='show a quick matplotlib image (requires matplotlib)')

    args = parser.parse_args()
    h = generate_terrain(args.n, roughness=args.roughness, seed=args.seed, smooth_sigma=args.smooth)

    if args.outfile:
        # Save as plain text
        np.savetxt(args.outfile, h, fmt='%.6f')
        print(f'Saved heightmap as plain text to {args.outfile}')(f'Saved heightmap to {args.outfile}')

    if args.show:
        if plt is None:
            print('matplotlib not available; cannot show image.')
        else:
            plt.figure(figsize=(6, 6))
            plt.imshow(h, cmap='terrain', origin='lower')
            plt.colorbar(label='height')
            plt.title(f'Terrain {args.n}x{args.n} (roughness={args.roughness}, smooth={args.smooth})')
            plt.show()

def read_terrain_from_file(filename: str) -> np.ndarray:
    """
    Read a terrain heightmap from a plain text file.
    Each line in the file should contain space-separated float values.
    """
    return np.loadtxt(filename)

if __name__ == '__main__':
    
    from road_mark import generate_track
    from texture import generate_texture
    from image_to_tarain import image_to_array
    
    n=500
    m=600
    #road_mark = generate_track(n, m, road_width=15)
    road_mark = image_to_array("race_track_shape.png")
    #generate_texture(road_mark)
    #h = generate_terrain(n,m, roughness=0.3, seed=42, smooth_sigma=0.1)
    h = read_terrain_from_file("heightmap_normalized.txt")
    #h = flatten(h, road_mark)
    h = flatten_own_function(h, road_mark)
    #h= smooth_road(h, road_mark, iterations=30)
    
    road_mark = np.array(road_mark, dtype=int)
    np.savetxt("road_mark.txt", road_mark, fmt='%d')
    np.savetxt("terrain.txt", h, fmt='%.6f')
    
    print(f'Saved heightmap as plain text to terrain.txt')