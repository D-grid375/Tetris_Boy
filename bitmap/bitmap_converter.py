from PIL import Image

# 入力画像
img = Image.open("C:\\dev\\raspi_pico_w\\trunk\\tools\\raspi_bitmap\\tetris_bitmap_def_restart_message_bold.png").convert("L")  # Grayscale
img = img.resize((128, 128))  # 念のためサイズ調整

threshold = 128  # 明度128を閾値に2値化
bitmap = [[0, 0] for _ in range(128)]  # bitmap_128_t の構造に合わせた初期化

for y in range(128):
    for x in range(128):
        pixel = img.getpixel((x, y))
        bit = 1 if pixel < threshold else 0
        index = 0 if x < 64 else 1
        shift = 63 - (x % 64)
        bitmap[y][index] |= (bit << shift)

# 結果をCの初期化配列として出力
print("const bitmap_128_t tetris_bitmap_def_restart_message_bold = {")
for row in bitmap:
    print(f"    {{0x{row[0]:016X}, 0x{row[1]:016X}}},")
print("};")