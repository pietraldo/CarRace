import math

def generate_track(n, m, road_width=7):
    """
    Generates a 2D array (n x m) containing a racing loop.
    Road cells = 1
    Non-road = 0
    """
    grid = [[0 for _ in range(m)] for _ in range(n)]

    # center of the track
    cx, cy = n / 2, m / 2

    rx = n * 0.35
    ry = m * 0.35

    for i in range(n):
        for j in range(m):
            # distance from cell to center relative to the oval shape
            dx = (i - cx) / rx
            dy = (j - cy) / ry
            dist = math.sqrt(dx*dx + dy*dy)

            # dist ? 1 means the point is on the loop
            # so we mark a band of width = road_width
            if 1 - (road_width / min(n, m)) < dist < 1 + (road_width / min(n, m)):
                grid[i][j] = 1

    return grid


