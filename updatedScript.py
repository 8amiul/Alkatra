import os
import shutil
import re
from mutagen.easyid3 import EasyID3
from mutagen.mp3 import MP3

INPUT_DIR = "NewMusic"          # folder containing NEW music only
OUTPUT_DIR = "DFPLAYER_SD"
HEADER_FILE = "songs.h"

os.makedirs(OUTPUT_DIR, exist_ok=True)

header_path = os.path.join(OUTPUT_DIR, HEADER_FILE)

songs = []
last_folder = 0

# ---------------------------
# Load existing songs if any
# ---------------------------

if os.path.exists(header_path):

    with open(header_path, "r", encoding="utf-8") as f:
        content = f.read()

    pattern = r'\{\s*(\d+),\s*(\d+),\s*"([^"]*)",\s*"([^"]*)",\s*"([^"]*)",\s*(\d+)\s*\}'
    matches = re.findall(pattern, content)

    for m in matches:
        songs.append({
            "folder": int(m[0]),
            "track": int(m[1]),
            "title": m[2],
            "artist": m[3],
            "album": m[4],
            "length": int(m[5])
        })

    if songs:
        last_folder = max(s["folder"] for s in songs)

folder_index = last_folder + 1


# ---------------------------
# Import new music
# ---------------------------

for folder_name in sorted(os.listdir(INPUT_DIR)):

    folder_path = os.path.join(INPUT_DIR, folder_name)
    if not os.path.isdir(folder_path):
        continue

    df_folder = f"{folder_index:02d}"
    df_folder_path = os.path.join(OUTPUT_DIR, df_folder)
    os.makedirs(df_folder_path, exist_ok=True)

    track_index = 1

    for file in sorted(os.listdir(folder_path)):

        if not file.lower().endswith(".mp3"):
            continue

        src = os.path.join(folder_path, file)
        dst = os.path.join(df_folder_path, f"{track_index:03d}.mp3")

        shutil.copy(src, dst)

        try:
            audio = MP3(src, ID3=EasyID3)
            title  = audio.get("title",  ["Unknown"])[0]
            artist = audio.get("artist", ["Unknown"])[0]
            album  = audio.get("album",  ["Unknown"])[0]
            length = int(audio.info.length)
        except Exception:
            title = artist = album = "Unknown"
            length = 0

        songs.append({
            "folder": folder_index,
            "track": track_index,
            "title": title.replace('"', "'"),
            "artist": artist.replace('"', "'"),
            "album": album.replace('"', "'"),
            "length": length
        })

        track_index += 1

    folder_index += 1


# ---------------------------
# Rewrite header
# ---------------------------

with open(header_path, "w", encoding="utf-8") as f:

    f.write("#pragma once\n\n")
    f.write("#include <stdint.h>\n\n")

    f.write("typedef struct {\n")
    f.write("  uint8_t folder;\n")
    f.write("  uint8_t track;\n")
    f.write("  const char* title;\n")
    f.write("  const char* artist;\n")
    f.write("  const char* album;\n")
    f.write("  uint16_t length;\n")
    f.write("} Song;\n\n")

    f.write(f"#define SONG_COUNT {len(songs)}\n\n")
    f.write("const Song songs[SONG_COUNT] = PROGMEM {\n")

    for s in songs:
        f.write(
            f'  {{ {s["folder"]}, {s["track"]}, '
            f'"{s["title"]}", "{s["artist"]}", "{s["album"]}", {s["length"]} }},\n'
        )

    f.write("};\n")

print("New music merged successfully!")