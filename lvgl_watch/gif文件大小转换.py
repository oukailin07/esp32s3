import os
from PIL import Image, ImageSequence

src_root = 'Emoji'
dst_root = 'Emoji5'
target_size = (320, 240)

for root, dirs, files in os.walk(src_root):
    for file in files:
        if file.lower().endswith('.gif'):
            src_path = os.path.join(root, file)
            relative_path = os.path.relpath(root, src_root)
            dst_dir = os.path.join(dst_root, relative_path)
            os.makedirs(dst_dir, exist_ok=True)
            dst_path = os.path.join(dst_dir, file)

            try:
                with Image.open(src_path) as im:
                    # 初始化黑色背景画布（RGB模式）
                    canvas = Image.new('RGB', im.size, (0, 0, 0))
                    frames = []
                    durations = []
                    loop = im.info.get('loop', 0)

                    for frame in ImageSequence.Iterator(im):
                        frame_rgba = frame.convert('RGBA')
                        # 将透明图粘贴到黑色背景上，得到 RGB 图像
                        composite = Image.new('RGB', im.size, (0, 0, 0))
                        composite.paste(frame_rgba, mask=frame_rgba.split()[3])  # alpha通道做mask
                        resized = composite.resize(target_size, Image.Resampling.LANCZOS)
                        frames.append(resized)
                        durations.append(im.info.get('duration', 100))

                    # 保存为不透明 GIF，彻底避免透明->白底
                    frames[0].save(
                        dst_path,
                        save_all=True,
                        append_images=frames[1:],
                        loop=loop,
                        duration=durations,
                        disposal=2
                    )
                    print(f"✅ 转换完成：{src_path} -> {dst_path}")
            except Exception as e:
                print(f"❌ 处理失败：{src_path}，错误：{e}")
