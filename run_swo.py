import subprocess
import time
import os

Import("env")

# Спрощена та універсальна сигнатура для SCons
def after_upload(source, target, env):
    print("\n[SWO Auto-Runner] Upload finished! Starting SWO Pipeline...")
    
    # 1. Шлях до OpenOCD, який використовує сам PlatformIO
    platform = env.PioPlatform()
    openocd_dir = platform.get_package_dir("tool-openocd")
    openocd_path = os.path.join(openocd_dir, "bin", "openocd.exe" if os.name == "nt" else "openocd")

    # 2. Команда для OpenOCD (без запуску GDB-сервера)
    openocd_cmd = [
        openocd_path,
        "-f", "interface/stlink.cfg",
        "-f", "target/stm32f1x.cfg",
        "-c", "init",
        "-c", "stm32f1x.tpiu configure -protocol uart -traceclk 72000000 -pin-freq 2000000 -formatter off -output :22888",
        "-c", "stm32f1x.tpiu enable",
        "-c", "itm ports on"
    ]

    print("[SWO Auto-Runner] Launching OpenOCD background process...")
    creationflags = subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0
    
    # Зберігаємо процес в env, щоб він жив після завершення функції
    env.openocd_process = subprocess.Popen(
        openocd_cmd, 
        stdout=subprocess.DEVNULL, 
        stderr=subprocess.DEVNULL, 
        creationflags=creationflags
    )
    
    # Даємо OpenOCD 1 секунду розігрітися
    time.sleep(1)

    # 3. Запускаємо ваш swo_parser.py
    print("[SWO Auto-Runner] Connecting swo_parser.py to port 22888...\n")
    try:
        # Передаємо керування терміналом у Python-скрипт
        subprocess.run(["python", "swo_parser.py"])
    except KeyboardInterrupt:
        print("\n[SWO Auto-Runner] Stopping SWO Monitor...")
    finally:
        # Коли натиснете Ctrl+C, глушимо OpenOCD
        if hasattr(env, 'openocd_process'):
            env.openocd_process.terminate()

# Реєструємо хук з правильним викликом
env.AddPostAction("upload", after_upload)

env.AddCustomTarget(
    name="monitor_swo",
    actions=[after_upload],
    dependencies=None,
    title="SWO Monitor",
    description="Start OpenOCD and SWO Parser without flashing the target"
)