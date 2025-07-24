from PIL import Image
from pathlib import Path

# 대상 폴더 경로
folder = Path(r"D:\_doc\LessonUnreal\git.3dapi\DirectX12-Spine-Game\2d_shooting\asset\sprite\space_shooter")

# PNG 파일 목록 순회
for file in folder.glob("*.png"):
    img = Image.open(file).convert("RGBA")
    alpha = img.getchannel("A")
    bbox = alpha.getbbox()

    if bbox is None:
        print(f"[SKIP] {file.name}: 투명 영역 없음")
        continue

    # 가로는 그대로, 세로만 자르기
    left = 0
    right = img.width
    top = bbox[1]
    bottom = bbox[3]
    cropped = img.crop((left, top, right, bottom))

    # 새 파일명
    new_file = file.with_stem("trimmed_" + file.stem)

    # 저장
    cropped.save(new_file)
    print(f"[OK] {file.name} -> {new_file.name}")
