import shutil
import os

def replace_files(source_folder, destination_folder, files_to_replace):
    os.makedirs(destination_folder, exist_ok=True)

    for file_name in files_to_replace:
        source_file = os.path.join(source_folder, file_name)
        destination_file = os.path.join(destination_folder, file_name)

        if not os.path.exists(source_file):
            print(f"[ERROR] Source file does not exist: {source_file}")
            return
        
        try:
            # Delete destination file if it exists
            if os.path.exists(destination_file):
                os.remove(destination_file)
                print(f"[DELETE] Removed old: {destination_file}")

            # Copy the new file
            shutil.copy2(source_file, destination_file)
            print(f"[COPY] Replaced with new: {destination_file}")

        except Exception as e:
            print(f"[ERROR] Failed to replace {file_name}: {e}")

# Example usage
source_folder = "C:/Users/pietr/Desktop/Inzynierka/CarRace/scripts"
destination_folder = "C:/Users/pietr/Desktop/Inzynierka/CarRace/assets/terrain"
files_to_replace = ["terrain.txt", "road_mark.txt"]

replace_files(source_folder, destination_folder, files_to_replace)