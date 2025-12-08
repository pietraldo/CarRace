from PIL import Image
import numpy as np

# Load the 16-bit heightmap
img = Image.open("mapk2.tif")

# Read as numpy array (preserves 16-bit values)
arr = np.array(img, dtype=np.uint16)

arr_norm = arr.astype(np.float32) / 65535.0

# Save to text file
with open("heightmap_normalized.txt", "w") as f:
    for row in arr_norm:
        row_str = " ".join(f"{v:.6f}" for v in row)  # 6 decimal places
        f.write(row_str + "\n")
