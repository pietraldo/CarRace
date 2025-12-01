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

if __name__ == '__main__':
    
    from road_mark import generate_track
    from texture import generate_texture
    
    n=300
    m=100
    road_mark = generate_track(n, m, road_width=7)
    generate_texture(road_mark)
    h = generate_terrain(n,m, roughness=0.5, seed=42, smooth_sigma=0.4)
    #h = flatten(h, road_mark)
    
    np.savetxt("terrain.txt", h, fmt='%.6f')
    print(f'Saved heightmap as plain text to terrain.txt')